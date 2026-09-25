#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "recomp.h"
#include "ultramodern/ultra64.h"
#include "ultramodern/ultramodern.hpp"
#include "ultramodern/input.hpp"

// Virtual Controller Pak for BattleTanx, backed by files next to the save file.
//
// The stock N64ModernRuntime reports "no pak" (PFS_ERR_NOPACK) from every osPfs* call
// (librecomp/src/pak.cpp), so the game complains the Controller Pak is missing. This file
// defines every osPfs*_recomp that pak.cpp defines, so the linker never pulls pak.cpp's
// object out of librecomp.a and these implementations are used instead.
//
// Ported from the virtual PFS the forked runtime carried (ultramodern/src/pfs.cpp and
// librecomp/src/pfs.cpp). The filesystem isn't emulated: each allocated pak file is a host
// file, and a small header file records each file's owner and name.

#ifndef PFS_ERR_NOPACK
#define PFS_ERR_NOPACK       1
#define PFS_ERR_INVALID      5
#define PFS_DIR_FULL         8
#define PFS_ERR_DEVICE       11
#endif
#ifndef PFS_ERR_CONTRFAIL
#define PFS_ERR_CONTRFAIL    4 // CONT_OVERRUN_ERROR
#endif
#ifndef PFS_FILE_NAME_LEN
#define PFS_FILE_NAME_LEN    16
#define PFS_FILE_EXT_LEN     4
#define PFS_BLOCKSIZE        32
#define PFS_ONE_PAGE         8
#endif
#ifndef PFS_READ
#define PFS_READ             0
#define PFS_WRITE            1
#endif
#ifndef PFS_INITIALIZED
#define PFS_INITIALIZED      0x1
#endif

#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ARRLEN(x) (sizeof(x) / sizeof((x)[0]))
#define DEF_DIR_PAGES 2
#define MAX_FILES 16
#define MAX_PAGES 123 // 128 total, 5 reserved for filesystem

// libultra OSPfsState as it sits in (byteswapped) RDRAM.
typedef struct {
    u32 file_size;
    u32 game_code;
    char ext_name_0[2]; // ext_name starts on a halfword boundary
    u16 company_code;
    char game_name_0[2];
    char ext_name_1[2];
    char game_name_1[12];
    char padding[2];
    char game_name_2[2];
} BtxOSPfsState; // size = 0x20

// main.cpp: which input device is connected to a controller channel.
ultramodern::input::connected_device_info_t get_connected_device_info(int controller_num);

// Arguments past a3 live on the MIPS stack at sp+0x10, sp+0x14, ...
static inline u32 stack_arg(uint8_t* rdram, recomp_context* ctx, int index) {
    return (u32)MEM_W(0x10 + 4 * (index - 4), ctx->r29);
}

/* Host-side storage */

struct pfs_header_t { // same layout as OSPfsState, but not byteswapped
    uint32_t file_size;
    uint32_t game_code;
    uint16_t company_code;
    std::array<char, PFS_FILE_EXT_LEN> ext_name;
    std::array<char, PFS_FILE_NAME_LEN> game_name;
    uint16_t padding;

    pfs_header_t() = default;
    pfs_header_t(uint32_t fs, uint32_t gc, uint16_t cc, const char* en, const char* gn)
        : file_size{fs}, game_code{gc}, company_code{cc}, ext_name{}, game_name{}, padding{} {
        std::memcpy(ext_name.data(), en, sizeof(ext_name));
        std::memcpy(game_name.data(), gn, sizeof(game_name));
    }
    bool valid() const {
        return game_code != 0 && company_code != 0;
    }
    bool compare(uint32_t gcode, uint16_t ccode, const char* ename, const char* gname) const {
        return game_code == gcode && company_code == ccode &&
            std::memcmp(ext_name.data(), ename, sizeof(ext_name)) == 0 &&
            std::memcmp(game_name.data(), gname, sizeof(game_name)) == 0;
    }
};

static std::filesystem::path pfs_base_path() {
    return ultramodern::get_save_file_path().parent_path();
}

static std::filesystem::path pfs_header_path() {
    return pfs_base_path() / "controllerpak_header.bin";
}

static std::filesystem::path pfs_file_path(size_t file_no) {
    return pfs_base_path() / ("controllerpak_file_" + std::to_string(file_no) + ".bin");
}

static bool pfs_header_alloc() {
    if (!std::filesystem::exists(pfs_header_path())) {
        std::filesystem::create_directories(pfs_base_path());
        std::vector<char> zero_block(MAX_FILES * sizeof(pfs_header_t));
        std::ofstream out(pfs_header_path(), std::ios::binary | std::ios::out | std::ios::trunc);
        out.write(zero_block.data(), zero_block.size());
        return out.good();
    }
    return true;
}

static bool pfs_header_write(int file_no, const pfs_header_t& hdr) {
    std::fstream out(pfs_header_path(), std::ios::binary | std::ios::out | std::ios::in);
    if (out.is_open() && out.good()) {
        out.seekp(file_no * sizeof(pfs_header_t), std::ios::beg);
        out.write((const char*)&hdr.file_size, sizeof(hdr.file_size));
        out.write((const char*)&hdr.game_code, sizeof(hdr.game_code));
        out.write((const char*)&hdr.company_code, sizeof(hdr.company_code));
        out.write(hdr.ext_name.data(), hdr.ext_name.size());
        out.write(hdr.game_name.data(), hdr.game_name.size());
        out.write((const char*)&hdr.padding, sizeof(hdr.padding));
    }
    return out.good();
}

static bool pfs_header_read(int file_no, pfs_header_t& hdr) {
    hdr = {};
    std::ifstream in(pfs_header_path(), std::ios::binary | std::ios::in);
    if (in.is_open() && in.good()) {
        in.seekg(file_no * sizeof(pfs_header_t), std::ios::beg);
        in.read((char*)&hdr.file_size, sizeof(hdr.file_size));
        in.read((char*)&hdr.game_code, sizeof(hdr.game_code));
        in.read((char*)&hdr.company_code, sizeof(hdr.company_code));
        in.read(hdr.ext_name.data(), hdr.ext_name.size());
        in.read(hdr.game_name.data(), hdr.game_name.size());
        in.read((char*)&hdr.padding, sizeof(hdr.padding));
    }
    return in.good();
}

static bool pfs_file_alloc(int file_no, int nbytes) {
    std::vector<char> zero_block(ALIGN_UP(nbytes, PFS_ONE_PAGE * PFS_BLOCKSIZE));
    std::ofstream out(pfs_file_path(file_no), std::ios::binary | std::ios::out | std::ios::trunc);
    if (out.is_open() && out.good()) {
        out.write(zero_block.data(), zero_block.size());
    }
    return out.good();
}

static bool pfs_file_write(int file_no, int offset, const char* data, int nbytes) {
    std::fstream out(pfs_file_path(file_no), std::ios::binary | std::ios::out | std::ios::in);
    if (out.is_open() && out.good()) {
        out.seekp(offset, std::ios::beg);
        out.write(data, nbytes);
    }
    return out.good();
}

static bool pfs_file_read(int file_no, int offset, char* data, int nbytes) {
    std::ifstream in(pfs_file_path(file_no), std::ios::binary | std::ios::in);
    if (in.is_open() && in.good()) {
        in.seekg(offset, std::ios::beg);
        in.read(data, nbytes);
    }
    return in.good();
}

/* Controller Pak */

// Any connected controller has a Controller Pak inserted. (The stock runtime's Pak enum has no
// ControllerPak value, so this is decided here rather than through the input callbacks.)
static s32 pfs_get_status(int channel) {
    if (get_connected_device_info(channel).connected_device != ultramodern::input::Device::Controller) {
        return PFS_ERR_CONTRFAIL;
    }
    pfs_header_alloc();
    return 0;
}

static void pfs_init_id(OSPfs* pfs) {
    // No real filesystem: mimic what libultra's __osGetId leaves behind.
    pfs->version = 0;
    pfs->banks = 1;
    pfs->activebank = 0;
    pfs->inode_start_page = 1 + DEF_DIR_PAGES + (2 * pfs->banks);
    pfs->dir_size = DEF_DIR_PAGES * PFS_ONE_PAGE;
    pfs->inode_table = 1 * PFS_ONE_PAGE;
    pfs->minode_table = (1 + pfs->banks) * PFS_ONE_PAGE;
    pfs->dir_table = pfs->minode_table + (pfs->banks * PFS_ONE_PAGE);
    std::memset(pfs->id, 0, ARRLEN(pfs->id));
    std::memset(pfs->label, 0, ARRLEN(pfs->label));
}

static void copy_from_rdram(uint8_t* rdram, gpr addr, u8* dst, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dst[i] = MEM_B(i, addr);
    }
}

extern "C" void osPfsInitPak_recomp(uint8_t* rdram, recomp_context* ctx) {
    PTR(OSMesgQueue) mq = (PTR(OSMesgQueue))ctx->r4;
    OSPfs* pfs = TO_PTR(OSPfs, ctx->r5);
    int channel = (int)ctx->r6;

    s32 status = pfs_get_status(channel);
    if (status == 0) {
        pfs->queue = mq;
        pfs->channel = channel;
        pfs->status = 0;
        pfs_init_id(pfs);
        pfs->status |= PFS_INITIALIZED;
    }
    ctx->r2 = status;
}

extern "C" void osPfsRepairId_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    ctx->r2 = 0;
}

extern "C" void osPfsChecker_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    ctx->r2 = 0;
}

extern "C" void osPfsAllocateFile_recomp(uint8_t* rdram, recomp_context* ctx) {
    u16 company_code = (u16)ctx->r5;
    u32 game_code = (u32)ctx->r6;
    gpr game_name_addr = ctx->r7;
    gpr ext_name_addr = (gpr)(int32_t)stack_arg(rdram, ctx, 4);
    int nbytes = (int)stack_arg(rdram, ctx, 5);
    s32* file_no = TO_PTR(s32, stack_arg(rdram, ctx, 6));

    if (company_code == 0 || game_code == 0) {
        ctx->r2 = PFS_ERR_INVALID;
        return;
    }
    u8 game_name[PFS_FILE_NAME_LEN];
    u8 ext_name[PFS_FILE_EXT_LEN];
    copy_from_rdram(rdram, game_name_addr, game_name, sizeof(game_name));
    copy_from_rdram(rdram, ext_name_addr, ext_name, sizeof(ext_name));

    pfs_header_t hdr{};
    int free_file_index = MAX_FILES;
    for (int i = 0; i < MAX_FILES; i++) {
        pfs_header_read(i, hdr);
        if (!hdr.valid()) {
            free_file_index = i;
            break;
        }
    }
    if (free_file_index == MAX_FILES) {
        ctx->r2 = PFS_DIR_FULL;
        return;
    }
    if (!pfs_header_write(free_file_index, pfs_header_t{(uint32_t)nbytes, game_code, company_code, (const char*)ext_name, (const char*)game_name}) ||
        !pfs_file_alloc(free_file_index, nbytes)) {
        ctx->r2 = PFS_ERR_INVALID;
        return;
    }
    *file_no = free_file_index;
    ctx->r2 = 0;
}

extern "C" void osPfsFindFile_recomp(uint8_t* rdram, recomp_context* ctx) {
    u16 company_code = (u16)ctx->r5;
    u32 game_code = (u32)ctx->r6;
    gpr game_name_addr = ctx->r7;
    gpr ext_name_addr = (gpr)(int32_t)stack_arg(rdram, ctx, 4);
    s32* file_no = TO_PTR(s32, stack_arg(rdram, ctx, 5));

    if (company_code == 0 || game_code == 0) {
        ctx->r2 = PFS_ERR_INVALID;
        return;
    }
    u8 game_name[PFS_FILE_NAME_LEN];
    u8 ext_name[PFS_FILE_EXT_LEN];
    copy_from_rdram(rdram, game_name_addr, game_name, sizeof(game_name));
    copy_from_rdram(rdram, ext_name_addr, ext_name, sizeof(ext_name));

    pfs_header_t hdr{};
    for (int i = 0; i < MAX_FILES; i++) {
        pfs_header_read(i, hdr);
        if (hdr.compare(game_code, company_code, (const char*)ext_name, (const char*)game_name)) {
            *file_no = i;
            ctx->r2 = 0;
            return;
        }
    }
    ctx->r2 = PFS_ERR_INVALID;
}

extern "C" void osPfsDeleteFile_recomp(uint8_t* rdram, recomp_context* ctx) {
    u16 company_code = (u16)ctx->r5;
    u32 game_code = (u32)ctx->r6;
    gpr game_name_addr = ctx->r7;
    gpr ext_name_addr = (gpr)(int32_t)stack_arg(rdram, ctx, 4);

    if (company_code == 0 || game_code == 0) {
        ctx->r2 = PFS_ERR_INVALID;
        return;
    }
    u8 game_name[PFS_FILE_NAME_LEN];
    u8 ext_name[PFS_FILE_EXT_LEN];
    copy_from_rdram(rdram, game_name_addr, game_name, sizeof(game_name));
    copy_from_rdram(rdram, ext_name_addr, ext_name, sizeof(ext_name));

    pfs_header_t hdr{};
    for (int i = 0; i < MAX_FILES; i++) {
        pfs_header_read(i, hdr);
        if (hdr.compare(game_code, company_code, (const char*)ext_name, (const char*)game_name)) {
            pfs_header_write(i, pfs_header_t{});
            std::filesystem::remove(pfs_file_path(i));
            ctx->r2 = 0;
            return;
        }
    }
    ctx->r2 = PFS_ERR_INVALID;
}

extern "C" void osPfsReadWriteFile_recomp(uint8_t* rdram, recomp_context* ctx) {
    s32 file_no = (s32)ctx->r5;
    u8 flag = (u8)ctx->r6;
    int offset = (int)ctx->r7;
    int nbytes = (int)stack_arg(rdram, ctx, 4);
    gpr buffer_addr = (gpr)(int32_t)stack_arg(rdram, ctx, 5);

    if (!std::filesystem::exists(pfs_file_path(file_no))) {
        ctx->r2 = PFS_ERR_INVALID;
        return;
    }
    const auto file_size = std::filesystem::file_size(pfs_file_path(file_no));
    if (offset % PFS_BLOCKSIZE || nbytes % PFS_BLOCKSIZE || (uint64_t)(offset + nbytes) > file_size) {
        ctx->r2 = PFS_ERR_INVALID;
        return;
    }

    std::vector<u8> data(nbytes);
    if (flag == PFS_WRITE) {
        copy_from_rdram(rdram, buffer_addr, data.data(), nbytes);
        if (!pfs_file_write(file_no, offset, (const char*)data.data(), nbytes)) {
            ctx->r2 = PFS_ERR_INVALID;
            return;
        }
    }
    else if (flag == PFS_READ) {
        if (!pfs_file_read(file_no, offset, (char*)data.data(), nbytes)) {
            ctx->r2 = PFS_ERR_INVALID;
            return;
        }
        for (int i = 0; i < nbytes; i++) {
            MEM_B(i, buffer_addr) = data[i];
        }
    }
    ctx->r2 = 0;
}

extern "C" void osPfsFileState_recomp(uint8_t* rdram, recomp_context* ctx) {
    s32 file_no = (s32)ctx->r5;
    BtxOSPfsState* state = TO_PTR(BtxOSPfsState, ctx->r6);

    if (!std::filesystem::exists(pfs_file_path(file_no))) {
        ctx->r2 = PFS_ERR_INVALID;
        return;
    }
    pfs_header_t hdr{};
    pfs_header_read(file_no, hdr);

    state->file_size = hdr.file_size;
    state->company_code = hdr.company_code;
    state->game_code = hdr.game_code;
    // The ext/game name bytes straddle halfword boundaries in OSPfsState; copy them byteswapped.
    const char* src = (const char*)&hdr;
    char* dst = (char*)state;
    for (int i = 0; i < 20; i++) {
        dst[(i + 10) ^ 3] = src[i + 10];
    }
    ctx->r2 = 0;
}

extern "C" void osPfsFreeBlocks_recomp(uint8_t* rdram, recomp_context* ctx) {
    s32* bytes_not_used = TO_PTR(s32, ctx->r5);

    s32 pages_used = 0;
    pfs_header_t hdr{};
    for (int i = 0; i < MAX_FILES; i++) {
        pfs_header_read(i, hdr);
        if (hdr.valid()) {
            pages_used += hdr.file_size >> 8;
        }
    }
    *bytes_not_used = (MAX_PAGES - pages_used) << 8;
    ctx->r2 = 0;
}

extern "C" void osPfsNumFiles_recomp(uint8_t* rdram, recomp_context* ctx) {
    s32* max_files = TO_PTR(s32, ctx->r5);
    s32* files_used = TO_PTR(s32, ctx->r6);

    s32 num_files = 0;
    pfs_header_t hdr{};
    for (int i = 0; i < MAX_FILES; i++) {
        pfs_header_read(i, hdr);
        if (hdr.valid()) {
            num_files++;
        }
    }
    *max_files = MAX_FILES;
    *files_used = num_files;
    ctx->r2 = 0;
}
