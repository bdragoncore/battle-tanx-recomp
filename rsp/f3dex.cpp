#include "librecomp/rsp.hpp"
#include "librecomp/rsp_vu_impl.hpp"
RspExitReason f3dex(uint8_t* rdram, [[maybe_unused]] uint32_t ucode_addr) {
    uint32_t           r1 = 0,  r2 = 0,  r3 = 0,  r4 = 0,  r5 = 0,  r6 = 0,  r7 = 0;
    uint32_t  r8 = 0,  r9 = 0, r10 = 0, r11 = 0, r12 = 0, r13 = 0, r14 = 0, r15 = 0;
    uint32_t r16 = 0, r17 = 0, r18 = 0, r19 = 0, r20 = 0, r21 = 0, r22 = 0, r23 = 0;
    uint32_t r24 = 0, r25 = 0, r26 = 0, r27 = 0, r28 = 0, r29 = 0, r30 = 0, r31 = 0;
    uint32_t dma_mem_address = 0, dma_dram_address = 0, jump_target = 0;
    const char * debug_file = NULL; int debug_line = 0;
    RSP rsp{};
    // The real N64 SDK's shared RSP boot stub occupies IMEM [0x1000,0x1080) (confirmed
    // from an RSP PC trace) and is merged in below (labels
    // L_1000..L_10C4) since F3DEX's own code genuinely calls back into it (shared
    // DMA-wait/status-check subroutines) even after the boot handoff, not just once at
    // startup. It always runs before F3DEX's own text (which starts at L_1080, matching
    // the boot stub's DMA destination) and performs the setup -- including the vector
    // constant registers v30/v31 F3DEX's own code depends on -- that this seam can't
    // otherwise replicate; the explicit seed below is deliberately left in as a fallback
    // in case some earlier stage (before this seam even starts) also depends on them.
    {
        static constexpr uint16_t kV30[8] = { 0xFFFF, 0xE000, 0xC000, 0xA000, 0x8000, 0x6000, 0x4000, 0x7FFF };
        static constexpr uint16_t kV31[8] = { 0x4000, 0x7FFF, 0x0800, 0x0020, 0xFFFF, 0x0002, 0x0001, 0x0000 };
        for (int i = 0; i < 8; i++) {
            rsp.vpu.r[30].element(i) = kV30[i];
            rsp.vpu.r[31].element(i) = kV31[i];
        }
    }
    r1 = 0xFC0;
L_1000:
    // j           L_1064  (boot stub entry; f3dex_boot.us.rev0.toml / rsp/f3dex_boot.cpp)
    goto L_1064;
L_1008:
    // lw          $2, 0x10($1)
    r2 = RSP_MEM_W_LOAD(0X10, r1);
    // addi        $3, $zero, 0xF7F
    r3 = RSP_ADD32(0, 0XF7F);
    // addi        $7, $zero, 0x1080
    r7 = RSP_ADD32(0, 0X1080);
    // mtc0        $7, SP_MEM_ADDR
    SET_DMA_MEM(r7);
    // mtc0        $2, SP_DRAM_ADDR
    SET_DMA_DRAM(r2);
    // mtc0        $3, SP_RD_LEN
    DO_DMA_READ(r3);
L_1020:
    // mfc0        $4, SP_DMA_BUSY
    r4 = 0;
    // bne         $4, $zero, L_1020
    if (r4 != 0) {
        // nop

        goto L_1020;
    }
    // nop

    // jal         0x103C
    r31 = 0x1034;
    // nop

    goto L_103C;
    // nop

L_1034:
    // jr          $7
    jump_target = r7;
    debug_file = __FILE__; debug_line = __LINE__;
    // mtc0        $zero, SP_SEMAPHORE
    goto do_indirect_jump;
    // mtc0        $zero, SP_SEMAPHORE
L_103C:
    // mfc0        $8, SP_STATUS
    r8 = get_sp_status();
    // andi        $8, $8, 0x80
    r8 = r8 & 0X80;
    // bne         $8, $zero, L_1050
    if (r8 != 0) {
        // nop

        goto L_1050;
    }
    // nop

    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // mtc0        $zero, SP_SEMAPHORE
    goto do_indirect_jump;
L_1050:
    // mtc0        $zero, SP_SEMAPHORE
    // ori         $8, $zero, 0x5200
    r8 = 0 | 0X5200;
    // mtc0        $8, SP_STATUS
    set_sp_status(r8);
L_1058:
    // break       0
    return RspExitReason::Broke;
    // nop

L_1064:
    // lw          $2, 0x4($1)
    r2 = RSP_MEM_W_LOAD(0X4, r1);
    // andi        $2, $2, 0x2
    r2 = r2 & 0X2;
    // beq         $2, $zero, L_108C
    if (r2 == 0) {
        // nop

        goto L_108C;
    }
L_1070:
    // nop (delay slot / fallthrough of the beq above)

    // jal         0x103C
    r31 = 0x107C;
    // nop

    goto L_103C;
    // nop

L_107C:
    // mfc0        $2, DPC_STATUS
    r2 = get_dpc_status();
    // andi        $2, $2, 0x100
    r2 = r2 & 0X100;
    // bgtz        $2, L_103C
    if (RSP_SIGNED(r2) > 0) {
        // nop

        goto L_103C;
    }
    // nop

L_108C:
    // lw          $2, 0x18($1)
    r2 = RSP_MEM_W_LOAD(0X18, r1);
    // lw          $3, 0x1C($1)
    r3 = RSP_MEM_W_LOAD(0X1C, r1);
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
L_1098:
    // mfc0        $30, SP_DMA_FULL
    r30 = 0;
    // bne         $30, $zero, L_1098
    if (r30 != 0) {
        // nop

        goto L_1098;
    }
    // nop

    // mtc0        $zero, SP_MEM_ADDR
    SET_DMA_MEM(0);
    // mtc0        $2, SP_DRAM_ADDR
    SET_DMA_DRAM(r2);
    // mtc0        $3, SP_RD_LEN
    DO_DMA_READ(r3);
L_10B0:
    // mfc0        $4, SP_DMA_BUSY
    r4 = 0;
    // bne         $4, $zero, L_10B0
    if (r4 != 0) {
        // nop

        goto L_10B0;
    }
    // nop

    // jal         0x103C
    r31 = 0x10C4;
    // nop

    goto L_103C;
    // nop

L_10C4:
    // j           L_1008
    goto L_1008;

L_1080:
    // j           L_1738
    // addi        $29, $zero, 0x110
    r29 = RSP_ADD32(0, 0X110);
    goto L_1738;
    // addi        $29, $zero, 0x110
    r29 = RSP_ADD32(0, 0X110);
    // jal         0x1FD4
    r31 = 0x1090;
    // add         $20, $zero, $22
    r20 = RSP_ADD32(0, r22);
    goto L_1FD4;
    // add         $20, $zero, $22
    r20 = RSP_ADD32(0, r22);
L_1090:
    // lh          $2, 0xB8($1)
    r2 = RSP_MEM_H_LOAD(0XB8, r1);
    // jr          $2
    jump_target = r2;
    debug_file = __FILE__; debug_line = __LINE__;
    // srl         $2, $25, 23
    r2 = S32(U32(r25) >> 23);
    goto do_indirect_jump;
    // srl         $2, $25, 23
    r2 = S32(U32(r25) >> 23);
L_109C:
    // mfc0        $2, SP_STATUS
    r2 = get_sp_status();
    // andi        $2, $2, 0x80
    r2 = r2 & 0X80;
    // bne         $2, $zero, L_10C0
    if (r2 != 0) {
        // lh          $21, 0x26($zero)
        r21 = RSP_MEM_H_LOAD(0X26, 0);
        goto L_10C0;
    }
    // lh          $21, 0x26($zero)
    r21 = RSP_MEM_H_LOAD(0X26, 0);
L_10AC:
    // bne         $28, $27, L_1064
    if (r28 != r27) {
        // lw          $25, 0x0($27)
        r25 = RSP_MEM_W_LOAD(0X0, r27);
        goto L_1064;
    }
    // lw          $25, 0x0($27)
    r25 = RSP_MEM_W_LOAD(0X0, r27);
    // j           L_10C8
    // lh          $ra, 0x104($zero)
    r31 = RSP_MEM_H_LOAD(0X104, 0);
    goto L_10C8;
    // lh          $ra, 0x104($zero)
    r31 = RSP_MEM_H_LOAD(0X104, 0);
L_10BC:
    // lh          $21, 0xB6($zero)
    r21 = RSP_MEM_H_LOAD(0XB6, 0);
L_10C0:
    // j           L_10F0
    // ori         $30, $zero, 0x20
    r30 = 0 | 0X20;
    goto L_10F0;
    // ori         $30, $zero, 0x20
    r30 = 0 | 0X20;
L_10C8:
    // addi        $28, $zero, 0xAE0
    r28 = RSP_ADD32(0, 0XAE0);
    // add         $21, $zero, $ra
    r21 = RSP_ADD32(0, r31);
    // addi        $20, $zero, 0x9A0
    r20 = RSP_ADD32(0, 0X9A0);
    // add         $19, $zero, $26
    r19 = RSP_ADD32(0, r26);
    // addi        $26, $26, 0x140
    r26 = RSP_ADD32(r26, 0X140);
    // jal         0x1FD4
    r31 = 0x10E4;
    // addi        $18, $zero, 0x13F
    r18 = RSP_ADD32(0, 0X13F);
    goto L_1FD4;
    // addi        $18, $zero, 0x13F
    r18 = RSP_ADD32(0, 0X13F);
L_10E4:
    // jr          $21
    jump_target = r21;
    debug_file = __FILE__; debug_line = __LINE__;
    // addi        $27, $zero, 0x9A0
    r27 = RSP_ADD32(0, 0X9A0);
    goto do_indirect_jump;
    // addi        $27, $zero, 0x9A0
    r27 = RSP_ADD32(0, 0X9A0);
L_10EC:
    // add         $21, $zero, $ra
    r21 = RSP_ADD32(0, r31);
L_10F0:
    // lw          $19, 0x0($30)
    r19 = RSP_MEM_W_LOAD(0X0, r30);
    // lh          $18, 0x4($30)
    r18 = RSP_MEM_H_LOAD(0X4, r30);
    // jal         0x1FD4
    r31 = 0x1100;
    // lh          $20, 0x6($30)
    r20 = RSP_MEM_H_LOAD(0X6, r30);
    goto L_1FD4;
    // lh          $20, 0x6($30)
    r20 = RSP_MEM_H_LOAD(0X6, r30);
L_1100:
    // jal         0x1FC4
    r31 = 0x1108;
    // nop

    goto L_1FC4;
    // nop

L_1108:
    // jr          $21
    jump_target = r21;
    debug_file = __FILE__; debug_line = __LINE__;
    // srl         $12, $19, 22
    r12 = S32(U32(r19) >> 22);
    goto do_indirect_jump;
L_110C:
    // srl         $12, $19, 22
    r12 = S32(U32(r19) >> 22);
L_1110:
    // andi        $12, $12, 0x3C
    r12 = r12 & 0X3C;
    // lw          $12, 0x160($12)
    r12 = RSP_MEM_W_LOAD(0X160, r12);
    // sll         $19, $19, 8
    r19 = S32(r19) << 8;
    // srl         $19, $19, 8
    r19 = S32(U32(r19) >> 8);
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // add         $19, $19, $12
    r19 = RSP_ADD32(r19, r12);
    goto do_indirect_jump;
    // add         $19, $19, $12
    r19 = RSP_ADD32(r19, r12);
L_1128:
    // add         $21, $zero, $ra
    r21 = RSP_ADD32(0, r31);
L_112C:
    // lw          $19, 0x18($29)
    r19 = RSP_MEM_W_LOAD(0X18, r29);
    // addi        $18, $23, -0xCE0
    r18 = RSP_ADD32(r23, -0XCE0);
    // lw          $23, 0x44($29)
    r23 = RSP_MEM_W_LOAD(0X44, r29);
L_1138:
    // blez        $18, L_11A4
    if (RSP_SIGNED(r18) <= 0) {
        // add         $20, $19, $18
        r20 = RSP_ADD32(r19, r18);
        goto L_11A4;
    }
    // add         $20, $19, $18
    r20 = RSP_ADD32(r19, r18);
    // sub         $20, $23, $20
    r20 = RSP_SUB32(r23, r20);
    // bgez        $20, L_1168
    if (RSP_SIGNED(r20) >= 0) {
        // mfc0        $20, DPC_STATUS
        r20 = get_dpc_status();
        goto L_1168;
    }
L_1148:
    // mfc0        $20, DPC_STATUS
    r20 = get_dpc_status();
    // andi        $20, $20, 0x400
    r20 = r20 & 0X400;
    // bne         $20, $zero, L_1148
    if (r20 != 0) {
        // mfc0        $23, DPC_CURRENT
        r23 = get_dpc_current();
        goto L_1148;
    }
L_1154:
    // mfc0        $23, DPC_CURRENT
    r23 = get_dpc_current();
    // lw          $19, 0x40($29)
    r19 = RSP_MEM_W_LOAD(0X40, r29);
    // beq         $23, $19, L_1154
    if (r23 == r19) {
        // nop

        goto L_1154;
    }
L_1160:
    // nop

    // mtc0        $19, DPC_START
    set_dpc_start(r19);
L_1168:
    // mfc0        $23, DPC_CURRENT
    r23 = get_dpc_current();
    // sub         $20, $19, $23
    r20 = RSP_SUB32(r19, r23);
    // bgez        $20, L_1184
    if (RSP_SIGNED(r20) >= 0) {
        // add         $20, $19, $18
        r20 = RSP_ADD32(r19, r18);
        goto L_1184;
    }
    // add         $20, $19, $18
    r20 = RSP_ADD32(r19, r18);
    // sub         $20, $20, $23
    r20 = RSP_SUB32(r20, r23);
    // bgez        $20, L_1168
    if (RSP_SIGNED(r20) >= 0) {
        // nop
    
        goto L_1168;
    }
    // nop

L_1184:
    // add         $23, $19, $18
    r23 = RSP_ADD32(r19, r18);
    // addi        $18, $18, -0x1
    r18 = RSP_ADD32(r18, -0X1);
    // addi        $20, $zero, 0xCE0
    r20 = RSP_ADD32(0, 0XCE0);
    // jal         0x1FD8
    r31 = 0x1198;
    // addi        $17, $zero, 0x1
    r17 = RSP_ADD32(0, 0X1);
    goto L_1FD8;
    // addi        $17, $zero, 0x1
    r17 = RSP_ADD32(0, 0X1);
L_1198:
    // jal         0x1FC4
    r31 = 0x11A0;
    // sw          $23, 0x18($29)
    RSP_MEM_W_STORE(0X18, r29, r23);
    goto L_1FC4;
    // sw          $23, 0x18($29)
    RSP_MEM_W_STORE(0X18, r29, r23);
L_11A0:
    // mtc0        $23, DPC_END
    set_dpc_end(r23);
L_11A4:
    // jr          $21
    jump_target = r21;
    debug_file = __FILE__; debug_line = __LINE__;
    // addi        $23, $zero, 0xCE0
    r23 = RSP_ADD32(0, 0XCE0);
    goto do_indirect_jump;
    // addi        $23, $zero, 0xCE0
    r23 = RSP_ADD32(0, 0XCE0);
    // andi        $2, $2, 0xFE
    r2 = r2 & 0XFE;
L_11B0:
    // lh          $2, 0x76($2)
    r2 = RSP_MEM_H_LOAD(0X76, r2);
    // jr          $2
    jump_target = r2;
    debug_file = __FILE__; debug_line = __LINE__;
    // lbu         $1, -0x1($27)
    r1 = RSP_MEM_BU(-0X1, r27);
    goto do_indirect_jump;
    // lbu         $1, -0x1($27)
    r1 = RSP_MEM_BU(-0X1, r27);
    // lh          $24, 0xBA($zero)
    r24 = RSP_MEM_H_LOAD(0XBA, 0);
L_11C0:
    // lh          $ra, 0x396($zero)
    r31 = RSP_MEM_H_LOAD(0X396, 0);
    // lbu         $1, -0x3($27)
    r1 = RSP_MEM_BU(-0X3, r27);
    // lbu         $2, -0x2($27)
    r2 = RSP_MEM_BU(-0X2, r27);
    // lbu         $3, -0x1($27)
    r3 = RSP_MEM_BU(-0X1, r27);
L_11D0:
    // lhu         $1, 0x31E($1)
    r1 = RSP_MEM_HU_LOAD(0X31E, r1);
    // lhu         $2, 0x31E($2)
    r2 = RSP_MEM_HU_LOAD(0X31E, r2);
    // lhu         $3, 0x31E($3)
    r3 = RSP_MEM_HU_LOAD(0X31E, r3);
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // addi        $4, $1, 0x0
    r4 = RSP_ADD32(r1, 0X0);
    goto do_indirect_jump;
    // addi        $4, $1, 0x0
    r4 = RSP_ADD32(r1, 0X0);
    // jal         0x11C0
    r31 = 0x11EC;
    // addi        $24, $ra, 0x0
    r24 = RSP_ADD32(r31, 0X0);
    goto L_11C0;
    // addi        $24, $ra, 0x0
    r24 = RSP_ADD32(r31, 0X0);
L_11EC:
    // lbu         $1, -0x7($27)
    r1 = RSP_MEM_BU(-0X7, r27);
L_11F0:
    // lbu         $2, -0x6($27)
    r2 = RSP_MEM_BU(-0X6, r27);
    // lbu         $3, -0x5($27)
    r3 = RSP_MEM_BU(-0X5, r27);
    // lh          $ra, 0x396($zero)
    r31 = RSP_MEM_H_LOAD(0X396, 0);
    // j           L_11D0
    // lh          $24, 0xBA($zero)
    r24 = RSP_MEM_H_LOAD(0XBA, 0);
    goto L_11D0;
    // lh          $24, 0xBA($zero)
    r24 = RSP_MEM_H_LOAD(0XBA, 0);
    // sbv         $v31[6], 0x1C($29)
    rsp.SBV<6>(rsp.vpu.r[31], r29, 0X1C);
    // lw          $19, 0x24($29)
    r19 = RSP_MEM_W_LOAD(0X24, r29);
    // lw          $3, 0xFE0($zero)
    r3 = RSP_MEM_W_LOAD(0XFE0, 0);
L_1210:
    // addi        $20, $zero, 0x3E0
    r20 = RSP_ADD32(0, 0X3E0);
    // sub         $3, $3, $19
    r3 = RSP_SUB32(r3, r19);
    // bgez        $3, L_109C
    if (RSP_SIGNED(r3) >= 0) {
        // addi        $19, $19, -0x40
        r19 = RSP_ADD32(r19, -0X40);
        goto L_109C;
    }
    // addi        $19, $19, -0x40
    r19 = RSP_ADD32(r19, -0X40);
    // jal         0x1FD4
    r31 = 0x1228;
    // addi        $18, $zero, 0x3F
    r18 = RSP_ADD32(0, 0X3F);
    goto L_1FD4;
    // addi        $18, $zero, 0x3F
    r18 = RSP_ADD32(0, 0X3F);
L_1228:
    // jal         0x1FC4
    r31 = 0x1230;
    // addi        $3, $zero, 0x460
    r3 = RSP_ADD32(0, 0X460);
    goto L_1FC4;
    // addi        $3, $zero, 0x460
    r3 = RSP_ADD32(0, 0X460);
L_1230:
    // j           L_15BC
    // sw          $19, 0x24($29)
    RSP_MEM_W_STORE(0X24, r29, r19);
    goto L_15BC;
    // sw          $19, 0x24($29)
    RSP_MEM_W_STORE(0X24, r29, r19);
    // lbu         $1, -0x5($27)
    r1 = RSP_MEM_BU(-0X5, r27);
    // lhu         $2, -0x7($27)
    r2 = RSP_MEM_HU_LOAD(-0X7, r27);
    // lh          $5, 0x30E($1)
    r5 = RSP_MEM_H_LOAD(0X30E, r1);
    // add         $5, $5, $2
    r5 = RSP_ADD32(r5, r2);
    // j           L_109C
    // sw          $24, 0x0($5)
    RSP_MEM_W_STORE(0X0, r5, r24);
    goto L_109C;
    // sw          $24, 0x0($5)
    RSP_MEM_W_STORE(0X0, r5, r24);
L_1250:
    // sw          $25, 0x10($29)
    RSP_MEM_W_STORE(0X10, r29, r25);
    // sw          $24, 0x14($29)
    RSP_MEM_W_STORE(0X14, r29, r24);
    // lh          $2, 0x6($29)
    r2 = RSP_MEM_H_LOAD(0X6, r29);
    // andi        $2, $2, 0xFFFD
    r2 = r2 & 0XFFFD;
    // andi        $3, $25, 0x1
    r3 = r25 & 0X1;
    // sll         $3, $3, 1
    r3 = S32(r3) << 1;
    // or          $2, $2, $3
    r2 = r2 | r3;
    // j           L_109C
    // sh          $2, 0x6($29)
    RSP_MEM_H_STORE(0X6, r29, r2);
    goto L_109C;
L_1270:
    // sh          $2, 0x6($29)
    RSP_MEM_H_STORE(0X6, r29, r2);
    // j           L_1280
    // addi        $7, $29, 0x8
    r7 = RSP_ADD32(r29, 0X8);
    goto L_1280;
    // addi        $7, $29, 0x8
    r7 = RSP_ADD32(r29, 0X8);
    // addi        $7, $29, 0xC
    r7 = RSP_ADD32(r29, 0XC);
L_1280:
    // lw          $3, 0x0($7)
    r3 = RSP_MEM_W_LOAD(0X0, r7);
    // lbu         $5, -0x5($27)
    r5 = RSP_MEM_BU(-0X5, r27);
    // lbu         $6, -0x6($27)
    r6 = RSP_MEM_BU(-0X6, r27);
    // addi        $2, $zero, 0x1
    r2 = RSP_ADD32(0, 0X1);
L_1290:
    // sllv        $2, $2, $5
    r2 = S32(r2) << (r5 & 31);
    // addi        $2, $2, -0x1
    r2 = RSP_ADD32(r2, -0X1);
    // sllv        $2, $2, $6
    r2 = S32(r2) << (r6 & 31);
    // nor         $2, $2, $zero
    r2 = ~(r2 | 0);
    // and         $2, $2, $3
    r2 = r2 & r3;
    // or          $3, $2, $24
    r3 = r2 | r24;
    // sw          $3, 0x0($7)
    RSP_MEM_W_STORE(0X0, r7, r3);
    // lw          $25, 0x8($29)
    r25 = RSP_MEM_W_LOAD(0X8, r29);
L_12B0:
    // j           L_1374
    // lw          $24, 0xC($29)
    r24 = RSP_MEM_W_LOAD(0XC, r29);
    goto L_1374;
    // lw          $24, 0xC($29)
    r24 = RSP_MEM_W_LOAD(0XC, r29);
    // lhu         $25, 0x31E($25)
    r25 = RSP_MEM_HU_LOAD(0X31E, r25);
    // lhu         $24, 0x31E($24)
    r24 = RSP_MEM_HU_LOAD(0X31E, r24);
    // ori         $2, $zero, 0x7070
    r2 = 0 | 0X7070;
L_12C4:
    // lh          $3, 0x24($25)
    r3 = RSP_MEM_H_LOAD(0X24, r25);
    // and         $2, $2, $3
    r2 = r2 & r3;
    // bne         $25, $24, L_12C4
    if (r25 != r24) {
        // addi        $25, $25, 0x28
        r25 = RSP_ADD32(r25, 0X28);
        goto L_12C4;
    }
L_12D0:
    // addi        $25, $25, 0x28
    r25 = RSP_ADD32(r25, 0X28);
    // beq         $2, $zero, L_109C
    if (r2 == 0) {
        // lb          $2, 0x38($29)
        r2 = RSP_MEM_B(0X38, r29);
        goto L_109C;
    }
    // lb          $2, 0x38($29)
    r2 = RSP_MEM_B(0X38, r29);
    // addi        $2, $2, -0x4
    r2 = RSP_ADD32(r2, -0X4);
    // bltz        $2, L_10BC
    if (RSP_SIGNED(r2) < 0) {
        // addi        $3, $2, 0x398
        r3 = RSP_ADD32(r2, 0X398);
        goto L_10BC;
    }
    // addi        $3, $2, 0x398
    r3 = RSP_ADD32(r2, 0X398);
    // lw          $26, 0x0($3)
    r26 = RSP_MEM_W_LOAD(0X0, r3);
    // sb          $2, 0x38($29)
    RSP_MEM_B(0X38, r29) = r2;
    // j           L_109C
    // addi        $28, $27, 0x0
    r28 = RSP_ADD32(r27, 0X0);
    goto L_109C;
    // addi        $28, $27, 0x0
    r28 = RSP_ADD32(r27, 0X0);
    // lhu         $25, 0x31E($25)
    r25 = RSP_MEM_HU_LOAD(0X31E, r25);
    // lw          $25, 0x1C($25)
    r25 = RSP_MEM_W_LOAD(0X1C, r25);
    // sub         $25, $25, $24
    r25 = RSP_SUB32(r25, r24);
    // bgtz        $25, L_109C
    if (RSP_SIGNED(r25) > 0) {
        // lw          $24, -0x4($29)
        r24 = RSP_MEM_W_LOAD(-0X4, r29);
        goto L_109C;
    }
    // lw          $24, -0x4($29)
    r24 = RSP_MEM_W_LOAD(-0X4, r29);
    // j           L_1708
    // lw          $2, 0x4($29)
    r2 = RSP_MEM_W_LOAD(0X4, r29);
    goto L_1708;
    // lw          $2, 0x4($29)
    r2 = RSP_MEM_W_LOAD(0X4, r29);
    // or          $2, $2, $24
    r2 = r2 | r24;
    // j           L_109C
    // sw          $2, 0x4($29)
    RSP_MEM_W_STORE(0X4, r29, r2);
    goto L_109C;
    // sw          $2, 0x4($29)
    RSP_MEM_W_STORE(0X4, r29, r2);
    // lw          $2, 0x4($29)
    r2 = RSP_MEM_W_LOAD(0X4, r29);
    // nor         $3, $24, $zero
    r3 = ~(r24 | 0);
    // and         $2, $2, $3
    r2 = r2 & r3;
    // j           L_109C
    // sw          $2, 0x4($29)
    RSP_MEM_W_STORE(0X4, r29, r2);
    goto L_109C;
    // sw          $2, 0x4($29)
    RSP_MEM_W_STORE(0X4, r29, r2);
    // lbu         $2, -0x7($27)
    r2 = RSP_MEM_BU(-0X7, r27);
    // lhu         $3, 0x31E($25)
    r3 = RSP_MEM_HU_LOAD(0X31E, r25);
    // add         $3, $3, $2
    r3 = RSP_ADD32(r3, r2);
    // j           L_109C
    // sw          $24, 0x0($3)
    RSP_MEM_W_STORE(0X0, r3, r24);
    goto L_109C;
    // sw          $24, 0x0($3)
    RSP_MEM_W_STORE(0X0, r3, r24);
    // j           L_10AC
    // sw          $24, -0x4($29)
    RSP_MEM_W_STORE(-0X4, r29, r24);
    goto L_10AC;
    // sw          $24, -0x4($29)
    RSP_MEM_W_STORE(-0X4, r29, r24);
    // j           L_1374
    // lw          $25, -0x4($29)
    r25 = RSP_MEM_W_LOAD(-0X4, r29);
    goto L_1374;
    // lw          $25, -0x4($29)
    r25 = RSP_MEM_W_LOAD(-0X4, r29);
    // sra         $2, $25, 24
    r2 = S32(RSP_SIGNED(r25) >> 24);
    // addi        $2, $2, 0x3
    r2 = RSP_ADD32(r2, 0X3);
    // bltz        $2, L_1374
    if (RSP_SIGNED(r2) < 0) {
        // addi        $2, $2, 0x18
        r2 = RSP_ADD32(r2, 0X18);
        goto L_1374;
    }
    // addi        $2, $2, 0x18
    r2 = RSP_ADD32(r2, 0X18);
    // jal         0x110C
    r31 = 0x1370;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
    goto L_110C;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
L_1370:
    // add         $24, $19, $zero
    r24 = RSP_ADD32(r19, 0);
L_1374:
    // sw          $25, 0x0($23)
    RSP_MEM_W_STORE(0X0, r23, r25);
    // sw          $24, 0x4($23)
    RSP_MEM_W_STORE(0X4, r23, r24);
    // jal         0x1128
    r31 = 0x1384;
    // addi        $23, $23, 0x8
    r23 = RSP_ADD32(r23, 0X8);
    goto L_1128;
L_1380:
    // addi        $23, $23, 0x8
    r23 = RSP_ADD32(r23, 0X8);
L_1384:
    // bgtz        $2, L_109C
    if (RSP_SIGNED(r2) > 0) {
        // nop
    
        goto L_109C;
    }
    // nop

    // j           L_10AC
    // andi        $2, $2, 0x1FE
    r2 = r2 & 0X1FE;
    goto L_10AC;
L_1390:
    // andi        $2, $2, 0x1FE
    r2 = r2 & 0X1FE;
    // lh          $2, 0xC0($2)
    r2 = RSP_MEM_H_LOAD(0XC0, r2);
    // jal         0x1FC4
    r31 = 0x13A0;
    // lbu         $1, -0x7($27)
    r1 = RSP_MEM_BU(-0X7, r27);
    goto L_1FC4;
    // lbu         $1, -0x7($27)
    r1 = RSP_MEM_BU(-0X7, r27);
L_13A0:
    // jr          $2
    jump_target = r2;
    debug_file = __FILE__; debug_line = __LINE__;
    // andi        $6, $1, 0xF
    r6 = r1 & 0XF;
    goto do_indirect_jump;
    // andi        $6, $1, 0xF
    r6 = r1 & 0XF;
    // lh          $8, 0xBA($zero)
    r8 = RSP_MEM_H_LOAD(0XBA, 0);
    // sh          $8, 0x106($zero)
    RSP_MEM_H_STORE(0X106, 0, r8);
    // lbu         $9, -0x6($27)
    r9 = RSP_MEM_BU(-0X6, r27);
    // ldv         $v2[0], 0x0($22)
    rsp.LDV<0>(rsp.vpu.r[2], r22, 0X0);
    // ldv         $v2[8], 0x10($22)
    rsp.LDV<8>(rsp.vpu.r[2], r22, 0X2);
    // srl         $9, $9, 2
    r9 = S32(U32(r9) >> 2);
    // jal         0x1660
    r31 = 0x13C8;
    // lhu         $7, 0x31E($1)
    r7 = RSP_MEM_HU_LOAD(0X31E, r1);
    goto L_1660;
    // lhu         $7, 0x31E($1)
    r7 = RSP_MEM_HU_LOAD(0X31E, r1);
L_13C8:
    // llv         $v17[0], 0x14($29)
    rsp.LLV<0>(rsp.vpu.r[17], r29, 0X5);
    // jal         0x1688
    r31 = 0x13D4;
    // llv         $v17[8], 0x14($29)
    rsp.LLV<8>(rsp.vpu.r[17], r29, 0X5);
    goto L_1688;
    // llv         $v17[8], 0x14($29)
    rsp.LLV<8>(rsp.vpu.r[17], r29, 0X5);
L_13D4:
    // vmudn       $v28, $v12, $v2[0h]
    rsp.VMUDN<4>(rsp.vpu.r[28], rsp.vpu.r[12], rsp.vpu.r[2]);
    // llv         $v18[0], 0x8($22)
    rsp.LLV<0>(rsp.vpu.r[18], r22, 0X2);
    // vmadh       $v28, $v8, $v2[0h]
    rsp.VMADH<4>(rsp.vpu.r[28], rsp.vpu.r[8], rsp.vpu.r[2]);
    // lw          $15, 0xC($22)
    r15 = RSP_MEM_W_LOAD(0XC, r22);
    // vmadn       $v28, $v13, $v2[1h]
    rsp.VMADN<5>(rsp.vpu.r[28], rsp.vpu.r[13], rsp.vpu.r[2]);
    // lw          $16, 0x1C($22)
    r16 = RSP_MEM_W_LOAD(0X1C, r22);
    // vmadh       $v28, $v9, $v2[1h]
    rsp.VMADH<5>(rsp.vpu.r[28], rsp.vpu.r[9], rsp.vpu.r[2]);
    // andi        $1, $3, 0x2
    r1 = r3 & 0X2;
    // vmadn       $v28, $v14, $v2[2h]
    rsp.VMADN<6>(rsp.vpu.r[28], rsp.vpu.r[14], rsp.vpu.r[2]);
    // vmadh       $v28, $v10, $v2[2h]
    rsp.VMADH<6>(rsp.vpu.r[28], rsp.vpu.r[10], rsp.vpu.r[2]);
    // vmadn       $v28, $v15, $v31[1]
    rsp.VMADN<9>(rsp.vpu.r[28], rsp.vpu.r[15], rsp.vpu.r[31]);
    // llv         $v18[8], 0x18($22)
    rsp.LLV<8>(rsp.vpu.r[18], r22, 0X6);
    // vmadh       $v29, $v11, $v31[1]
    rsp.VMADH<9>(rsp.vpu.r[29], rsp.vpu.r[11], rsp.vpu.r[31]);
    // bne         $1, $zero, L_172C
    if (r1 != 0) {
        // addi        $22, $22, 0x20
        r22 = RSP_ADD32(r22, 0X20);
        goto L_172C;
    }
    // addi        $22, $22, 0x20
    r22 = RSP_ADD32(r22, 0X20);
    // vmudm       $v18, $v18, $v17
    rsp.VMUDM<0>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[17]);
    // lsv         $v21[0], 0x76($zero)
    rsp.LSV<0>(rsp.vpu.r[21], 0, 0X3B);
    // vmudn       $v20, $v28, $v21[0]
    rsp.VMUDN<8>(rsp.vpu.r[20], rsp.vpu.r[28], rsp.vpu.r[21]);
    // vmadh       $v21, $v29, $v21[0]
    rsp.VMADH<8>(rsp.vpu.r[21], rsp.vpu.r[29], rsp.vpu.r[21]);
    // vch         $v3, $v29, $v29[3h]
    rsp.VCH<7>(rsp.vpu.r[3], rsp.vpu.r[29], rsp.vpu.r[29]);
    // vcl         $v3, $v28, $v28[3h]
    rsp.VCL<7>(rsp.vpu.r[3], rsp.vpu.r[28], rsp.vpu.r[28]);
    // cfc2        $t5, $1
    rsp.CFC2(r13, 1);
    // vch         $v3, $v29, $v21[3h]
    rsp.VCH<7>(rsp.vpu.r[3], rsp.vpu.r[29], rsp.vpu.r[21]);
    // vcl         $v3, $v28, $v20[3h]
    rsp.VCL<7>(rsp.vpu.r[3], rsp.vpu.r[28], rsp.vpu.r[20]);
    // andi        $8, $13, 0x707
    r8 = r13 & 0X707;
    // andi        $13, $13, 0x7070
    r13 = r13 & 0X7070;
    // sll         $8, $8, 4
    r8 = S32(r8) << 4;
    // sll         $13, $13, 16
    r13 = S32(r13) << 16;
    // or          $13, $13, $8
    r13 = r13 | r8;
    // cfc2        $t6, $1
    rsp.CFC2(r14, 1);
    // andi        $8, $14, 0x707
    r8 = r14 & 0X707;
    // vadd        $v21, $v29, $v31[0]
    rsp.VADD<8>(rsp.vpu.r[21], rsp.vpu.r[29], rsp.vpu.r[31]);
    // andi        $14, $14, 0x7070
    r14 = r14 & 0X7070;
    // vadd        $v20, $v28, $v31[0]
    rsp.VADD<8>(rsp.vpu.r[20], rsp.vpu.r[28], rsp.vpu.r[31]);
    // sll         $14, $14, 12
    r14 = S32(r14) << 12;
L_1460:
    // vmudl       $v28, $v28, $v19[0]
    rsp.VMUDL<8>(rsp.vpu.r[28], rsp.vpu.r[28], rsp.vpu.r[19]);
    // or          $8, $8, $14
    r8 = r8 | r14;
    // vmadm       $v29, $v29, $v19[0]
    rsp.VMADM<8>(rsp.vpu.r[29], rsp.vpu.r[29], rsp.vpu.r[19]);
    // or          $8, $8, $13
    r8 = r8 | r13;
L_1470:
    // vmadn       $v28, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[28], rsp.vpu.r[31], rsp.vpu.r[31]);
    // jal         0x1000
    r31 = 0x147C;
    // sh          $8, 0x24($7)
    RSP_MEM_H_STORE(0X24, r7, r8);
    goto L_1000;
    // sh          $8, 0x24($7)
    RSP_MEM_H_STORE(0X24, r7, r8);
L_147C:
    // vge         $v6, $v27, $v31[0]
    rsp.VGE<8>(rsp.vpu.r[6], rsp.vpu.r[27], rsp.vpu.r[31]);
L_1480:
    // sdv         $v21[0], 0x0($7)
    rsp.SDV<0>(rsp.vpu.r[21], r7, 0X0);
    // vmrg        $v6, $v27, $v30[0]
    rsp.VMRG<8>(rsp.vpu.r[6], rsp.vpu.r[27], rsp.vpu.r[30]);
    // sdv         $v20[0], 0x8($7)
    rsp.SDV<0>(rsp.vpu.r[20], r7, 0X1);
    // vmudl       $v5, $v20, $v26[3h]
    rsp.VMUDL<7>(rsp.vpu.r[5], rsp.vpu.r[20], rsp.vpu.r[26]);
L_1490:
    // vmadm       $v5, $v21, $v26[3h]
    rsp.VMADM<7>(rsp.vpu.r[5], rsp.vpu.r[21], rsp.vpu.r[26]);
    // vmadn       $v5, $v20, $v6[3h]
    rsp.VMADN<7>(rsp.vpu.r[5], rsp.vpu.r[20], rsp.vpu.r[6]);
    // vmadh       $v4, $v21, $v6[3h]
    rsp.VMADH<7>(rsp.vpu.r[4], rsp.vpu.r[21], rsp.vpu.r[6]);
    // addi        $9, $9, -0x2
    r9 = RSP_ADD32(r9, -0X2);
L_14A0:
    // vmudl       $v5, $v5, $v19[0]
    rsp.VMUDL<8>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[19]);
    // vmadm       $v4, $v4, $v19[0]
    rsp.VMADM<8>(rsp.vpu.r[4], rsp.vpu.r[4], rsp.vpu.r[19]);
    // vmadn       $v5, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[5], rsp.vpu.r[31], rsp.vpu.r[31]);
    // andi        $12, $3, 0x1
    r12 = r3 & 0X1;
    // ldv         $v2[0], 0x0($22)
    rsp.LDV<0>(rsp.vpu.r[2], r22, 0X0);
    // vmudh       $v7, $v1, $v31[1]
    rsp.VMUDH<9>(rsp.vpu.r[7], rsp.vpu.r[1], rsp.vpu.r[31]);
    // ldv         $v2[8], 0x10($22)
    rsp.LDV<8>(rsp.vpu.r[2], r22, 0X2);
    // vmadn       $v7, $v5, $v0
    rsp.VMADN<0>(rsp.vpu.r[7], rsp.vpu.r[5], rsp.vpu.r[0]);
    // ldv         $v29[0], 0x28($zero)
    rsp.LDV<0>(rsp.vpu.r[29], 0, 0X5);
    // vmadh       $v6, $v4, $v0
    rsp.VMADH<0>(rsp.vpu.r[6], rsp.vpu.r[4], rsp.vpu.r[0]);
L_14C8:
    // ldv         $v29[8], 0x28($zero)
    rsp.LDV<8>(rsp.vpu.r[29], 0, 0X5);
    // vmadn       $v7, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[7], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vge         $v6, $v6, $v29[1q]
    rsp.VGE<3>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[29]);
    // sw          $15, 0x10($7)
    RSP_MEM_W_STORE(0X10, r7, r15);
    // beq         $12, $zero, L_1508
    if (r12 == 0) {
        // vlt         $v6, $v6, $v29[0q]
        rsp.VLT<2>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[29]);
        goto L_1508;
    }
    // vlt         $v6, $v6, $v29[0q]
    rsp.VLT<2>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[29]);
    // lqv         $v3[0], 0x390($zero)
    rsp.LQV<0>(rsp.vpu.r[3], 0, 0X39);
    // vmudn       $v5, $v5, $v3[0]
    rsp.VMUDN<8>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[3]);
    // vmadh       $v4, $v4, $v3[0]
    rsp.VMADH<8>(rsp.vpu.r[4], rsp.vpu.r[4], rsp.vpu.r[3]);
    // vadd        $v4, $v4, $v3[1]
    rsp.VADD<9>(rsp.vpu.r[4], rsp.vpu.r[4], rsp.vpu.r[3]);
L_14F0:
    // vge         $v4, $v4, $v31[0]
    rsp.VGE<8>(rsp.vpu.r[4], rsp.vpu.r[4], rsp.vpu.r[31]);
    // vlt         $v4, $v4, $v3[2]
    rsp.VLT<10>(rsp.vpu.r[4], rsp.vpu.r[4], rsp.vpu.r[3]);
    // sbv         $v4[5], 0x13($7)
    rsp.SBV<5>(rsp.vpu.r[4], r7, 0X13);
    // sw          $16, 0x18($7)
    RSP_MEM_W_STORE(0X18, r7, r16);
    // sbv         $v4[13], 0x1B($7)
    rsp.SBV<13>(rsp.vpu.r[4], r7, 0X1B);
    // lw          $16, 0x18($7)
    r16 = RSP_MEM_W_LOAD(0X18, r7);
L_1508:
    // slv         $v18[0], 0x14($7)
    rsp.SLV<0>(rsp.vpu.r[18], r7, 0X5);
    // sdv         $v6[0], 0x18($7)
    rsp.SDV<0>(rsp.vpu.r[6], r7, 0X3);
    // ssv         $v7[4], 0x1E($7)
    rsp.SSV<4>(rsp.vpu.r[7], r7, 0XF);
    // ssv         $v27[6], 0x20($7)
    rsp.SSV<6>(rsp.vpu.r[27], r7, 0X10);
L_1518:
    // bltz        $9, L_154C
    if (RSP_SIGNED(r9) < 0) {
        // ssv         $v26[6], 0x22($7)
        rsp.SSV<6>(rsp.vpu.r[26], r7, 0X11);
        goto L_154C;
    }
    // ssv         $v26[6], 0x22($7)
    rsp.SSV<6>(rsp.vpu.r[26], r7, 0X11);
    // sdv         $v21[8], 0x28($7)
    rsp.SDV<8>(rsp.vpu.r[21], r7, 0X5);
    // sdv         $v20[8], 0x30($7)
    rsp.SDV<8>(rsp.vpu.r[20], r7, 0X6);
    // slv         $v18[8], 0x3C($7)
    rsp.SLV<8>(rsp.vpu.r[18], r7, 0XF);
    // sw          $16, 0x38($7)
    RSP_MEM_W_STORE(0X38, r7, r16);
    // sdv         $v6[8], 0x40($7)
    rsp.SDV<8>(rsp.vpu.r[6], r7, 0X8);
    // ssv         $v7[12], 0x46($7)
    rsp.SSV<12>(rsp.vpu.r[7], r7, 0X23);
    // ssv         $v27[14], 0x48($7)
    rsp.SSV<14>(rsp.vpu.r[27], r7, 0X24);
    // ssv         $v26[14], 0x4A($7)
    rsp.SSV<14>(rsp.vpu.r[26], r7, 0X25);
L_1540:
    // sw          $8, 0x4C($7)
    RSP_MEM_W_STORE(0X4C, r7, r8);
    // addi        $7, $7, 0x50
    r7 = RSP_ADD32(r7, 0X50);
    // bgtz        $9, L_13D4
    if (RSP_SIGNED(r9) > 0) {
        // lh          $8, 0x106($zero)
        r8 = RSP_MEM_H_LOAD(0X106, 0);
        goto L_13D4;
    }
L_154C:
    // lh          $8, 0x106($zero)
    r8 = RSP_MEM_H_LOAD(0X106, 0);
    // jr          $8
    jump_target = r8;
    debug_file = __FILE__; debug_line = __LINE__;
    // andi        $8, $1, 0x1
    r8 = r1 & 0X1;
    goto do_indirect_jump;
    // andi        $8, $1, 0x1
    r8 = r1 & 0X1;
    // sbv         $v31[6], 0x1C($29)
    rsp.SBV<6>(rsp.vpu.r[31], r29, 0X1C);
    // bne         $8, $zero, L_15CC
    if (r8 != 0) {
        // andi        $7, $1, 0x2
        r7 = r1 & 0X2;
        goto L_15CC;
    }
    // andi        $7, $1, 0x2
    r7 = r1 & 0X2;
    // addi        $20, $zero, 0x3E0
    r20 = RSP_ADD32(0, 0X3E0);
L_1568:
    // andi        $8, $1, 0x4
    r8 = r1 & 0X4;
    // beq         $8, $zero, L_1598
    if (r8 == 0) {
        // lqv         $v26[0], 0x30($22)
        rsp.LQV<0>(rsp.vpu.r[26], r22, 0X3);
        goto L_1598;
    }
    // lqv         $v26[0], 0x30($22)
    rsp.LQV<0>(rsp.vpu.r[26], r22, 0X3);
    // lw          $19, 0x24($29)
    r19 = RSP_MEM_W_LOAD(0X24, r29);
    // lw          $8, 0x4C($29)
    r8 = RSP_MEM_W_LOAD(0X4C, r29);
    // addi        $17, $zero, 0x1
    r17 = RSP_ADD32(0, 0X1);
    // addi        $1, $19, 0x40
    r1 = RSP_ADD32(r19, 0X40);
    // beq         $19, $8, L_1598
    if (r19 == r8) {
        // addi        $12, $zero, 0x3F
        r12 = RSP_ADD32(0, 0X3F);
        goto L_1598;
    }
    // addi        $12, $zero, 0x3F
    r12 = RSP_ADD32(0, 0X3F);
    // jal         0x1FD8
    r31 = 0x1594;
    // sw          $1, 0x24($29)
    RSP_MEM_W_STORE(0X24, r29, r1);
    goto L_1FD8;
L_1590:
    // sw          $1, 0x24($29)
    RSP_MEM_W_STORE(0X24, r29, r1);
L_1594:
    // jal         0x1FC4
    r31 = 0x159C;
    // lqv         $v28[0], 0x10($22)
    rsp.LQV<0>(rsp.vpu.r[28], r22, 0X1);
    goto L_1FC4;
L_1598:
    // lqv         $v28[0], 0x10($22)
    rsp.LQV<0>(rsp.vpu.r[28], r22, 0X1);
L_159C:
    // beq         $7, $zero, L_15D8
    if (r7 == 0) {
        // lqv         $v27[0], 0x20($22)
        rsp.LQV<0>(rsp.vpu.r[27], r22, 0X2);
        goto L_15D8;
    }
    // lqv         $v27[0], 0x20($22)
    rsp.LQV<0>(rsp.vpu.r[27], r22, 0X2);
    // sqv         $v26[0], 0x30($20)
    rsp.SQV<0>(rsp.vpu.r[26], r20, 0X3);
    // lqv         $v29[0], 0x0($22)
    rsp.LQV<0>(rsp.vpu.r[29], r22, 0X0);
    // sqv         $v28[0], 0x10($20)
    rsp.SQV<0>(rsp.vpu.r[28], r20, 0X1);
L_15B0:
    // addi        $3, $zero, 0x460
    r3 = RSP_ADD32(0, 0X460);
    // sqv         $v27[0], 0x20($20)
    rsp.SQV<0>(rsp.vpu.r[27], r20, 0X2);
L_15B8:
    // sqv         $v29[0], 0x0($20)
    rsp.SQV<0>(rsp.vpu.r[29], r20, 0X0);
L_15BC:
    // addi        $1, $zero, 0x3E0
    r1 = RSP_ADD32(0, 0X3E0);
    // addi        $2, $zero, 0x420
    r2 = RSP_ADD32(0, 0X420);
    // j           L_15FC
    // lh          $ra, 0xBA($zero)
    r31 = RSP_MEM_H_LOAD(0XBA, 0);
    goto L_15FC;
    // lh          $ra, 0xBA($zero)
    r31 = RSP_MEM_H_LOAD(0XBA, 0);
L_15CC:
    // lqv         $v26[0], 0x30($22)
    rsp.LQV<0>(rsp.vpu.r[26], r22, 0X3);
    // j           L_1598
    // addi        $20, $zero, 0x420
    r20 = RSP_ADD32(0, 0X420);
    goto L_1598;
    // addi        $20, $zero, 0x420
    r20 = RSP_ADD32(0, 0X420);
L_15D8:
    // addiu       $3, $zero, 0xDE0
    r3 = RSP_ADD32(0, 0XDE0);
    // addu        $1, $zero, $22
    r1 = RSP_ADD32(0, r22);
L_15E0:
    // jal         0x15FC
    r31 = 0x15E8;
    // addu        $2, $zero, $20
    r2 = RSP_ADD32(0, r20);
    goto L_15FC;
    // addu        $2, $zero, $20
    r2 = RSP_ADD32(0, r20);
L_15E8:
    // sqv         $v6[0], 0x30($20)
    rsp.SQV<0>(rsp.vpu.r[6], r20, 0X3);
    // sqv         $v5[0], 0x10($20)
    rsp.SQV<0>(rsp.vpu.r[5], r20, 0X1);
    // lqv         $v27[0], 0x0($3)
    rsp.LQV<0>(rsp.vpu.r[27], r3, 0X0);
    // j           L_15B0
    // lqv         $v29[0], 0x7E0($3)
    rsp.LQV<0>(rsp.vpu.r[29], r3, -0X2);
    goto L_15B0;
    // lqv         $v29[0], 0x7E0($3)
    rsp.LQV<0>(rsp.vpu.r[29], r3, -0X2);
L_15FC:
    // addi        $19, $3, 0x10
    r19 = RSP_ADD32(r3, 0X10);
L_1600:
    // vmudh       $v5, $v31, $v31[0]
    rsp.VMUDH<8>(rsp.vpu.r[5], rsp.vpu.r[31], rsp.vpu.r[31]);
    // addi        $18, $1, 0x8
    r18 = RSP_ADD32(r1, 0X8);
L_1608:
    // ldv         $v3[0], 0x0($2)
    rsp.LDV<0>(rsp.vpu.r[3], r2, 0X0);
    // ldv         $v4[0], 0x20($2)
    rsp.LDV<0>(rsp.vpu.r[4], r2, 0X4);
    // lqv         $v1[0], 0x0($1)
    rsp.LQV<0>(rsp.vpu.r[1], r1, 0X0);
    // lqv         $v2[0], 0x20($1)
    rsp.LQV<0>(rsp.vpu.r[2], r1, 0X2);
    // ldv         $v3[8], 0x0($2)
    rsp.LDV<8>(rsp.vpu.r[3], r2, 0X0);
    // ldv         $v4[8], 0x20($2)
    rsp.LDV<8>(rsp.vpu.r[4], r2, 0X4);
    // vmadl       $v6, $v4, $v2[0h]
    rsp.VMADL<4>(rsp.vpu.r[6], rsp.vpu.r[4], rsp.vpu.r[2]);
    // addi        $1, $1, 0x2
    r1 = RSP_ADD32(r1, 0X2);
    // vmadm       $v6, $v3, $v2[0h]
    rsp.VMADM<4>(rsp.vpu.r[6], rsp.vpu.r[3], rsp.vpu.r[2]);
    // addi        $2, $2, 0x8
    r2 = RSP_ADD32(r2, 0X8);
L_1630:
    // vmadn       $v6, $v4, $v1[0h]
    rsp.VMADN<4>(rsp.vpu.r[6], rsp.vpu.r[4], rsp.vpu.r[1]);
    // vmadh       $v5, $v3, $v1[0h]
    rsp.VMADH<4>(rsp.vpu.r[5], rsp.vpu.r[3], rsp.vpu.r[1]);
    // bne         $1, $18, L_1608
    if (r1 != r18) {
        // vmadn       $v6, $v31, $v31[0]
        rsp.VMADN<8>(rsp.vpu.r[6], rsp.vpu.r[31], rsp.vpu.r[31]);
        goto L_1608;
    }
    // vmadn       $v6, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[6], rsp.vpu.r[31], rsp.vpu.r[31]);
    // addi        $2, $2, -0x20
    r2 = RSP_ADD32(r2, -0X20);
    // addi        $1, $1, 0x8
    r1 = RSP_ADD32(r1, 0X8);
    // sqv         $v5[0], 0x0($3)
    rsp.SQV<0>(rsp.vpu.r[5], r3, 0X0);
    // sqv         $v6[0], 0x20($3)
    rsp.SQV<0>(rsp.vpu.r[6], r3, 0X2);
    // bne         $3, $19, L_1600
    if (r3 != r19) {
        // addi        $3, $3, 0x10
        r3 = RSP_ADD32(r3, 0X10);
        goto L_1600;
    }
    // addi        $3, $3, 0x10
    r3 = RSP_ADD32(r3, 0X10);
L_1658:
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // nop

    goto do_indirect_jump;
    // nop

L_1660:
    // addi        $8, $zero, 0x380
    r8 = RSP_ADD32(0, 0X380);
    // lqv         $v3[0], 0x50($zero)
    rsp.LQV<0>(rsp.vpu.r[3], 0, 0X5);
    // lsv         $v19[0], 0x2($29)
    rsp.LSV<0>(rsp.vpu.r[19], r29, 0X1);
    // lh          $3, 0x4($29)
    r3 = RSP_MEM_H_LOAD(0X4, r29);
    // ldv         $v0[0], 0x0($8)
    rsp.LDV<0>(rsp.vpu.r[0], r8, 0X0);
    // ldv         $v1[0], 0x8($8)
    rsp.LDV<0>(rsp.vpu.r[1], r8, 0X1);
    // ldv         $v0[8], 0x0($8)
    rsp.LDV<8>(rsp.vpu.r[0], r8, 0X0);
    // ldv         $v1[8], 0x8($8)
    rsp.LDV<8>(rsp.vpu.r[1], r8, 0X1);
L_1680:
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // vmudh       $v0, $v0, $v3
    rsp.VMUDH<0>(rsp.vpu.r[0], rsp.vpu.r[0], rsp.vpu.r[3]);
    goto do_indirect_jump;
    // vmudh       $v0, $v0, $v3
    rsp.VMUDH<0>(rsp.vpu.r[0], rsp.vpu.r[0], rsp.vpu.r[3]);
L_1688:
    // addi        $8, $zero, 0x460
    r8 = RSP_ADD32(0, 0X460);
    // ldv         $v11[0], 0x18($8)
    rsp.LDV<0>(rsp.vpu.r[11], r8, 0X3);
    // ldv         $v11[8], 0x18($8)
    rsp.LDV<8>(rsp.vpu.r[11], r8, 0X3);
    // ldv         $v15[0], 0x38($8)
    rsp.LDV<0>(rsp.vpu.r[15], r8, 0X7);
    // ldv         $v15[8], 0x38($8)
    rsp.LDV<8>(rsp.vpu.r[15], r8, 0X7);
    // ldv         $v8[0], 0x0($8)
    rsp.LDV<0>(rsp.vpu.r[8], r8, 0X0);
    // ldv         $v9[0], 0x8($8)
    rsp.LDV<0>(rsp.vpu.r[9], r8, 0X1);
    // ldv         $v10[0], 0x10($8)
    rsp.LDV<0>(rsp.vpu.r[10], r8, 0X2);
L_16A8:
    // ldv         $v12[0], 0x20($8)
    rsp.LDV<0>(rsp.vpu.r[12], r8, 0X4);
    // ldv         $v13[0], 0x28($8)
    rsp.LDV<0>(rsp.vpu.r[13], r8, 0X5);
    // ldv         $v14[0], 0x30($8)
    rsp.LDV<0>(rsp.vpu.r[14], r8, 0X6);
    // ldv         $v8[8], 0x0($8)
    rsp.LDV<8>(rsp.vpu.r[8], r8, 0X0);
    // ldv         $v9[8], 0x8($8)
    rsp.LDV<8>(rsp.vpu.r[9], r8, 0X1);
    // ldv         $v10[8], 0x10($8)
    rsp.LDV<8>(rsp.vpu.r[10], r8, 0X2);
    // ldv         $v12[8], 0x20($8)
    rsp.LDV<8>(rsp.vpu.r[12], r8, 0X4);
    // ldv         $v13[8], 0x28($8)
    rsp.LDV<8>(rsp.vpu.r[13], r8, 0X5);
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // ldv         $v14[8], 0x30($8)
    rsp.LDV<8>(rsp.vpu.r[14], r8, 0X6);
    goto do_indirect_jump;
    // ldv         $v14[8], 0x30($8)
    rsp.LDV<8>(rsp.vpu.r[14], r8, 0X6);
L_16D0:
    // lqv         $v0[0], 0x0($22)
    rsp.LQV<0>(rsp.vpu.r[0], r22, 0X0);
    // lh          $5, 0x270($1)
    r5 = RSP_MEM_H_LOAD(0X270, r1);
    // j           L_109C
    // sqv         $v0[0], 0x0($5)
    rsp.SQV<0>(rsp.vpu.r[0], r5, 0X0);
    goto L_109C;
    // sqv         $v0[0], 0x0($5)
    rsp.SQV<0>(rsp.vpu.r[0], r5, 0X0);
    // bgtz        $1, L_1708
    if (RSP_SIGNED(r1) > 0) {
        // lb          $2, 0x38($29)
        r2 = RSP_MEM_B(0X38, r29);
        goto L_1708;
    }
    // lb          $2, 0x38($29)
    r2 = RSP_MEM_B(0X38, r29);
    // addi        $4, $2, -0x44
    r4 = RSP_ADD32(r2, -0X44);
    // bgtz        $4, L_109C
    if (RSP_SIGNED(r4) > 0) {
        // addi        $3, $2, 0x398
        r3 = RSP_ADD32(r2, 0X398);
        goto L_109C;
    }
    // addi        $3, $2, 0x398
    r3 = RSP_ADD32(r2, 0X398);
    // addi        $2, $2, 0x4
    r2 = RSP_ADD32(r2, 0X4);
L_16F8:
    // add         $26, $26, $27
    r26 = RSP_ADD32(r26, r27);
    // sub         $26, $26, $28
    r26 = RSP_SUB32(r26, r28);
    // sw          $26, 0x0($3)
    RSP_MEM_W_STORE(0X0, r3, r26);
    // sb          $2, 0x38($29)
    RSP_MEM_B(0X38, r29) = r2;
L_1708:
    // jal         0x110C
    r31 = 0x1710;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
    goto L_110C;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
L_1710:
    // add         $26, $19, $zero
    r26 = RSP_ADD32(r19, 0);
    // j           L_109C
    // addi        $28, $27, 0x0
    r28 = RSP_ADD32(r27, 0X0);
    goto L_109C;
    // addi        $28, $27, 0x0
    r28 = RSP_ADD32(r27, 0X0);
    // nop

L_1720:
    // ori         $30, $zero, 0x10
    r30 = 0 | 0X10;
    // b           L_10F0
    // lh          $21, 0x100($zero)
    r21 = RSP_MEM_H_LOAD(0X100, 0);
    goto L_10F0;
    // lh          $21, 0x100($zero)
    r21 = RSP_MEM_H_LOAD(0X100, 0);
L_172C:
    // ori         $30, $zero, 0x18
    r30 = 0 | 0X18;
    // b           L_10F0
    // lh          $21, 0xA0($zero)
    r21 = RSP_MEM_H_LOAD(0XA0, 0);
    goto L_10F0;
    // lh          $21, 0xA0($zero)
    r21 = RSP_MEM_H_LOAD(0XA0, 0);
L_1738:
    // ori         $2, $zero, 0x2800
    r2 = 0 | 0X2800;
    // mtc0        $2, SP_STATUS
    set_sp_status(r2);
    // lqv         $v31[0], 0x30($zero)
    rsp.LQV<0>(rsp.vpu.r[31], 0, 0X3);
    // lqv         $v30[0], 0x40($zero)
    rsp.LQV<0>(rsp.vpu.r[30], 0, 0X4);
L_1748:
    // lw          $4, 0xFC4($zero)
    r4 = RSP_MEM_W_LOAD(0XFC4, 0);
    // andi        $4, $4, 0x1
    r4 = r4 & 0X1;
    // bne         $4, $zero, L_182C
    if (r4 != 0) {
        // sw          $zero, 0xFC4($zero)
        RSP_MEM_W_STORE(0XFC4, 0, 0);
        goto L_182C;
    }
    // sw          $zero, 0xFC4($zero)
    RSP_MEM_W_STORE(0XFC4, 0, 0);
    // lw          $23, 0x28($1)
    r23 = RSP_MEM_W_LOAD(0X28, r1);
    // lw          $3, 0x2C($1)
    r3 = RSP_MEM_W_LOAD(0X2C, r1);
    // sw          $23, 0x40($29)
    RSP_MEM_W_STORE(0X40, r29, r23);
    // sw          $3, 0x44($29)
    RSP_MEM_W_STORE(0X44, r29, r3);
    // mfc0        $4, DPC_STATUS
    r4 = get_dpc_status();
    // andi        $4, $4, 0x1
    r4 = r4 & 0X1;
L_1770:
    // bne         $4, $zero, L_179C
    if (r4 != 0) {
        // mfc0        $4, DPC_END
        r4 = get_dpc_end();
        goto L_179C;
    }
    // mfc0        $4, DPC_END
    r4 = get_dpc_end();
    // sub         $23, $23, $4
    r23 = RSP_SUB32(r23, r4);
    // bgtz        $23, L_179C
    if (RSP_SIGNED(r23) > 0) {
        // mfc0        $5, DPC_CURRENT
        r5 = get_dpc_current();
        goto L_179C;
    }
    // mfc0        $5, DPC_CURRENT
    r5 = get_dpc_current();
    // beq         $5, $zero, L_179C
    if (r5 == 0) {
        // nop
    
        goto L_179C;
    }
    // nop

    // beq         $5, $4, L_179C
    if (r5 == r4) {
        // nop
    
        goto L_179C;
    }
    // nop

    // j           L_17B8
    // ori         $3, $4, 0x0
    r3 = r4 | 0X0;
    goto L_17B8;
L_1798:
    // ori         $3, $4, 0x0
    r3 = r4 | 0X0;
L_179C:
    // mfc0        $4, DPC_STATUS
    r4 = get_dpc_status();
    // andi        $4, $4, 0x400
    r4 = r4 & 0X400;
    // bne         $4, $zero, L_179C
    if (r4 != 0) {
        // addi        $4, $zero, 0x1
        r4 = RSP_ADD32(0, 0X1);
        goto L_179C;
    }
    // addi        $4, $zero, 0x1
    r4 = RSP_ADD32(0, 0X1);
    // mtc0        $4, DPC_STATUS
    set_dpc_status(r4);
    // mtc0        $3, DPC_START
    set_dpc_start(r3);
    // mtc0        $3, DPC_END
    set_dpc_end(r3);
L_17B8:
    // sw          $3, 0x18($29)
    RSP_MEM_W_STORE(0X18, r29, r3);
    // addi        $23, $zero, 0xCE0
    r23 = RSP_ADD32(0, 0XCE0);
L_17C0:
    // lw          $5, 0x10($1)
    r5 = RSP_MEM_W_LOAD(0X10, r1);
    // lw          $2, 0x8($zero)
    r2 = RSP_MEM_W_LOAD(0X8, 0);
    // lw          $3, 0x10($zero)
    r3 = RSP_MEM_W_LOAD(0X10, 0);
    // lw          $4, 0x18($zero)
    r4 = RSP_MEM_W_LOAD(0X18, 0);
    // lw          $6, 0x20($zero)
    r6 = RSP_MEM_W_LOAD(0X20, 0);
    // add         $2, $2, $5
    r2 = RSP_ADD32(r2, r5);
    // add         $3, $3, $5
    r3 = RSP_ADD32(r3, r5);
    // add         $4, $4, $5
    r4 = RSP_ADD32(r4, r5);
    // add         $6, $6, $5
    r6 = RSP_ADD32(r6, r5);
    // sw          $2, 0x8($zero)
    RSP_MEM_W_STORE(0X8, 0, r2);
L_17E8:
    // sw          $3, 0x10($zero)
    RSP_MEM_W_STORE(0X10, 0, r3);
    // sw          $4, 0x18($zero)
    RSP_MEM_W_STORE(0X18, 0, r4);
    // sw          $6, 0x20($zero)
    RSP_MEM_W_STORE(0X20, 0, r6);
    // jal         0x10EC
    r31 = 0x17FC;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
    goto L_10EC;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
L_17FC:
    // jal         0x10C8
    r31 = 0x1804;
    // lw          $26, 0x30($1)
    r26 = RSP_MEM_W_LOAD(0X30, r1);
    goto L_10C8;
    // lw          $26, 0x30($1)
    r26 = RSP_MEM_W_LOAD(0X30, r1);
L_1804:
    // lw          $2, 0x20($1)
    r2 = RSP_MEM_W_LOAD(0X20, r1);
    // lw          $3, 0x24($1)
    r3 = RSP_MEM_W_LOAD(0X24, r1);
    // sw          $2, 0x20($29)
    RSP_MEM_W_STORE(0X20, r29, r2);
L_1810:
    // sw          $2, 0x24($29)
    RSP_MEM_W_STORE(0X24, r29, r2);
    // add         $2, $2, $3
    r2 = RSP_ADD32(r2, r3);
    // sw          $2, 0x4C($29)
    RSP_MEM_W_STORE(0X4C, r29, r2);
    // lw          $2, -0x8($zero)
    r2 = RSP_MEM_W_LOAD(-0X8, 0);
    // sw          $2, 0x108($zero)
    RSP_MEM_W_STORE(0X108, 0, r2);
    // j           L_1058
    // nop

    goto L_1058;
    // nop

L_182C:
    // jal         0x10EC
    r31 = 0x1834;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
    goto L_10EC;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
L_1834:
    // lw          $23, 0xBF0($zero)
    r23 = RSP_MEM_W_LOAD(0XBF0, 0);
L_1838:
    // lw          $28, 0xBE4($zero)
    r28 = RSP_MEM_W_LOAD(0XBE4, 0);
    // lw          $27, 0xBE8($zero)
    r27 = RSP_MEM_W_LOAD(0XBE8, 0);
    // j           L_109C
    // lw          $26, 0xBEC($zero)
    r26 = RSP_MEM_W_LOAD(0XBEC, 0);
    goto L_109C;
    // lw          $26, 0xBEC($zero)
    r26 = RSP_MEM_W_LOAD(0XBEC, 0);
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

L_1860:
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

L_1888:
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

L_18B0:
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

L_18D8:
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

L_1900:
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

L_1928:
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // lh          $11, 0x24($3)
    r11 = RSP_MEM_H_LOAD(0X24, r3);
    // lh          $8, 0x24($2)
    r8 = RSP_MEM_H_LOAD(0X24, r2);
    // lh          $9, 0x24($1)
    r9 = RSP_MEM_H_LOAD(0X24, r1);
    // and         $12, $11, $8
    r12 = r11 & r8;
L_1950:
    // or          $11, $11, $8
    r11 = r11 | r8;
    // and         $12, $12, $9
    r12 = r12 & r9;
    // andi        $12, $12, 0x7070
    r12 = r12 & 0X7070;
    // bne         $12, $zero, L_1F1C
    if (r12 != 0) {
        // or          $11, $11, $9
        r11 = r11 | r9;
        goto L_1F1C;
    }
    // or          $11, $11, $9
    r11 = r11 | r9;
    // andi        $11, $11, 0x4343
    r11 = r11 & 0X4343;
    // bne         $11, $zero, L_1720
    if (r11 != 0) {
        // llv         $v13[0], 0x18($1)
        rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
        goto L_1720;
    }
    // llv         $v13[0], 0x18($1)
    rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
    // llv         $v14[0], 0x18($2)
    rsp.LLV<0>(rsp.vpu.r[14], r2, 0X6);
    // llv         $v15[0], 0x18($3)
    rsp.LLV<0>(rsp.vpu.r[15], r3, 0X6);
L_1978:
    // lw          $13, 0x4($29)
    r13 = RSP_MEM_W_LOAD(0X4, r29);
    // addi        $8, $zero, 0xBE0
    r8 = RSP_ADD32(0, 0XBE0);
    // lsv         $v21[0], 0x2($29)
    rsp.LSV<0>(rsp.vpu.r[21], r29, 0X1);
    // lsv         $v5[0], 0x6($1)
    rsp.LSV<0>(rsp.vpu.r[5], r1, 0X3);
    // vsub        $v10, $v14, $v13
    rsp.VSUB<0>(rsp.vpu.r[10], rsp.vpu.r[14], rsp.vpu.r[13]);
    // lsv         $v6[0], 0xE($1)
    rsp.LSV<0>(rsp.vpu.r[6], r1, 0X7);
    // vsub        $v9, $v15, $v13
    rsp.VSUB<0>(rsp.vpu.r[9], rsp.vpu.r[15], rsp.vpu.r[13]);
    // lsv         $v5[2], 0x6($2)
    rsp.LSV<2>(rsp.vpu.r[5], r2, 0X3);
    // vsub        $v12, $v13, $v14
    rsp.VSUB<0>(rsp.vpu.r[12], rsp.vpu.r[13], rsp.vpu.r[14]);
    // lsv         $v6[2], 0xE($2)
    rsp.LSV<2>(rsp.vpu.r[6], r2, 0X7);
    // lsv         $v5[4], 0x6($3)
    rsp.LSV<4>(rsp.vpu.r[5], r3, 0X3);
    // lsv         $v6[4], 0xE($3)
    rsp.LSV<4>(rsp.vpu.r[6], r3, 0X7);
    // vmudh       $v16, $v9, $v10[1]
    rsp.VMUDH<9>(rsp.vpu.r[16], rsp.vpu.r[9], rsp.vpu.r[10]);
    // lh          $9, 0x1A($1)
    r9 = RSP_MEM_H_LOAD(0X1A, r1);
    // vmadh       $v16, $v12, $v9[1]
    rsp.VMADH<9>(rsp.vpu.r[16], rsp.vpu.r[12], rsp.vpu.r[9]);
    // lh          $10, 0x1A($2)
    r10 = RSP_MEM_H_LOAD(0X1A, r2);
    // vsar        $v28, $v28, $v28[1]
    rsp.VSAR<9>(rsp.vpu.r[28], rsp.vpu.r[28]);
    // lh          $11, 0x1A($3)
    r11 = RSP_MEM_H_LOAD(0X1A, r3);
    // vsar        $v29, $v29, $v29[0]
    rsp.VSAR<8>(rsp.vpu.r[29], rsp.vpu.r[29]);
    // sll         $15, $13, 18
    r15 = S32(r13) << 18;
    // mfc2        $17, $v16[0]
    rsp.MFC2<0>(r17, rsp.vpu.r[16]);
    // sh          $1, 0xBE0($zero)
    RSP_MEM_H_STORE(0XBE0, 0, r1);
    // sh          $2, 0xBE2($zero)
    RSP_MEM_H_STORE(0XBE2, 0, r2);
    // and         $15, $17, $15
    r15 = r17 & r15;
    // bltz        $15, L_1F18
    if (RSP_SIGNED(r15) < 0) {
        // sh          $3, 0xBE4($zero)
        RSP_MEM_H_STORE(0XBE4, 0, r3);
        goto L_1F18;
    }
    // sh          $3, 0xBE4($zero)
    RSP_MEM_H_STORE(0XBE4, 0, r3);
    // slt         $7, $11, $10
    r7 = RSP_SIGNED(r11) < RSP_SIGNED(r10) ? 1 : 0;
    // slt         $12, $10, $9
    r12 = RSP_SIGNED(r10) < RSP_SIGNED(r9) ? 1 : 0;
    // add         $7, $7, $7
    r7 = RSP_ADD32(r7, r7);
    // add         $7, $7, $12
    r7 = RSP_ADD32(r7, r12);
    // slt         $12, $9, $11
    r12 = RSP_SIGNED(r9) < RSP_SIGNED(r11) ? 1 : 0;
    // add         $7, $7, $7
    r7 = RSP_ADD32(r7, r7);
    // add         $7, $7, $12
    r7 = RSP_ADD32(r7, r12);
    // lbu         $3, 0x360($7)
    r3 = RSP_MEM_BU(0X360, r7);
    // lbu         $2, 0x367($7)
    r2 = RSP_MEM_BU(0X367, r7);
    // lbu         $1, 0x36E($7)
    r1 = RSP_MEM_BU(0X36E, r7);
    // lh          $3, 0xBE0($3)
    r3 = RSP_MEM_H_LOAD(0XBE0, r3);
    // lh          $2, 0xBE0($2)
    r2 = RSP_MEM_H_LOAD(0XBE0, r2);
    // lh          $1, 0xBE0($1)
    r1 = RSP_MEM_H_LOAD(0XBE0, r1);
    // lbu         $12, 0x375($7)
    r12 = RSP_MEM_BU(0X375, r7);
    // llv         $v15[0], 0x18($3)
    rsp.LLV<0>(rsp.vpu.r[15], r3, 0X6);
    // llv         $v14[0], 0x18($2)
    rsp.LLV<0>(rsp.vpu.r[14], r2, 0X6);
    // blez        $12, L_1A30
    if (RSP_SIGNED(r12) <= 0) {
        // llv         $v13[0], 0x18($1)
        rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
        goto L_1A30;
    }
    // llv         $v13[0], 0x18($1)
    rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
    // vsubc       $v28, $v31, $v28
    rsp.VSUBC<0>(rsp.vpu.r[28], rsp.vpu.r[31], rsp.vpu.r[28]);
    // vsub        $v29, $v31, $v29
    rsp.VSUB<0>(rsp.vpu.r[29], rsp.vpu.r[31], rsp.vpu.r[29]);
L_1A30:
    // beq         $17, $zero, L_1F1C
    if (r17 == 0) {
        // vsub        $v4, $v15, $v14
        rsp.VSUB<0>(rsp.vpu.r[4], rsp.vpu.r[15], rsp.vpu.r[14]);
        goto L_1F1C;
    }
    // vsub        $v4, $v15, $v14
    rsp.VSUB<0>(rsp.vpu.r[4], rsp.vpu.r[15], rsp.vpu.r[14]);
    // sll         $14, $13, 19
    r14 = S32(r13) << 19;
    // vsub        $v10, $v14, $v13
    rsp.VSUB<0>(rsp.vpu.r[10], rsp.vpu.r[14], rsp.vpu.r[13]);
    // nor         $14, $14, $zero
    r14 = ~(r14 | 0);
    // vsub        $v9, $v15, $v13
    rsp.VSUB<0>(rsp.vpu.r[9], rsp.vpu.r[15], rsp.vpu.r[13]);
    // or          $14, $17, $14
    r14 = r17 | r14;
    // vmov        $v29[3], $v29[0]
    rsp.VMOV<8>(rsp.vpu.r[29], 3, rsp.vpu.r[29]);
    // bgez        $14, L_1F18
    if (RSP_SIGNED(r14) >= 0) {
        // vmov        $v28[3], $v28[0]
        rsp.VMOV<8>(rsp.vpu.r[28], 3, rsp.vpu.r[28]);
        goto L_1F18;
    }
    // vmov        $v28[3], $v28[0]
    rsp.VMOV<8>(rsp.vpu.r[28], 3, rsp.vpu.r[28]);
    // ssv         $v15[2], 0x2($23)
    rsp.SSV<2>(rsp.vpu.r[15], r23, 0X1);
    // vmov        $v4[2], $v10[0]
    rsp.VMOV<8>(rsp.vpu.r[4], 2, rsp.vpu.r[10]);
    // ssv         $v14[2], 0x4($23)
    rsp.SSV<2>(rsp.vpu.r[14], r23, 0X2);
    // vmov        $v4[3], $v10[1]
    rsp.VMOV<9>(rsp.vpu.r[4], 3, rsp.vpu.r[10]);
    // ssv         $v13[2], 0x6($23)
    rsp.SSV<2>(rsp.vpu.r[13], r23, 0X3);
    // vmov        $v4[4], $v9[0]
    rsp.VMOV<8>(rsp.vpu.r[4], 4, rsp.vpu.r[9]);
    // mfc2        $6, $v29[5]
    rsp.MFC2<5>(r6, rsp.vpu.r[29]);
    // vmov        $v4[5], $v9[1]
    rsp.VMOV<9>(rsp.vpu.r[4], 5, rsp.vpu.r[9]);
    // jal         0x1000
    r31 = 0x1A80;
    // vmudn       $v2, $v13, $v31[4]
    rsp.VMUDN<12>(rsp.vpu.r[2], rsp.vpu.r[13], rsp.vpu.r[31]);
    goto L_1000;
    // vmudn       $v2, $v13, $v31[4]
    rsp.VMUDN<12>(rsp.vpu.r[2], rsp.vpu.r[13], rsp.vpu.r[31]);
L_1A80:
    // lb          $5, 0x7($29)
    r5 = RSP_MEM_B(0X7, r29);
    // vmudm       $v9, $v4, $v31[4]
    rsp.VMUDM<12>(rsp.vpu.r[9], rsp.vpu.r[4], rsp.vpu.r[31]);
    // vmadn       $v10, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[10], rsp.vpu.r[31], rsp.vpu.r[31]);
    // andi        $6, $6, 0x80
    r6 = r6 & 0X80;
    // vrcp        $v8[1], $v4[1]
    rsp.VRCP<9>(rsp.vpu.r[8], 1, rsp.vpu.r[4]);
    // ori         $5, $5, 0xC8
    r5 = r5 | 0XC8;
    // vrcph       $v7[1], $v31[0]
    rsp.VRCPH<8>(rsp.vpu.r[7], 1, rsp.vpu.r[31]);
    // lb          $7, 0x12($29)
    r7 = RSP_MEM_B(0X12, r29);
    // vrcp        $v8[3], $v4[3]
    rsp.VRCP<11>(rsp.vpu.r[8], 3, rsp.vpu.r[4]);
    // vrcph       $v7[3], $v31[0]
    rsp.VRCPH<8>(rsp.vpu.r[7], 3, rsp.vpu.r[31]);
    // vrcp        $v8[5], $v4[5]
    rsp.VRCP<13>(rsp.vpu.r[8], 5, rsp.vpu.r[4]);
    // vrcph       $v7[5], $v31[0]
    rsp.VRCPH<8>(rsp.vpu.r[7], 5, rsp.vpu.r[31]);
    // or          $6, $6, $7
    r6 = r6 | r7;
    // vmudl       $v8, $v8, $v30[4]
    rsp.VMUDL<12>(rsp.vpu.r[8], rsp.vpu.r[8], rsp.vpu.r[30]);
    // sb          $5, 0x0($23)
    RSP_MEM_B(0X0, r23) = r5;
    // vmadm       $v7, $v7, $v30[4]
    rsp.VMADM<12>(rsp.vpu.r[7], rsp.vpu.r[7], rsp.vpu.r[30]);
    // sb          $6, 0x1($23)
    RSP_MEM_B(0X1, r23) = r6;
    // vmadn       $v8, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[8], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudh       $v4, $v4, $v31[5]
    rsp.VMUDH<13>(rsp.vpu.r[4], rsp.vpu.r[4], rsp.vpu.r[31]);
    // lsv         $v12[0], 0x18($2)
    rsp.LSV<0>(rsp.vpu.r[12], r2, 0XC);
    // vmudl       $v6, $v6, $v21[0]
    rsp.VMUDL<8>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[21]);
    // lsv         $v12[4], 0x18($1)
    rsp.LSV<4>(rsp.vpu.r[12], r1, 0XC);
    // vmadm       $v5, $v5, $v21[0]
    rsp.VMADM<8>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[21]);
    // lsv         $v12[8], 0x18($1)
    rsp.LSV<8>(rsp.vpu.r[12], r1, 0XC);
    // vmadn       $v6, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[6], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudl       $v1, $v8, $v10[0q]
    rsp.VMUDL<2>(rsp.vpu.r[1], rsp.vpu.r[8], rsp.vpu.r[10]);
    // vmadm       $v1, $v7, $v10[0q]
    rsp.VMADM<2>(rsp.vpu.r[1], rsp.vpu.r[7], rsp.vpu.r[10]);
    // vmadn       $v1, $v8, $v9[0q]
    rsp.VMADN<2>(rsp.vpu.r[1], rsp.vpu.r[8], rsp.vpu.r[9]);
    // vmadh       $v0, $v7, $v9[0q]
    rsp.VMADH<2>(rsp.vpu.r[0], rsp.vpu.r[7], rsp.vpu.r[9]);
    // vmadn       $v1, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[1], rsp.vpu.r[31], rsp.vpu.r[31]);
    // sw          $3, 0x0($8)
    RSP_MEM_W_STORE(0X0, r8, r3);
    // vmudl       $v8, $v8, $v31[4]
    rsp.VMUDL<12>(rsp.vpu.r[8], rsp.vpu.r[8], rsp.vpu.r[31]);
    // vmadm       $v7, $v7, $v31[4]
    rsp.VMADM<12>(rsp.vpu.r[7], rsp.vpu.r[7], rsp.vpu.r[31]);
    // vmadn       $v8, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[8], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudl       $v1, $v1, $v31[4]
    rsp.VMUDL<12>(rsp.vpu.r[1], rsp.vpu.r[1], rsp.vpu.r[31]);
    // vmadm       $v0, $v0, $v31[4]
    rsp.VMADM<12>(rsp.vpu.r[0], rsp.vpu.r[0], rsp.vpu.r[31]);
    // vmadn       $v1, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[1], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vand        $v16, $v1, $v30[1]
    rsp.VAND<9>(rsp.vpu.r[16], rsp.vpu.r[1], rsp.vpu.r[30]);
    // vmudm       $v12, $v12, $v31[4]
    rsp.VMUDM<12>(rsp.vpu.r[12], rsp.vpu.r[12], rsp.vpu.r[31]);
    // sw          $2, 0x4($8)
    RSP_MEM_W_STORE(0X4, r8, r2);
    // vmadn       $v13, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[13], rsp.vpu.r[31], rsp.vpu.r[31]);
    // sw          $1, 0x8($8)
    RSP_MEM_W_STORE(0X8, r8, r1);
    // vcr         $v0, $v0, $v30[6]
    rsp.VCR<14>(rsp.vpu.r[0], rsp.vpu.r[0], rsp.vpu.r[30]);
    // ssv         $v12[0], 0x8($23)
    rsp.SSV<0>(rsp.vpu.r[12], r23, 0X4);
    // vmudl       $v11, $v16, $v2[1]
    rsp.VMUDL<9>(rsp.vpu.r[11], rsp.vpu.r[16], rsp.vpu.r[2]);
    // ssv         $v13[0], 0xA($23)
    rsp.SSV<0>(rsp.vpu.r[13], r23, 0X5);
    // vmadm       $v10, $v0, $v2[1]
    rsp.VMADM<9>(rsp.vpu.r[10], rsp.vpu.r[0], rsp.vpu.r[2]);
    // ssv         $v0[2], 0xC($23)
    rsp.SSV<2>(rsp.vpu.r[0], r23, 0X6);
    // vmadn       $v11, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[11], rsp.vpu.r[31], rsp.vpu.r[31]);
    // ssv         $v1[2], 0xE($23)
    rsp.SSV<2>(rsp.vpu.r[1], r23, 0X7);
    // andi        $7, $5, 0x2
    r7 = r5 & 0X2;
    // addi        $15, $8, 0x8
    r15 = RSP_ADD32(r8, 0X8);
    // addi        $16, $8, 0x10
    r16 = RSP_ADD32(r8, 0X10);
    // vsubc       $v3, $v13, $v11[1q]
    rsp.VSUBC<3>(rsp.vpu.r[3], rsp.vpu.r[13], rsp.vpu.r[11]);
    // ssv         $v0[10], 0x14($23)
    rsp.SSV<10>(rsp.vpu.r[0], r23, 0XA);
    // vsub        $v9, $v12, $v10[1q]
    rsp.VSUB<3>(rsp.vpu.r[9], rsp.vpu.r[12], rsp.vpu.r[10]);
    // ssv         $v1[10], 0x16($23)
    rsp.SSV<10>(rsp.vpu.r[1], r23, 0XB);
    // vsubc       $v21, $v6, $v6[1]
    rsp.VSUBC<9>(rsp.vpu.r[21], rsp.vpu.r[6], rsp.vpu.r[6]);
    // ssv         $v0[6], 0x1C($23)
    rsp.SSV<6>(rsp.vpu.r[0], r23, 0XE);
    // vlt         $v19, $v5, $v5[1]
    rsp.VLT<9>(rsp.vpu.r[19], rsp.vpu.r[5], rsp.vpu.r[5]);
    // ssv         $v1[6], 0x1E($23)
    rsp.SSV<6>(rsp.vpu.r[1], r23, 0XF);
    // vmrg        $v20, $v6, $v6[1]
    rsp.VMRG<9>(rsp.vpu.r[20], rsp.vpu.r[6], rsp.vpu.r[6]);
    // ssv         $v9[8], 0x10($23)
    rsp.SSV<8>(rsp.vpu.r[9], r23, 0X8);
    // vsubc       $v21, $v20, $v6[2]
    rsp.VSUBC<10>(rsp.vpu.r[21], rsp.vpu.r[20], rsp.vpu.r[6]);
    // ssv         $v3[8], 0x12($23)
    rsp.SSV<8>(rsp.vpu.r[3], r23, 0X9);
    // vlt         $v19, $v19, $v5[2]
    rsp.VLT<10>(rsp.vpu.r[19], rsp.vpu.r[19], rsp.vpu.r[5]);
    // ssv         $v9[4], 0x18($23)
    rsp.SSV<4>(rsp.vpu.r[9], r23, 0XC);
    // vmrg        $v20, $v20, $v6[2]
    rsp.VMRG<10>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[6]);
    // ssv         $v3[4], 0x1A($23)
    rsp.SSV<4>(rsp.vpu.r[3], r23, 0XD);
    // addi        $23, $23, 0x20
    r23 = RSP_ADD32(r23, 0X20);
    // blez        $7, L_1C60
    if (RSP_SIGNED(r7) <= 0) {
        // vmudl       $v20, $v20, $v30[5]
        rsp.VMUDL<13>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[30]);
        goto L_1C60;
    }
    // vmudl       $v20, $v20, $v30[5]
    rsp.VMUDL<13>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[30]);
    // lw          $14, 0x0($15)
    r14 = RSP_MEM_W_LOAD(0X0, r15);
    // vmadm       $v19, $v19, $v30[5]
    rsp.VMADM<13>(rsp.vpu.r[19], rsp.vpu.r[19], rsp.vpu.r[30]);
    // lw          $17, -0x4($15)
    r17 = RSP_MEM_W_LOAD(-0X4, r15);
    // vmadn       $v20, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[20], rsp.vpu.r[31], rsp.vpu.r[31]);
    // lw          $18, -0x8($15)
    r18 = RSP_MEM_W_LOAD(-0X8, r15);
    // llv         $v9[0], 0x14($14)
    rsp.LLV<0>(rsp.vpu.r[9], r14, 0X5);
    // llv         $v9[8], 0x14($17)
    rsp.LLV<8>(rsp.vpu.r[9], r17, 0X5);
    // llv         $v22[0], 0x14($18)
    rsp.LLV<0>(rsp.vpu.r[22], r18, 0X5);
    // lsv         $v11[0], 0x22($14)
    rsp.LSV<0>(rsp.vpu.r[11], r14, 0X11);
    // lsv         $v12[0], 0x20($14)
    rsp.LSV<0>(rsp.vpu.r[12], r14, 0X10);
    // lsv         $v11[8], 0x22($17)
    rsp.LSV<8>(rsp.vpu.r[11], r17, 0X11);
    // vmov        $v9[2], $v30[0]
    rsp.VMOV<8>(rsp.vpu.r[9], 2, rsp.vpu.r[30]);
    // lsv         $v12[8], 0x20($17)
    rsp.LSV<8>(rsp.vpu.r[12], r17, 0X10);
    // vmov        $v9[6], $v30[0]
    rsp.VMOV<8>(rsp.vpu.r[9], 6, rsp.vpu.r[30]);
    // lsv         $v24[0], 0x22($18)
    rsp.LSV<0>(rsp.vpu.r[24], r18, 0X11);
    // vmov        $v22[2], $v30[0]
    rsp.VMOV<8>(rsp.vpu.r[22], 2, rsp.vpu.r[30]);
    // lsv         $v25[0], 0x20($18)
    rsp.LSV<0>(rsp.vpu.r[25], r18, 0X10);
    // vmudl       $v6, $v11, $v20[0]
    rsp.VMUDL<8>(rsp.vpu.r[6], rsp.vpu.r[11], rsp.vpu.r[20]);
    // vmadm       $v6, $v12, $v20[0]
    rsp.VMADM<8>(rsp.vpu.r[6], rsp.vpu.r[12], rsp.vpu.r[20]);
    // ssv         $v19[0], 0x44($8)
    rsp.SSV<0>(rsp.vpu.r[19], r8, 0X22);
    // vmadn       $v6, $v11, $v19[0]
    rsp.VMADN<8>(rsp.vpu.r[6], rsp.vpu.r[11], rsp.vpu.r[19]);
    // ssv         $v20[0], 0x4C($8)
    rsp.SSV<0>(rsp.vpu.r[20], r8, 0X26);
    // vmadh       $v5, $v12, $v19[0]
    rsp.VMADH<8>(rsp.vpu.r[5], rsp.vpu.r[12], rsp.vpu.r[19]);
    // vmudl       $v16, $v24, $v20[0]
    rsp.VMUDL<8>(rsp.vpu.r[16], rsp.vpu.r[24], rsp.vpu.r[20]);
    // vmadm       $v16, $v25, $v20[0]
    rsp.VMADM<8>(rsp.vpu.r[16], rsp.vpu.r[25], rsp.vpu.r[20]);
    // vmadn       $v20, $v24, $v19[0]
    rsp.VMADN<8>(rsp.vpu.r[20], rsp.vpu.r[24], rsp.vpu.r[19]);
    // vmadh       $v19, $v25, $v19[0]
    rsp.VMADH<8>(rsp.vpu.r[19], rsp.vpu.r[25], rsp.vpu.r[19]);
    // vmudm       $v16, $v9, $v6[0h]
    rsp.VMUDM<4>(rsp.vpu.r[16], rsp.vpu.r[9], rsp.vpu.r[6]);
    // vmadh       $v9, $v9, $v5[0h]
    rsp.VMADH<4>(rsp.vpu.r[9], rsp.vpu.r[9], rsp.vpu.r[5]);
    // vmadn       $v10, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[10], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudm       $v16, $v22, $v20[0]
    rsp.VMUDM<8>(rsp.vpu.r[16], rsp.vpu.r[22], rsp.vpu.r[20]);
    // vmadh       $v22, $v22, $v19[0]
    rsp.VMADH<8>(rsp.vpu.r[22], rsp.vpu.r[22], rsp.vpu.r[19]);
    // vmadn       $v23, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[31]);
    // sdv         $v9[8], 0x10($16)
    rsp.SDV<8>(rsp.vpu.r[9], r16, 0X2);
    // sdv         $v10[8], 0x18($16)
    rsp.SDV<8>(rsp.vpu.r[10], r16, 0X3);
    // sdv         $v9[0], 0x0($16)
    rsp.SDV<0>(rsp.vpu.r[9], r16, 0X0);
    // sdv         $v10[0], 0x8($16)
    rsp.SDV<0>(rsp.vpu.r[10], r16, 0X1);
    // sdv         $v22[0], 0x20($16)
    rsp.SDV<0>(rsp.vpu.r[22], r16, 0X4);
    // sdv         $v23[0], 0x28($16)
    rsp.SDV<0>(rsp.vpu.r[23], r16, 0X5);
    // vabs        $v9, $v9, $v9
    rsp.VABS<0>(rsp.vpu.r[9], rsp.vpu.r[9], rsp.vpu.r[9]);
    // llv         $v19[0], 0x10($16)
    rsp.LLV<0>(rsp.vpu.r[19], r16, 0X4);
    // vabs        $v22, $v22, $v22
    rsp.VABS<0>(rsp.vpu.r[22], rsp.vpu.r[22], rsp.vpu.r[22]);
    // llv         $v20[0], 0x18($16)
    rsp.LLV<0>(rsp.vpu.r[20], r16, 0X6);
    // vabs        $v19, $v19, $v19
    rsp.VABS<0>(rsp.vpu.r[19], rsp.vpu.r[19], rsp.vpu.r[19]);
    // vge         $v17, $v9, $v22
    rsp.VGE<0>(rsp.vpu.r[17], rsp.vpu.r[9], rsp.vpu.r[22]);
    // vmrg        $v18, $v10, $v23
    rsp.VMRG<0>(rsp.vpu.r[18], rsp.vpu.r[10], rsp.vpu.r[23]);
    // vge         $v17, $v17, $v19
    rsp.VGE<0>(rsp.vpu.r[17], rsp.vpu.r[17], rsp.vpu.r[19]);
    // vmrg        $v18, $v18, $v20
    rsp.VMRG<0>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[20]);
L_1C60:
    // slv         $v17[0], 0x40($8)
    rsp.SLV<0>(rsp.vpu.r[17], r8, 0X10);
    // slv         $v18[0], 0x48($8)
    rsp.SLV<0>(rsp.vpu.r[18], r8, 0X12);
    // andi        $7, $5, 0x7
    r7 = r5 & 0X7;
    // blez        $7, L_1F10
    if (RSP_SIGNED(r7) <= 0) {
        // vxor        $v18, $v31, $v31
        rsp.VXOR<0>(rsp.vpu.r[18], rsp.vpu.r[31], rsp.vpu.r[31]);
        goto L_1F10;
    }
    // vxor        $v18, $v31, $v31
    rsp.VXOR<0>(rsp.vpu.r[18], rsp.vpu.r[31], rsp.vpu.r[31]);
    // luv         $v25[0], 0x10($3)
    rsp.LUV<0>(rsp.vpu.r[25], r3, 0X2);
    // vadd        $v16, $v18, $v30[5]
    rsp.VADD<13>(rsp.vpu.r[16], rsp.vpu.r[18], rsp.vpu.r[30]);
    // luv         $v15[0], 0x10($1)
    rsp.LUV<0>(rsp.vpu.r[15], r1, 0X2);
    // vadd        $v24, $v18, $v30[5]
    rsp.VADD<13>(rsp.vpu.r[24], rsp.vpu.r[18], rsp.vpu.r[30]);
    // andi        $7, $13, 0x200
    r7 = r13 & 0X200;
    // vadd        $v5, $v18, $v30[5]
    rsp.VADD<13>(rsp.vpu.r[5], rsp.vpu.r[18], rsp.vpu.r[30]);
    // bgtz        $7, L_1CA0
    if (RSP_SIGNED(r7) > 0) {
        // luv         $v23[0], 0x10($2)
        rsp.LUV<0>(rsp.vpu.r[23], r2, 0X2);
        goto L_1CA0;
    }
    // luv         $v23[0], 0x10($2)
    rsp.LUV<0>(rsp.vpu.r[23], r2, 0X2);
    // luv         $v25[0], 0x10($4)
    rsp.LUV<0>(rsp.vpu.r[25], r4, 0X2);
    // luv         $v15[0], 0x10($4)
    rsp.LUV<0>(rsp.vpu.r[15], r4, 0X2);
    // luv         $v23[0], 0x10($4)
    rsp.LUV<0>(rsp.vpu.r[23], r4, 0X2);
L_1CA0:
    // vmudm       $v25, $v25, $v31[7]
    rsp.VMUDM<15>(rsp.vpu.r[25], rsp.vpu.r[25], rsp.vpu.r[31]);
    // vmudm       $v15, $v15, $v31[7]
    rsp.VMUDM<15>(rsp.vpu.r[15], rsp.vpu.r[15], rsp.vpu.r[31]);
    // vmudm       $v23, $v23, $v31[7]
    rsp.VMUDM<15>(rsp.vpu.r[23], rsp.vpu.r[23], rsp.vpu.r[31]);
    // ldv         $v16[8], 0x18($8)
    rsp.LDV<8>(rsp.vpu.r[16], r8, 0X3);
    // ldv         $v15[8], 0x10($8)
    rsp.LDV<8>(rsp.vpu.r[15], r8, 0X2);
    // ldv         $v24[8], 0x28($8)
    rsp.LDV<8>(rsp.vpu.r[24], r8, 0X5);
    // ldv         $v23[8], 0x20($8)
    rsp.LDV<8>(rsp.vpu.r[23], r8, 0X4);
    // ldv         $v5[8], 0x38($8)
    rsp.LDV<8>(rsp.vpu.r[5], r8, 0X7);
    // ldv         $v25[8], 0x30($8)
    rsp.LDV<8>(rsp.vpu.r[25], r8, 0X6);
    // lsv         $v16[14], 0x1E($1)
    rsp.LSV<14>(rsp.vpu.r[16], r1, 0XF);
    // lsv         $v15[14], 0x1C($1)
    rsp.LSV<14>(rsp.vpu.r[15], r1, 0XE);
    // lsv         $v24[14], 0x1E($2)
    rsp.LSV<14>(rsp.vpu.r[24], r2, 0XF);
    // lsv         $v23[14], 0x1C($2)
    rsp.LSV<14>(rsp.vpu.r[23], r2, 0XE);
    // lsv         $v5[14], 0x1E($3)
    rsp.LSV<14>(rsp.vpu.r[5], r3, 0XF);
    // lsv         $v25[14], 0x1C($3)
    rsp.LSV<14>(rsp.vpu.r[25], r3, 0XE);
    // vsubc       $v12, $v24, $v16
    rsp.VSUBC<0>(rsp.vpu.r[12], rsp.vpu.r[24], rsp.vpu.r[16]);
    // vsub        $v11, $v23, $v15
    rsp.VSUB<0>(rsp.vpu.r[11], rsp.vpu.r[23], rsp.vpu.r[15]);
    // vsubc       $v20, $v16, $v5
    rsp.VSUBC<0>(rsp.vpu.r[20], rsp.vpu.r[16], rsp.vpu.r[5]);
    // vsub        $v19, $v15, $v25
    rsp.VSUB<0>(rsp.vpu.r[19], rsp.vpu.r[15], rsp.vpu.r[25]);
    // vsubc       $v10, $v5, $v16
    rsp.VSUBC<0>(rsp.vpu.r[10], rsp.vpu.r[5], rsp.vpu.r[16]);
    // vsub        $v9, $v25, $v15
    rsp.VSUB<0>(rsp.vpu.r[9], rsp.vpu.r[25], rsp.vpu.r[15]);
    // vsubc       $v22, $v16, $v24
    rsp.VSUBC<0>(rsp.vpu.r[22], rsp.vpu.r[16], rsp.vpu.r[24]);
    // vsub        $v21, $v15, $v23
    rsp.VSUB<0>(rsp.vpu.r[21], rsp.vpu.r[15], rsp.vpu.r[23]);
    // vmudn       $v6, $v10, $v4[3]
    rsp.VMUDN<11>(rsp.vpu.r[6], rsp.vpu.r[10], rsp.vpu.r[4]);
    // vmadh       $v6, $v9, $v4[3]
    rsp.VMADH<11>(rsp.vpu.r[6], rsp.vpu.r[9], rsp.vpu.r[4]);
    // vmadn       $v6, $v22, $v4[5]
    rsp.VMADN<13>(rsp.vpu.r[6], rsp.vpu.r[22], rsp.vpu.r[4]);
    // vmadh       $v6, $v21, $v4[5]
    rsp.VMADH<13>(rsp.vpu.r[6], rsp.vpu.r[21], rsp.vpu.r[4]);
    // vsar        $v9, $v9, $v9[0]
    rsp.VSAR<8>(rsp.vpu.r[9], rsp.vpu.r[9]);
    // vsar        $v10, $v10, $v10[1]
    rsp.VSAR<9>(rsp.vpu.r[10], rsp.vpu.r[10]);
    // vmudn       $v6, $v12, $v4[4]
    rsp.VMUDN<12>(rsp.vpu.r[6], rsp.vpu.r[12], rsp.vpu.r[4]);
    // vmadh       $v6, $v11, $v4[4]
    rsp.VMADH<12>(rsp.vpu.r[6], rsp.vpu.r[11], rsp.vpu.r[4]);
    // vmadn       $v6, $v20, $v4[2]
    rsp.VMADN<10>(rsp.vpu.r[6], rsp.vpu.r[20], rsp.vpu.r[4]);
    // vmadh       $v6, $v19, $v4[2]
    rsp.VMADH<10>(rsp.vpu.r[6], rsp.vpu.r[19], rsp.vpu.r[4]);
    // vsar        $v11, $v11, $v11[0]
    rsp.VSAR<8>(rsp.vpu.r[11], rsp.vpu.r[11]);
    // vsar        $v12, $v12, $v12[1]
    rsp.VSAR<9>(rsp.vpu.r[12], rsp.vpu.r[12]);
    // vmudl       $v6, $v10, $v26[3]
    rsp.VMUDL<11>(rsp.vpu.r[6], rsp.vpu.r[10], rsp.vpu.r[26]);
    // vmadm       $v6, $v9, $v26[3]
    rsp.VMADM<11>(rsp.vpu.r[6], rsp.vpu.r[9], rsp.vpu.r[26]);
    // vmadn       $v10, $v10, $v27[3]
    rsp.VMADN<11>(rsp.vpu.r[10], rsp.vpu.r[10], rsp.vpu.r[27]);
    // vmadh       $v9, $v9, $v27[3]
    rsp.VMADH<11>(rsp.vpu.r[9], rsp.vpu.r[9], rsp.vpu.r[27]);
    // vmudl       $v6, $v12, $v26[3]
    rsp.VMUDL<11>(rsp.vpu.r[6], rsp.vpu.r[12], rsp.vpu.r[26]);
    // vmadm       $v6, $v11, $v26[3]
    rsp.VMADM<11>(rsp.vpu.r[6], rsp.vpu.r[11], rsp.vpu.r[26]);
    // vmadn       $v12, $v12, $v27[3]
    rsp.VMADN<11>(rsp.vpu.r[12], rsp.vpu.r[12], rsp.vpu.r[27]);
    // sdv         $v9[0], 0x8($23)
    rsp.SDV<0>(rsp.vpu.r[9], r23, 0X1);
    // vmadh       $v11, $v11, $v27[3]
    rsp.VMADH<11>(rsp.vpu.r[11], rsp.vpu.r[11], rsp.vpu.r[27]);
    // sdv         $v10[0], 0x18($23)
    rsp.SDV<0>(rsp.vpu.r[10], r23, 0X3);
    // vmudn       $v6, $v12, $v31[1]
    rsp.VMUDN<9>(rsp.vpu.r[6], rsp.vpu.r[12], rsp.vpu.r[31]);
    // vmadh       $v6, $v11, $v31[1]
    rsp.VMADH<9>(rsp.vpu.r[6], rsp.vpu.r[11], rsp.vpu.r[31]);
    // vmadl       $v6, $v10, $v1[5]
    rsp.VMADL<13>(rsp.vpu.r[6], rsp.vpu.r[10], rsp.vpu.r[1]);
    // vmadm       $v6, $v9, $v1[5]
    rsp.VMADM<13>(rsp.vpu.r[6], rsp.vpu.r[9], rsp.vpu.r[1]);
    // vmadn       $v14, $v10, $v0[5]
    rsp.VMADN<13>(rsp.vpu.r[14], rsp.vpu.r[10], rsp.vpu.r[0]);
    // sdv         $v11[0], 0x28($23)
    rsp.SDV<0>(rsp.vpu.r[11], r23, 0X5);
    // vmadh       $v13, $v9, $v0[5]
    rsp.VMADH<13>(rsp.vpu.r[13], rsp.vpu.r[9], rsp.vpu.r[0]);
    // sdv         $v12[0], 0x38($23)
    rsp.SDV<0>(rsp.vpu.r[12], r23, 0X7);
    // vmudl       $v28, $v14, $v2[1]
    rsp.VMUDL<9>(rsp.vpu.r[28], rsp.vpu.r[14], rsp.vpu.r[2]);
    // sdv         $v13[0], 0x20($23)
    rsp.SDV<0>(rsp.vpu.r[13], r23, 0X4);
    // vmadm       $v6, $v13, $v2[1]
    rsp.VMADM<9>(rsp.vpu.r[6], rsp.vpu.r[13], rsp.vpu.r[2]);
    // sdv         $v14[0], 0x30($23)
    rsp.SDV<0>(rsp.vpu.r[14], r23, 0X6);
    // vmadn       $v28, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[28], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vsubc       $v18, $v16, $v28
    rsp.VSUBC<0>(rsp.vpu.r[18], rsp.vpu.r[16], rsp.vpu.r[28]);
    // vsub        $v17, $v15, $v6
    rsp.VSUB<0>(rsp.vpu.r[17], rsp.vpu.r[15], rsp.vpu.r[6]);
    // andi        $7, $5, 0x4
    r7 = r5 & 0X4;
    // blez        $7, L_1DA8
    if (RSP_SIGNED(r7) <= 0) {
        // andi        $7, $5, 0x2
        r7 = r5 & 0X2;
        goto L_1DA8;
    }
    // andi        $7, $5, 0x2
    r7 = r5 & 0X2;
    // addi        $23, $23, 0x40
    r23 = RSP_ADD32(r23, 0X40);
    // sdv         $v17[0], 0x3C0($23)
    rsp.SDV<0>(rsp.vpu.r[17], r23, -0X8);
    // sdv         $v18[0], 0x3D0($23)
    rsp.SDV<0>(rsp.vpu.r[18], r23, -0X6);
L_1DA8:
    // blez        $7, L_1EA4
    if (RSP_SIGNED(r7) <= 0) {
        // andi        $7, $5, 0x1
        r7 = r5 & 0X1;
        goto L_1EA4;
    }
    // andi        $7, $5, 0x1
    r7 = r5 & 0X1;
    // addi        $16, $zero, 0x800
    r16 = RSP_ADD32(0, 0X800);
    // mtc2        $16, $v19[0]
    rsp.MTC2<0>(r16, rsp.vpu.r[19]);
    // vabs        $v24, $v9, $v9
    rsp.VABS<0>(rsp.vpu.r[24], rsp.vpu.r[9], rsp.vpu.r[9]);
    // ldv         $v20[8], 0x40($8)
    rsp.LDV<8>(rsp.vpu.r[20], r8, 0X8);
    // vabs        $v25, $v11, $v11
    rsp.VABS<0>(rsp.vpu.r[25], rsp.vpu.r[11], rsp.vpu.r[11]);
    // ldv         $v21[8], 0x48($8)
    rsp.LDV<8>(rsp.vpu.r[21], r8, 0X9);
    // vmudm       $v24, $v24, $v19[0]
    rsp.VMUDM<8>(rsp.vpu.r[24], rsp.vpu.r[24], rsp.vpu.r[19]);
    // vmadn       $v26, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[26], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudm       $v25, $v25, $v19[0]
    rsp.VMUDM<8>(rsp.vpu.r[25], rsp.vpu.r[25], rsp.vpu.r[19]);
    // vmadn       $v27, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[27], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudl       $v21, $v21, $v19[0]
    rsp.VMUDL<8>(rsp.vpu.r[21], rsp.vpu.r[21], rsp.vpu.r[19]);
    // vmadm       $v20, $v20, $v19[0]
    rsp.VMADM<8>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[19]);
    // vmadn       $v21, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudn       $v26, $v26, $v31[2]
    rsp.VMUDN<10>(rsp.vpu.r[26], rsp.vpu.r[26], rsp.vpu.r[31]);
    // vmadh       $v24, $v24, $v31[2]
    rsp.VMADH<10>(rsp.vpu.r[24], rsp.vpu.r[24], rsp.vpu.r[31]);
    // vmadn       $v26, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[26], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmadn       $v23, $v27, $v31[1]
    rsp.VMADN<9>(rsp.vpu.r[23], rsp.vpu.r[27], rsp.vpu.r[31]);
    // vmadh       $v22, $v25, $v31[1]
    rsp.VMADH<9>(rsp.vpu.r[22], rsp.vpu.r[25], rsp.vpu.r[31]);
    // vmadn       $v6, $v21, $v31[1]
    rsp.VMADN<9>(rsp.vpu.r[6], rsp.vpu.r[21], rsp.vpu.r[31]);
    // vmadh       $v5, $v20, $v31[1]
    rsp.VMADH<9>(rsp.vpu.r[5], rsp.vpu.r[20], rsp.vpu.r[31]);
    // vsubc       $v23, $v6, $v6[5]
    rsp.VSUBC<13>(rsp.vpu.r[23], rsp.vpu.r[6], rsp.vpu.r[6]);
    // vge         $v5, $v5, $v5[5]
    rsp.VGE<13>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[5]);
    // vmrg        $v6, $v6, $v6[5]
    rsp.VMRG<13>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[6]);
    // vsubc       $v23, $v6, $v6[6]
    rsp.VSUBC<14>(rsp.vpu.r[23], rsp.vpu.r[6], rsp.vpu.r[6]);
    // vge         $v5, $v5, $v5[6]
    rsp.VGE<14>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[5]);
    // vmrg        $v6, $v6, $v6[6]
    rsp.VMRG<14>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[6]);
    // vmudl       $v6, $v6, $v30[3]
    rsp.VMUDL<11>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[30]);
    // vmadm       $v5, $v5, $v30[3]
    rsp.VMADM<11>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[30]);
    // vmadn       $v6, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[6], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vrcph       $v23[0], $v5[4]
    rsp.VRCPH<12>(rsp.vpu.r[23], 0, rsp.vpu.r[5]);
    // vrcpl       $v6[0], $v6[4]
    rsp.VRCPL<12>(rsp.vpu.r[6], 0, rsp.vpu.r[6]);
    // vrcph       $v5[0], $v31[0]
    rsp.VRCPH<8>(rsp.vpu.r[5], 0, rsp.vpu.r[31]);
    // vmudn       $v6, $v6, $v31[2]
    rsp.VMUDN<10>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[31]);
    // vmadh       $v5, $v5, $v31[2]
    rsp.VMADH<10>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[31]);
    // vlt         $v5, $v5, $v31[1]
    rsp.VLT<9>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[31]);
    // vmrg        $v6, $v6, $v31[0]
    rsp.VMRG<8>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[31]);
    // vmudl       $v20, $v18, $v6[0]
    rsp.VMUDL<8>(rsp.vpu.r[20], rsp.vpu.r[18], rsp.vpu.r[6]);
    // vmadm       $v20, $v17, $v6[0]
    rsp.VMADM<8>(rsp.vpu.r[20], rsp.vpu.r[17], rsp.vpu.r[6]);
    // vmadn       $v20, $v18, $v5[0]
    rsp.VMADN<8>(rsp.vpu.r[20], rsp.vpu.r[18], rsp.vpu.r[5]);
    // vmadh       $v19, $v17, $v5[0]
    rsp.VMADH<8>(rsp.vpu.r[19], rsp.vpu.r[17], rsp.vpu.r[5]);
    // vmudl       $v22, $v10, $v6[0]
    rsp.VMUDL<8>(rsp.vpu.r[22], rsp.vpu.r[10], rsp.vpu.r[6]);
    // vmadm       $v22, $v9, $v6[0]
    rsp.VMADM<8>(rsp.vpu.r[22], rsp.vpu.r[9], rsp.vpu.r[6]);
    // vmadn       $v22, $v10, $v5[0]
    rsp.VMADN<8>(rsp.vpu.r[22], rsp.vpu.r[10], rsp.vpu.r[5]);
    // sdv         $v19[8], 0x0($23)
    rsp.SDV<8>(rsp.vpu.r[19], r23, 0X0);
    // vmadh       $v21, $v9, $v5[0]
    rsp.VMADH<8>(rsp.vpu.r[21], rsp.vpu.r[9], rsp.vpu.r[5]);
    // sdv         $v20[8], 0x10($23)
    rsp.SDV<8>(rsp.vpu.r[20], r23, 0X2);
    // vmudl       $v24, $v12, $v6[0]
    rsp.VMUDL<8>(rsp.vpu.r[24], rsp.vpu.r[12], rsp.vpu.r[6]);
    // vmadm       $v24, $v11, $v6[0]
    rsp.VMADM<8>(rsp.vpu.r[24], rsp.vpu.r[11], rsp.vpu.r[6]);
    // vmadn       $v24, $v12, $v5[0]
    rsp.VMADN<8>(rsp.vpu.r[24], rsp.vpu.r[12], rsp.vpu.r[5]);
    // sdv         $v21[8], 0x8($23)
    rsp.SDV<8>(rsp.vpu.r[21], r23, 0X1);
    // vmadh       $v23, $v11, $v5[0]
    rsp.VMADH<8>(rsp.vpu.r[23], rsp.vpu.r[11], rsp.vpu.r[5]);
    // sdv         $v22[8], 0x18($23)
    rsp.SDV<8>(rsp.vpu.r[22], r23, 0X3);
    // vmudl       $v26, $v14, $v6[0]
    rsp.VMUDL<8>(rsp.vpu.r[26], rsp.vpu.r[14], rsp.vpu.r[6]);
    // vmadm       $v26, $v13, $v6[0]
    rsp.VMADM<8>(rsp.vpu.r[26], rsp.vpu.r[13], rsp.vpu.r[6]);
    // vmadn       $v26, $v14, $v5[0]
    rsp.VMADN<8>(rsp.vpu.r[26], rsp.vpu.r[14], rsp.vpu.r[5]);
    // sdv         $v23[8], 0x28($23)
    rsp.SDV<8>(rsp.vpu.r[23], r23, 0X5);
    // vmadh       $v25, $v13, $v5[0]
    rsp.VMADH<8>(rsp.vpu.r[25], rsp.vpu.r[13], rsp.vpu.r[5]);
    // sdv         $v24[8], 0x38($23)
    rsp.SDV<8>(rsp.vpu.r[24], r23, 0X7);
    // addi        $23, $23, 0x40
    r23 = RSP_ADD32(r23, 0X40);
    // sdv         $v25[8], 0x3E0($23)
    rsp.SDV<8>(rsp.vpu.r[25], r23, -0X4);
    // sdv         $v26[8], 0x3F0($23)
    rsp.SDV<8>(rsp.vpu.r[26], r23, -0X2);
L_1EA4:
    // blez        $7, L_1F10
    if (RSP_SIGNED(r7) <= 0) {
        // vmudn       $v14, $v14, $v30[4]
        rsp.VMUDN<12>(rsp.vpu.r[14], rsp.vpu.r[14], rsp.vpu.r[30]);
        goto L_1F10;
    }
    // vmudn       $v14, $v14, $v30[4]
    rsp.VMUDN<12>(rsp.vpu.r[14], rsp.vpu.r[14], rsp.vpu.r[30]);
    // vmadh       $v13, $v13, $v30[4]
    rsp.VMADH<12>(rsp.vpu.r[13], rsp.vpu.r[13], rsp.vpu.r[30]);
    // vmadn       $v14, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[14], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudn       $v16, $v16, $v30[4]
    rsp.VMUDN<12>(rsp.vpu.r[16], rsp.vpu.r[16], rsp.vpu.r[30]);
    // vmadh       $v15, $v15, $v30[4]
    rsp.VMADH<12>(rsp.vpu.r[15], rsp.vpu.r[15], rsp.vpu.r[30]);
    // vmadn       $v16, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[16], rsp.vpu.r[31], rsp.vpu.r[31]);
    // ssv         $v13[14], 0x8($23)
    rsp.SSV<14>(rsp.vpu.r[13], r23, 0X4);
    // vmudn       $v10, $v10, $v30[4]
    rsp.VMUDN<12>(rsp.vpu.r[10], rsp.vpu.r[10], rsp.vpu.r[30]);
    // ssv         $v14[14], 0xA($23)
    rsp.SSV<14>(rsp.vpu.r[14], r23, 0X5);
    // vmadh       $v9, $v9, $v30[4]
    rsp.VMADH<12>(rsp.vpu.r[9], rsp.vpu.r[9], rsp.vpu.r[30]);
    // vmadn       $v10, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[10], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vmudn       $v12, $v12, $v30[4]
    rsp.VMUDN<12>(rsp.vpu.r[12], rsp.vpu.r[12], rsp.vpu.r[30]);
    // vmadh       $v11, $v11, $v30[4]
    rsp.VMADH<12>(rsp.vpu.r[11], rsp.vpu.r[11], rsp.vpu.r[30]);
    // vmadn       $v12, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[12], rsp.vpu.r[31], rsp.vpu.r[31]);
    // ssv         $v9[14], 0x4($23)
    rsp.SSV<14>(rsp.vpu.r[9], r23, 0X2);
    // vmudl       $v28, $v14, $v2[1]
    rsp.VMUDL<9>(rsp.vpu.r[28], rsp.vpu.r[14], rsp.vpu.r[2]);
    // ssv         $v10[14], 0x6($23)
    rsp.SSV<14>(rsp.vpu.r[10], r23, 0X3);
    // vmadm       $v6, $v13, $v2[1]
    rsp.VMADM<9>(rsp.vpu.r[6], rsp.vpu.r[13], rsp.vpu.r[2]);
    // ssv         $v11[14], 0xC($23)
    rsp.SSV<14>(rsp.vpu.r[11], r23, 0X6);
    // vmadn       $v28, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[28], rsp.vpu.r[31], rsp.vpu.r[31]);
    // ssv         $v12[14], 0xE($23)
    rsp.SSV<14>(rsp.vpu.r[12], r23, 0X7);
    // vsubc       $v18, $v16, $v28
    rsp.VSUBC<0>(rsp.vpu.r[18], rsp.vpu.r[16], rsp.vpu.r[28]);
    // vsub        $v17, $v15, $v6
    rsp.VSUB<0>(rsp.vpu.r[17], rsp.vpu.r[15], rsp.vpu.r[6]);
    // addi        $23, $23, 0x10
    r23 = RSP_ADD32(r23, 0X10);
    // ssv         $v17[14], 0xF0($23)
    rsp.SSV<14>(rsp.vpu.r[17], r23, -0X8);
    // ssv         $v18[14], 0xF2($23)
    rsp.SSV<14>(rsp.vpu.r[18], r23, -0X7);
L_1F10:
    // jal         0x1128
    r31 = 0x1F18;
    // addi        $ra, $24, 0x0
    r31 = RSP_ADD32(r24, 0X0);
    goto L_1128;
    // addi        $ra, $24, 0x0
    r31 = RSP_ADD32(r24, 0X0);
L_1F18:
    // addi        $ra, $zero, 0x0
    r31 = RSP_ADD32(0, 0X0);
L_1F1C:
    // jr          $24
    jump_target = r24;
    debug_file = __FILE__; debug_line = __LINE__;
    // nop

    goto do_indirect_jump;
    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // nop

    // addi        $19, $24, 0x0
    r19 = RSP_ADD32(r24, 0X0);
    // sw          $24, 0xFD0($zero)
    RSP_MEM_W_STORE(0XFD0, 0, r24);
    // add         $26, $26, $27
    r26 = RSP_ADD32(r26, r27);
    // sub         $26, $26, $28
    r26 = RSP_SUB32(r26, r28);
    // sw          $26, 0xFF0($zero)
    RSP_MEM_W_STORE(0XFF0, 0, r26);
    // addi        $20, $zero, 0x1080
    r20 = RSP_ADD32(0, 0X1080);
    // jal         0x1FD4
    r31 = 0x1FAC;
    // addi        $18, $zero, 0xF17
    r18 = RSP_ADD32(0, 0XF17);
    goto L_1FD4;
    // addi        $18, $zero, 0xF17
    r18 = RSP_ADD32(0, 0XF17);
L_1FAC:
    // lw          $19, 0x10C($zero)
    r19 = RSP_MEM_W_LOAD(0X10C, 0);
    // addi        $20, $zero, 0x0
    r20 = RSP_ADD32(0, 0X0);
    // jal         0x1FD4
    r31 = 0x1FBC;
    // andi        $18, $25, 0xFFFF
    r18 = r25 & 0XFFFF;
    goto L_1FD4;
    // andi        $18, $25, 0xFFFF
    r18 = r25 & 0XFFFF;
L_1FBC:
    // addi        $1, $zero, 0xFC0
    r1 = RSP_ADD32(0, 0XFC0);
    // addi        $ra, $zero, 0x1080
    r31 = RSP_ADD32(0, 0X1080);
L_1FC4:
    // mfc0        $11, SP_DMA_BUSY
    r11 = 0;
    // bne         $11, $zero, L_1FC4
    if (r11 != 0) {
        // nop
    
        goto L_1FC4;
    }
    // nop

    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // add         $17, $zero, $zero
    r17 = RSP_ADD32(0, 0);
    goto do_indirect_jump;
L_1FD4:
    // add         $17, $zero, $zero
    r17 = RSP_ADD32(0, 0);
L_1FD8:
    // mfc0        $11, SP_DMA_FULL
    r11 = 0;
    // bne         $11, $zero, L_1FD8
    if (r11 != 0) {
        // nop
    
        goto L_1FD8;
    }
    // nop

    // mtc0        $20, SP_MEM_ADDR
    SET_DMA_MEM(r20);
    // bgtz        $17, L_1FF8
    if (RSP_SIGNED(r17) > 0) {
        // mtc0        $19, SP_DRAM_ADDR
        SET_DMA_DRAM(r19);
        goto L_1FF8;
    }
    // mtc0        $19, SP_DRAM_ADDR
    SET_DMA_DRAM(r19);
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // mtc0        $18, SP_RD_LEN
    DO_DMA_READ(r18);
    goto do_indirect_jump;
    // mtc0        $18, SP_RD_LEN
    DO_DMA_READ(r18);
L_1FF8:
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // mtc0        $18, SP_WR_LEN
    DO_DMA_WRITE(r18);
    goto do_indirect_jump;
    // mtc0        $18, SP_WR_LEN
    DO_DMA_WRITE(r18);
    return RspExitReason::ImemOverrun;
do_indirect_jump:
    switch ((jump_target | 0x1000) & 0X1FFF) {
        // Boot stub's own indirect jumps (merged in above at L_1000..L_10C4).
        case 0x1034: goto L_1034;
        case 0x107C: goto L_107C;
        case 0x10C4: goto L_10C4;
        // F3DEX's real entry (boot stub hands off here via "jr $7", r7=0x1080).
        case 0x1080: goto L_1080;
        case 0x18D8: goto L_18D8;
        case 0x18B0: goto L_18B0;
        case 0x1838: goto L_1838;
        case 0x1810: goto L_1810;
        case 0x17E8: goto L_17E8;
        case 0x1770: goto L_1770;
        case 0x1748: goto L_1748;
        case 0x16F8: goto L_16F8;
        case 0x16D0: goto L_16D0;
        case 0x11B0: goto L_11B0;
        case 0x1380: goto L_1380;
        case 0x1834: goto L_1834;
        case 0x1540: goto L_1540;
        case 0x17FC: goto L_17FC;
        case 0x1710: goto L_1710;
        case 0x1100: goto L_1100;
        case 0x11EC: goto L_11EC;
        case 0x1F18: goto L_1F18;
        case 0x1230: goto L_1230;
        case 0x15E0: goto L_15E0;
        case 0x1594: goto L_1594;
        case 0x12D0: goto L_12D0;
        case 0x1680: goto L_1680;
        case 0x147C: goto L_147C;
        case 0x1390: goto L_1390;
        case 0x1568: goto L_1568;
        case 0x1FBC: goto L_1FBC;
        case 0x13D4: goto L_13D4;
        case 0x1900: goto L_1900;
        case 0x1110: goto L_1110;
        case 0x10E4: goto L_10E4;
        case 0x11D0: goto L_11D0;
        case 0x1108: goto L_1108;
        case 0x1370: goto L_1370;
        case 0x1798: goto L_1798;
        case 0x159C: goto L_159C;
        case 0x13A0: goto L_13A0;
        case 0x1FAC: goto L_1FAC;
        case 0x11A0: goto L_11A0;
        case 0x1090: goto L_1090;
        case 0x17C0: goto L_17C0;
        case 0x13C8: goto L_13C8;
        case 0x15E8: goto L_15E8;
        case 0x11F0: goto L_11F0;
        case 0x1228: goto L_1228;
        case 0x1978: goto L_1978;
        case 0x1384: goto L_1384;
        case 0x1470: goto L_1470;
        case 0x1250: goto L_1250;
        case 0x1270: goto L_1270;
        case 0x1A80: goto L_1A80;
        case 0x1290: goto L_1290;
        case 0x16A8: goto L_16A8;
        case 0x12B0: goto L_12B0;
        case 0x1928: goto L_1928;
        case 0x1138: goto L_1138;
        case 0x1480: goto L_1480;
        case 0x1658: goto L_1658;
        case 0x1888: goto L_1888;
        case 0x1490: goto L_1490;
        case 0x1460: goto L_1460;
        case 0x1720: goto L_1720;
        case 0x112C: goto L_112C;
        case 0x1860: goto L_1860;
        case 0x1070: goto L_1070;
        case 0x1950: goto L_1950;
        case 0x1160: goto L_1160;
        case 0x14A0: goto L_14A0;
        case 0x14C8: goto L_14C8;
        case 0x14F0: goto L_14F0;
        case 0x1518: goto L_1518;
        case 0x1198: goto L_1198;
        case 0x1590: goto L_1590;
        case 0x15B8: goto L_15B8;
        case 0x1804: goto L_1804;
        case 0x1210: goto L_1210;
        case 0x1608: goto L_1608;
        case 0x1630: goto L_1630;
    }
    printf("Unhandled jump target 0x%04X in microcode f3dex, coming from [%s:%d]\n", jump_target, debug_file, debug_line);
    printf("Register dump: r0  = %08X r1  = %08X r2  = %08X r3  = %08X r4  = %08X r5  = %08X r6  = %08X r7  = %08X\n"
           "               r8  = %08X r9  = %08X r10 = %08X r11 = %08X r12 = %08X r13 = %08X r14 = %08X r15 = %08X\n"
           "               r16 = %08X r17 = %08X r18 = %08X r19 = %08X r20 = %08X r21 = %08X r22 = %08X r23 = %08X\n"
           "               r24 = %08X r25 = %08X r26 = %08X r27 = %08X r28 = %08X r29 = %08X r30 = %08X r31 = %08X\n",
           0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16,
           r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31);
    return RspExitReason::UnhandledJumpTarget;
}
