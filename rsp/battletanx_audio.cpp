#include "librecomp/rsp.hpp"
#include "librecomp/rsp_vu_impl.hpp"
#include <map>
#include <vector>

using RspUcodePermutationFunc = RspExitReason(uint8_t* rdram, RspContext* ctx);

RspExitReason battletanx_audio_initial(uint8_t* rdram, RspContext* ctx);
RspExitReason battletanx_audio0(uint8_t* rdram, RspContext* ctx);

static const std::map<uint32_t, uint32_t> imemToSlot = {
    { 0x1080, 0 },
};

static const std::vector<std::map<uint32_t, uint32_t>> offsetToOverlay = {
    {
        { 0x0000, 0 },
    },
};

static RspUcodePermutationFunc* permutations[] = {
    battletanx_audio0,
};

RspExitReason battletanx_audio(uint8_t* rdram, uint32_t ucode_addr) {
    RspContext ctx{};
    uint32_t slots[] = {0};

    RspExitReason exitReason = battletanx_audio_initial(rdram, &ctx);

    while (exitReason == RspExitReason::SwapOverlay) {
        uint32_t slot = imemToSlot.at(ctx.dma_mem_address);
        uint32_t overlay = offsetToOverlay.at(slot).at(ctx.dma_dram_address - ucode_addr);
        slots[slot] = overlay;

        RspUcodePermutationFunc* permutationFunc = permutations[slots[0] * 1];
        exitReason = permutationFunc(rdram, &ctx);
    }

    return exitReason;
}

RspExitReason battletanx_audio_initial(uint8_t* rdram, RspContext* ctx) {
    uint32_t                 r1 = ctx->r1,   r2 = ctx->r2,   r3 = ctx->r3,   r4 = ctx->r4,   r5 = ctx->r5,   r6 = ctx->r6,   r7 = ctx->r7;
    uint32_t  r8 = ctx->r8,  r9 = ctx->r9,   r10 = ctx->r10, r11 = ctx->r11, r12 = ctx->r12, r13 = ctx->r13, r14 = ctx->r14, r15 = ctx->r15;
    uint32_t r16 = ctx->r16, r17 = ctx->r17, r18 = ctx->r18, r19 = ctx->r19, r20 = ctx->r20, r21 = ctx->r21, r22 = ctx->r22, r23 = ctx->r23;
    uint32_t r24 = ctx->r24, r25 = ctx->r25, r26 = ctx->r26, r27 = ctx->r27, r28 = ctx->r28, r29 = ctx->r29, r30 = ctx->r30, r31 = ctx->r31;
    uint32_t dma_mem_address = ctx->dma_mem_address, dma_dram_address = ctx->dma_dram_address, jump_target = ctx->jump_target;
    const char * debug_file = NULL; int debug_line = 0;
    RSP rsp = ctx->rsp;
    r1 = 0xFC0;
L_1000:
    // j           L_1064
    // addi        $1, $zero, 0xFC0
    r1 = RSP_ADD32(0, 0XFC0);
    goto L_1064;
    // addi        $1, $zero, 0xFC0
    r1 = RSP_ADD32(0, 0XFC0);
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
    if (dma_mem_address & 0x1000) {
        ctx->resume_address = 0x101C;
        ctx->resume_delay = false;
        goto do_overlay_swap;
    }
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
L_1058:
    // mtc0        $8, SP_STATUS
    set_sp_status(r8);
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
    // nop

    // jal         0x103C
    r31 = 0x107C;
    // nop

    goto L_103C;
    // nop

L_107C:
    // mfc0        $2, DPC_STATUS
    r2 = 0;
L_1080:
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
L_109C:
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
L_10AC:
    // mtc0        $3, SP_RD_LEN
    if (dma_mem_address & 0x1000) {
        ctx->resume_address = 0x10AC;
        ctx->resume_delay = false;
        goto do_overlay_swap;
    }
    DO_DMA_READ(r3);
L_10B0:
    // mfc0        $4, SP_DMA_BUSY
    r4 = 0;
L_10B4:
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
    // nop

    goto L_1008;
L_10C8:
    // nop

    // nop

    // j           L_1738
    // addi        $29, $zero, 0x110
    r29 = RSP_ADD32(0, 0X110);
    goto L_1738;
    // addi        $29, $zero, 0x110
    r29 = RSP_ADD32(0, 0X110);
    // jal         0x1FD4
    r31 = 0x10E0;
    // add         $20, $zero, $22
    r20 = RSP_ADD32(0, r22);
    goto L_1FD4;
    // add         $20, $zero, $22
    r20 = RSP_ADD32(0, r22);
L_10E0:
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
L_10EC:
    // mfc0        $2, SP_STATUS
    r2 = get_sp_status();
L_10F0:
    // andi        $2, $2, 0x80
    r2 = r2 & 0X80;
    // bne         $2, $zero, L_1110
    if (r2 != 0) {
        // lh          $21, 0x26($zero)
        r21 = RSP_MEM_H_LOAD(0X26, 0);
        goto L_1110;
    }
    // lh          $21, 0x26($zero)
    r21 = RSP_MEM_H_LOAD(0X26, 0);
    // bne         $28, $27, L_10B4
    if (r28 != r27) {
        // lw          $25, 0x0($27)
        r25 = RSP_MEM_W_LOAD(0X0, r27);
        goto L_10B4;
    }
    // lw          $25, 0x0($27)
    r25 = RSP_MEM_W_LOAD(0X0, r27);
    // j           L_10C8
    // lh          $ra, 0x104($zero)
    r31 = RSP_MEM_H_LOAD(0X104, 0);
    goto L_10C8;
    // lh          $ra, 0x104($zero)
    r31 = RSP_MEM_H_LOAD(0X104, 0);
L_110C:
    // lh          $21, 0xB6($zero)
    r21 = RSP_MEM_H_LOAD(0XB6, 0);
L_1110:
    // j           L_10F0
    // ori         $30, $zero, 0x20
    r30 = 0 | 0X20;
    goto L_10F0;
    // ori         $30, $zero, 0x20
    r30 = 0 | 0X20;
L_1118:
    // addi        $28, $zero, 0xAE0
    r28 = RSP_ADD32(0, 0XAE0);
    // add         $21, $zero, $ra
    r21 = RSP_ADD32(0, r31);
    // addi        $20, $zero, 0x9A0
    r20 = RSP_ADD32(0, 0X9A0);
    // add         $19, $zero, $26
    r19 = RSP_ADD32(0, r26);
L_1128:
    // addi        $26, $26, 0x140
    r26 = RSP_ADD32(r26, 0X140);
    // jal         0x1FD4
    r31 = 0x1134;
    // addi        $18, $zero, 0x13F
    r18 = RSP_ADD32(0, 0X13F);
    goto L_1FD4;
    // addi        $18, $zero, 0x13F
    r18 = RSP_ADD32(0, 0X13F);
L_1134:
    // jr          $21
    jump_target = r21;
    debug_file = __FILE__; debug_line = __LINE__;
    // addi        $27, $zero, 0x9A0
    r27 = RSP_ADD32(0, 0X9A0);
    goto do_indirect_jump;
    // addi        $27, $zero, 0x9A0
    r27 = RSP_ADD32(0, 0X9A0);
    // add         $21, $zero, $ra
    r21 = RSP_ADD32(0, r31);
L_1140:
    // lw          $19, 0x0($30)
    r19 = RSP_MEM_W_LOAD(0X0, r30);
    // lh          $18, 0x4($30)
    r18 = RSP_MEM_H_LOAD(0X4, r30);
    // jal         0x1FD4
    r31 = 0x1150;
    // lh          $20, 0x6($30)
    r20 = RSP_MEM_H_LOAD(0X6, r30);
    goto L_1FD4;
    // lh          $20, 0x6($30)
    r20 = RSP_MEM_H_LOAD(0X6, r30);
L_1150:
    // jal         0x1FC4
    r31 = 0x1158;
    // nop

    goto L_1FC4;
    // nop

L_1158:
    // jr          $21
    jump_target = r21;
    debug_file = __FILE__; debug_line = __LINE__;
    // srl         $12, $19, 22
    r12 = S32(U32(r19) >> 22);
    goto do_indirect_jump;
L_115C:
    // srl         $12, $19, 22
    r12 = S32(U32(r19) >> 22);
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
    // add         $21, $zero, $ra
    r21 = RSP_ADD32(0, r31);
    // lw          $19, 0x18($29)
    r19 = RSP_MEM_W_LOAD(0X18, r29);
    // addi        $18, $23, -0xCE0
    r18 = RSP_ADD32(r23, -0XCE0);
    // lw          $23, 0x44($29)
    r23 = RSP_MEM_W_LOAD(0X44, r29);
    // blez        $18, L_11F4
    if (RSP_SIGNED(r18) <= 0) {
        // add         $20, $19, $18
        r20 = RSP_ADD32(r19, r18);
        goto L_11F4;
    }
    // add         $20, $19, $18
    r20 = RSP_ADD32(r19, r18);
    // sub         $20, $23, $20
    r20 = RSP_SUB32(r23, r20);
    // bgez        $20, L_11B8
    if (RSP_SIGNED(r20) >= 0) {
        // mfc0        $20, DPC_STATUS
        r20 = 0;
        goto L_11B8;
    }
L_1198:
    // mfc0        $20, DPC_STATUS
    r20 = 0;
    // andi        $20, $20, 0x400
    r20 = r20 & 0X400;
    // bne         $20, $zero, L_1198
    if (r20 != 0) {
        // mfc0        $23, DPC_CURRENT
        r23 = 0;
        goto L_1198;
    }
L_11A4:
    // mfc0        $23, DPC_CURRENT
    r23 = 0;
    // lw          $19, 0x40($29)
    r19 = RSP_MEM_W_LOAD(0X40, r29);
    // beq         $23, $19, L_11A4
    if (r23 == r19) {
        // nop
    
        goto L_11A4;
    }
    // nop

    // mtc0        $19, DPC_START
L_11B8:
    // mfc0        $23, DPC_CURRENT
    r23 = 0;
    // sub         $20, $19, $23
    r20 = RSP_SUB32(r19, r23);
L_11C0:
    // bgez        $20, L_11D4
    if (RSP_SIGNED(r20) >= 0) {
        // add         $20, $19, $18
        r20 = RSP_ADD32(r19, r18);
        goto L_11D4;
    }
    // add         $20, $19, $18
    r20 = RSP_ADD32(r19, r18);
    // sub         $20, $20, $23
    r20 = RSP_SUB32(r20, r23);
    // bgez        $20, L_11B8
    if (RSP_SIGNED(r20) >= 0) {
        // nop
    
        goto L_11B8;
    }
L_11D0:
    // nop

L_11D4:
    // add         $23, $19, $18
    r23 = RSP_ADD32(r19, r18);
    // addi        $18, $18, -0x1
    r18 = RSP_ADD32(r18, -0X1);
L_11DC:
    // addi        $20, $zero, 0xCE0
    r20 = RSP_ADD32(0, 0XCE0);
    // jal         0x1FD8
    r31 = 0x11E8;
    // addi        $17, $zero, 0x1
    r17 = RSP_ADD32(0, 0X1);
    goto L_1FD8;
    // addi        $17, $zero, 0x1
    r17 = RSP_ADD32(0, 0X1);
L_11E8:
    // jal         0x1FC4
    r31 = 0x11F0;
    // sw          $23, 0x18($29)
    RSP_MEM_W_STORE(0X18, r29, r23);
    goto L_1FC4;
    // sw          $23, 0x18($29)
    RSP_MEM_W_STORE(0X18, r29, r23);
L_11F0:
    // mtc0        $23, DPC_END
L_11F4:
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
    // lh          $ra, 0x396($zero)
    r31 = RSP_MEM_H_LOAD(0X396, 0);
L_1214:
    // lbu         $1, -0x3($27)
    r1 = RSP_MEM_BU(-0X3, r27);
    // lbu         $2, -0x2($27)
    r2 = RSP_MEM_BU(-0X2, r27);
    // lbu         $3, -0x1($27)
    r3 = RSP_MEM_BU(-0X1, r27);
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
    r31 = 0x123C;
    // addi        $24, $ra, 0x0
    r24 = RSP_ADD32(r31, 0X0);
    goto L_11C0;
    // addi        $24, $ra, 0x0
    r24 = RSP_ADD32(r31, 0X0);
L_123C:
    // lbu         $1, -0x7($27)
    r1 = RSP_MEM_BU(-0X7, r27);
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
L_1254:
    // sbv         $v31[6], 0x1C($29)
    rsp.SBV<6>(rsp.vpu.r[31], r29, 0X1C);
    // lw          $19, 0x24($29)
    r19 = RSP_MEM_W_LOAD(0X24, r29);
    // lw          $3, 0xFE0($zero)
    r3 = RSP_MEM_W_LOAD(0XFE0, 0);
    // addi        $20, $zero, 0x3E0
    r20 = RSP_ADD32(0, 0X3E0);
    // sub         $3, $3, $19
    r3 = RSP_SUB32(r3, r19);
    // bgez        $3, L_10EC
    if (RSP_SIGNED(r3) >= 0) {
        // addi        $19, $19, -0x40
        r19 = RSP_ADD32(r19, -0X40);
        goto L_10EC;
    }
    // addi        $19, $19, -0x40
    r19 = RSP_ADD32(r19, -0X40);
    // jal         0x1FD4
    r31 = 0x1278;
    // addi        $18, $zero, 0x3F
    r18 = RSP_ADD32(0, 0X3F);
    goto L_1FD4;
    // addi        $18, $zero, 0x3F
    r18 = RSP_ADD32(0, 0X3F);
L_1278:
    // jal         0x1FC4
    r31 = 0x1280;
    // addi        $3, $zero, 0x460
    r3 = RSP_ADD32(0, 0X460);
    goto L_1FC4;
    // addi        $3, $zero, 0x460
    r3 = RSP_ADD32(0, 0X460);
L_1280:
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
L_1294:
    // add         $5, $5, $2
    r5 = RSP_ADD32(r5, r2);
    // j           L_109C
    // sw          $24, 0x0($5)
    RSP_MEM_W_STORE(0X0, r5, r24);
    goto L_109C;
    // sw          $24, 0x0($5)
    RSP_MEM_W_STORE(0X0, r5, r24);
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
L_12D0:
    // lw          $3, 0x0($7)
    r3 = RSP_MEM_W_LOAD(0X0, r7);
    // lbu         $5, -0x5($27)
    r5 = RSP_MEM_BU(-0X5, r27);
    // lbu         $6, -0x6($27)
    r6 = RSP_MEM_BU(-0X6, r27);
    // addi        $2, $zero, 0x1
    r2 = RSP_ADD32(0, 0X1);
    // sllv        $2, $2, $5
    r2 = S32(r2) << (r5 & 31);
    // addi        $2, $2, -0x1
    r2 = RSP_ADD32(r2, -0X1);
    // sllv        $2, $2, $6
    r2 = S32(r2) << (r6 & 31);
L_12EC:
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
L_1314:
    // lh          $3, 0x24($25)
    r3 = RSP_MEM_H_LOAD(0X24, r25);
    // and         $2, $2, $3
    r2 = r2 & r3;
    // bne         $25, $24, L_1314
    if (r25 != r24) {
        // addi        $25, $25, 0x28
        r25 = RSP_ADD32(r25, 0X28);
        goto L_1314;
    }
    // addi        $25, $25, 0x28
    r25 = RSP_ADD32(r25, 0X28);
    // beq         $2, $zero, L_10EC
    if (r2 == 0) {
        // lb          $2, 0x38($29)
        r2 = RSP_MEM_B(0X38, r29);
        goto L_10EC;
    }
L_1328:
    // lb          $2, 0x38($29)
    r2 = RSP_MEM_B(0X38, r29);
    // addi        $2, $2, -0x4
    r2 = RSP_ADD32(r2, -0X4);
    // bltz        $2, L_110C
    if (RSP_SIGNED(r2) < 0) {
        // addi        $3, $2, 0x398
        r3 = RSP_ADD32(r2, 0X398);
        goto L_110C;
    }
    // addi        $3, $2, 0x398
    r3 = RSP_ADD32(r2, 0X398);
L_1338:
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
    // bgtz        $25, L_10EC
    if (RSP_SIGNED(r25) > 0) {
        // lw          $24, -0x4($29)
        r24 = RSP_MEM_W_LOAD(-0X4, r29);
        goto L_10EC;
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
L_1374:
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
L_1388:
    // lhu         $3, 0x31E($25)
    r3 = RSP_MEM_HU_LOAD(0X31E, r25);
L_138C:
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
    // bltz        $2, L_13C4
    if (RSP_SIGNED(r2) < 0) {
        // addi        $2, $2, 0x18
        r2 = RSP_ADD32(r2, 0X18);
        goto L_13C4;
    }
    // addi        $2, $2, 0x18
    r2 = RSP_ADD32(r2, 0X18);
    // jal         0x110C
    r31 = 0x13C0;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
    goto L_110C;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
L_13C0:
    // add         $24, $19, $zero
    r24 = RSP_ADD32(r19, 0);
L_13C4:
    // sw          $25, 0x0($23)
    RSP_MEM_W_STORE(0X0, r23, r25);
    // sw          $24, 0x4($23)
    RSP_MEM_W_STORE(0X4, r23, r24);
    // jal         0x1128
    r31 = 0x13D4;
    // addi        $23, $23, 0x8
    r23 = RSP_ADD32(r23, 0X8);
    goto L_1128;
    // addi        $23, $23, 0x8
    r23 = RSP_ADD32(r23, 0X8);
L_13D4:
    // bgtz        $2, L_10EC
    if (RSP_SIGNED(r2) > 0) {
        // nop
    
        goto L_10EC;
    }
    // nop

    // j           L_10AC
    // andi        $2, $2, 0x1FE
    r2 = r2 & 0X1FE;
    goto L_10AC;
    // andi        $2, $2, 0x1FE
    r2 = r2 & 0X1FE;
    // lh          $2, 0xC0($2)
    r2 = RSP_MEM_H_LOAD(0XC0, r2);
    // jal         0x1FC4
    r31 = 0x13F0;
    // lbu         $1, -0x7($27)
    r1 = RSP_MEM_BU(-0X7, r27);
    goto L_1FC4;
    // lbu         $1, -0x7($27)
    r1 = RSP_MEM_BU(-0X7, r27);
L_13F0:
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
L_140C:
    // srl         $9, $9, 2
    r9 = S32(U32(r9) >> 2);
    // jal         0x1660
    r31 = 0x1418;
    // lhu         $7, 0x31E($1)
    r7 = RSP_MEM_HU_LOAD(0X31E, r1);
    goto L_1660;
    // lhu         $7, 0x31E($1)
    r7 = RSP_MEM_HU_LOAD(0X31E, r1);
L_1418:
    // llv         $v17[0], 0x14($29)
    rsp.LLV<0>(rsp.vpu.r[17], r29, 0X5);
    // jal         0x1688
    r31 = 0x1424;
    // llv         $v17[8], 0x14($29)
    rsp.LLV<8>(rsp.vpu.r[17], r29, 0X5);
    goto L_1688;
    // llv         $v17[8], 0x14($29)
    rsp.LLV<8>(rsp.vpu.r[17], r29, 0X5);
L_1424:
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
L_144C:
    // vmadn       $v28, $v15, $v31[1]
    rsp.VMADN<9>(rsp.vpu.r[28], rsp.vpu.r[15], rsp.vpu.r[31]);
    // llv         $v18[8], 0x18($22)
    rsp.LLV<8>(rsp.vpu.r[18], r22, 0X6);
    // vmadh       $v29, $v11, $v31[1]
    rsp.VMADH<9>(rsp.vpu.r[29], rsp.vpu.r[11], rsp.vpu.r[31]);
    // bne         $1, $zero, L_177C
    if (r1 != 0) {
        // addi        $22, $22, 0x20
        r22 = RSP_ADD32(r22, 0X20);
        goto L_177C;
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
L_1470:
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
L_148C:
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
    // vmudl       $v28, $v28, $v19[0]
    rsp.VMUDL<8>(rsp.vpu.r[28], rsp.vpu.r[28], rsp.vpu.r[19]);
    // or          $8, $8, $14
    r8 = r8 | r14;
    // vmadm       $v29, $v29, $v19[0]
    rsp.VMADM<8>(rsp.vpu.r[29], rsp.vpu.r[29], rsp.vpu.r[19]);
    // or          $8, $8, $13
    r8 = r8 | r13;
    // vmadn       $v28, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[28], rsp.vpu.r[31], rsp.vpu.r[31]);
    // jal         0x1000
    r31 = 0x14CC;
    // sh          $8, 0x24($7)
    RSP_MEM_H_STORE(0X24, r7, r8);
    goto L_1000;
    // sh          $8, 0x24($7)
    RSP_MEM_H_STORE(0X24, r7, r8);
L_14CC:
    // vge         $v6, $v27, $v31[0]
    rsp.VGE<8>(rsp.vpu.r[6], rsp.vpu.r[27], rsp.vpu.r[31]);
    // sdv         $v21[0], 0x0($7)
    rsp.SDV<0>(rsp.vpu.r[21], r7, 0X0);
    // vmrg        $v6, $v27, $v30[0]
    rsp.VMRG<8>(rsp.vpu.r[6], rsp.vpu.r[27], rsp.vpu.r[30]);
    // sdv         $v20[0], 0x8($7)
    rsp.SDV<0>(rsp.vpu.r[20], r7, 0X1);
    // vmudl       $v5, $v20, $v26[3h]
    rsp.VMUDL<7>(rsp.vpu.r[5], rsp.vpu.r[20], rsp.vpu.r[26]);
    // vmadm       $v5, $v21, $v26[3h]
    rsp.VMADM<7>(rsp.vpu.r[5], rsp.vpu.r[21], rsp.vpu.r[26]);
    // vmadn       $v5, $v20, $v6[3h]
    rsp.VMADN<7>(rsp.vpu.r[5], rsp.vpu.r[20], rsp.vpu.r[6]);
    // vmadh       $v4, $v21, $v6[3h]
    rsp.VMADH<7>(rsp.vpu.r[4], rsp.vpu.r[21], rsp.vpu.r[6]);
    // addi        $9, $9, -0x2
    r9 = RSP_ADD32(r9, -0X2);
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
    // ldv         $v29[8], 0x28($zero)
    rsp.LDV<8>(rsp.vpu.r[29], 0, 0X5);
    // vmadn       $v7, $v31, $v31[0]
    rsp.VMADN<8>(rsp.vpu.r[7], rsp.vpu.r[31], rsp.vpu.r[31]);
    // vge         $v6, $v6, $v29[1q]
    rsp.VGE<3>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[29]);
    // sw          $15, 0x10($7)
    RSP_MEM_W_STORE(0X10, r7, r15);
    // beq         $12, $zero, L_1558
    if (r12 == 0) {
        // vlt         $v6, $v6, $v29[0q]
        rsp.VLT<2>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[29]);
        goto L_1558;
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
L_1558:
    // slv         $v18[0], 0x14($7)
    rsp.SLV<0>(rsp.vpu.r[18], r7, 0X5);
    // sdv         $v6[0], 0x18($7)
    rsp.SDV<0>(rsp.vpu.r[6], r7, 0X3);
    // ssv         $v7[4], 0x1E($7)
    rsp.SSV<4>(rsp.vpu.r[7], r7, 0XF);
    // ssv         $v27[6], 0x20($7)
    rsp.SSV<6>(rsp.vpu.r[27], r7, 0X10);
    // bltz        $9, L_159C
    if (RSP_SIGNED(r9) < 0) {
        // ssv         $v26[6], 0x22($7)
        rsp.SSV<6>(rsp.vpu.r[26], r7, 0X11);
        goto L_159C;
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
    // sw          $8, 0x4C($7)
    RSP_MEM_W_STORE(0X4C, r7, r8);
    // addi        $7, $7, 0x50
    r7 = RSP_ADD32(r7, 0X50);
L_1598:
    // bgtz        $9, L_1424
    if (RSP_SIGNED(r9) > 0) {
        // lh          $8, 0x106($zero)
        r8 = RSP_MEM_H_LOAD(0X106, 0);
        goto L_1424;
    }
L_159C:
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
    // bne         $8, $zero, L_161C
    if (r8 != 0) {
        // andi        $7, $1, 0x2
        r7 = r1 & 0X2;
        goto L_161C;
    }
L_15B0:
    // andi        $7, $1, 0x2
    r7 = r1 & 0X2;
    // addi        $20, $zero, 0x3E0
    r20 = RSP_ADD32(0, 0X3E0);
    // andi        $8, $1, 0x4
    r8 = r1 & 0X4;
L_15BC:
    // beq         $8, $zero, L_15E8
    if (r8 == 0) {
        // lqv         $v26[0], 0x30($22)
        rsp.LQV<0>(rsp.vpu.r[26], r22, 0X3);
        goto L_15E8;
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
    // beq         $19, $8, L_15E8
    if (r19 == r8) {
        // addi        $12, $zero, 0x3F
        r12 = RSP_ADD32(0, 0X3F);
        goto L_15E8;
    }
    // addi        $12, $zero, 0x3F
    r12 = RSP_ADD32(0, 0X3F);
    // jal         0x1FD8
    r31 = 0x15E4;
    // sw          $1, 0x24($29)
    RSP_MEM_W_STORE(0X24, r29, r1);
    goto L_1FD8;
    // sw          $1, 0x24($29)
    RSP_MEM_W_STORE(0X24, r29, r1);
L_15E4:
    // jal         0x1FC4
    r31 = 0x15EC;
    // lqv         $v28[0], 0x10($22)
    rsp.LQV<0>(rsp.vpu.r[28], r22, 0X1);
    goto L_1FC4;
L_15E8:
    // lqv         $v28[0], 0x10($22)
    rsp.LQV<0>(rsp.vpu.r[28], r22, 0X1);
L_15EC:
    // beq         $7, $zero, L_1628
    if (r7 == 0) {
        // lqv         $v27[0], 0x20($22)
        rsp.LQV<0>(rsp.vpu.r[27], r22, 0X2);
        goto L_1628;
    }
    // lqv         $v27[0], 0x20($22)
    rsp.LQV<0>(rsp.vpu.r[27], r22, 0X2);
    // sqv         $v26[0], 0x30($20)
    rsp.SQV<0>(rsp.vpu.r[26], r20, 0X3);
    // lqv         $v29[0], 0x0($22)
    rsp.LQV<0>(rsp.vpu.r[29], r22, 0X0);
L_15FC:
    // sqv         $v28[0], 0x10($20)
    rsp.SQV<0>(rsp.vpu.r[28], r20, 0X1);
    // addi        $3, $zero, 0x460
    r3 = RSP_ADD32(0, 0X460);
    // sqv         $v27[0], 0x20($20)
    rsp.SQV<0>(rsp.vpu.r[27], r20, 0X2);
    // sqv         $v29[0], 0x0($20)
    rsp.SQV<0>(rsp.vpu.r[29], r20, 0X0);
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
L_161C:
    // lqv         $v26[0], 0x30($22)
    rsp.LQV<0>(rsp.vpu.r[26], r22, 0X3);
    // j           L_1598
    // addi        $20, $zero, 0x420
    r20 = RSP_ADD32(0, 0X420);
    goto L_1598;
    // addi        $20, $zero, 0x420
    r20 = RSP_ADD32(0, 0X420);
L_1628:
    // addiu       $3, $zero, 0xDE0
    r3 = RSP_ADD32(0, 0XDE0);
    // addu        $1, $zero, $22
    r1 = RSP_ADD32(0, r22);
    // jal         0x15FC
    r31 = 0x1638;
    // addu        $2, $zero, $20
    r2 = RSP_ADD32(0, r20);
    goto L_15FC;
    // addu        $2, $zero, $20
    r2 = RSP_ADD32(0, r20);
L_1638:
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
    // addi        $19, $3, 0x10
    r19 = RSP_ADD32(r3, 0X10);
L_1650:
    // vmudh       $v5, $v31, $v31[0]
    rsp.VMUDH<8>(rsp.vpu.r[5], rsp.vpu.r[31], rsp.vpu.r[31]);
    // addi        $18, $1, 0x8
    r18 = RSP_ADD32(r1, 0X8);
L_1658:
    // ldv         $v3[0], 0x0($2)
    rsp.LDV<0>(rsp.vpu.r[3], r2, 0X0);
    // ldv         $v4[0], 0x20($2)
    rsp.LDV<0>(rsp.vpu.r[4], r2, 0X4);
L_1660:
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
    // vmadn       $v6, $v4, $v1[0h]
    rsp.VMADN<4>(rsp.vpu.r[6], rsp.vpu.r[4], rsp.vpu.r[1]);
    // vmadh       $v5, $v3, $v1[0h]
    rsp.VMADH<4>(rsp.vpu.r[5], rsp.vpu.r[3], rsp.vpu.r[1]);
L_1688:
    // bne         $1, $18, L_1658
    if (r1 != r18) {
        // vmadn       $v6, $v31, $v31[0]
        rsp.VMADN<8>(rsp.vpu.r[6], rsp.vpu.r[31], rsp.vpu.r[31]);
        goto L_1658;
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
    // bne         $3, $19, L_1650
    if (r3 != r19) {
        // addi        $3, $3, 0x10
        r3 = RSP_ADD32(r3, 0X10);
        goto L_1650;
    }
    // addi        $3, $3, 0x10
    r3 = RSP_ADD32(r3, 0X10);
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // nop

    goto do_indirect_jump;
    // nop

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
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // vmudh       $v0, $v0, $v3
    rsp.VMUDH<0>(rsp.vpu.r[0], rsp.vpu.r[0], rsp.vpu.r[3]);
    goto do_indirect_jump;
    // vmudh       $v0, $v0, $v3
    rsp.VMUDH<0>(rsp.vpu.r[0], rsp.vpu.r[0], rsp.vpu.r[3]);
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
    // ldv         $v12[0], 0x20($8)
    rsp.LDV<0>(rsp.vpu.r[12], r8, 0X4);
    // ldv         $v13[0], 0x28($8)
    rsp.LDV<0>(rsp.vpu.r[13], r8, 0X5);
    // ldv         $v14[0], 0x30($8)
    rsp.LDV<0>(rsp.vpu.r[14], r8, 0X6);
    // ldv         $v8[8], 0x0($8)
    rsp.LDV<8>(rsp.vpu.r[8], r8, 0X0);
L_1708:
    // ldv         $v9[8], 0x8($8)
    rsp.LDV<8>(rsp.vpu.r[9], r8, 0X1);
L_170C:
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
    // bgtz        $1, L_1758
    if (RSP_SIGNED(r1) > 0) {
        // lb          $2, 0x38($29)
        r2 = RSP_MEM_B(0X38, r29);
        goto L_1758;
    }
    // lb          $2, 0x38($29)
    r2 = RSP_MEM_B(0X38, r29);
L_1738:
    // addi        $4, $2, -0x44
    r4 = RSP_ADD32(r2, -0X44);
    // bgtz        $4, L_10EC
    if (RSP_SIGNED(r4) > 0) {
        // addi        $3, $2, 0x398
        r3 = RSP_ADD32(r2, 0X398);
        goto L_10EC;
    }
    // addi        $3, $2, 0x398
    r3 = RSP_ADD32(r2, 0X398);
    // addi        $2, $2, 0x4
    r2 = RSP_ADD32(r2, 0X4);
    // add         $26, $26, $27
    r26 = RSP_ADD32(r26, r27);
    // sub         $26, $26, $28
    r26 = RSP_SUB32(r26, r28);
    // sw          $26, 0x0($3)
    RSP_MEM_W_STORE(0X0, r3, r26);
    // sb          $2, 0x38($29)
    RSP_MEM_B(0X38, r29) = r2;
L_1758:
    // jal         0x110C
    r31 = 0x1760;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
    goto L_110C;
    // add         $19, $24, $zero
    r19 = RSP_ADD32(r24, 0);
L_1760:
    // add         $26, $19, $zero
    r26 = RSP_ADD32(r19, 0);
    // j           L_109C
    // addi        $28, $27, 0x0
    r28 = RSP_ADD32(r27, 0X0);
    goto L_109C;
    // addi        $28, $27, 0x0
    r28 = RSP_ADD32(r27, 0X0);
    // nop

L_1770:
    // ori         $30, $zero, 0x10
    r30 = 0 | 0X10;
    // b           L_1140
    // lh          $21, 0x100($zero)
    r21 = RSP_MEM_H_LOAD(0X100, 0);
    goto L_1140;
    // lh          $21, 0x100($zero)
    r21 = RSP_MEM_H_LOAD(0X100, 0);
L_177C:
    // ori         $30, $zero, 0x18
    r30 = 0 | 0X18;
    // b           L_1140
    // lh          $21, 0xA0($zero)
    r21 = RSP_MEM_H_LOAD(0XA0, 0);
    goto L_1140;
    // lh          $21, 0xA0($zero)
    r21 = RSP_MEM_H_LOAD(0XA0, 0);
    // ori         $2, $zero, 0x2800
    r2 = 0 | 0X2800;
    // mtc0        $2, SP_STATUS
    set_sp_status(r2);
    // lqv         $v31[0], 0x30($zero)
    rsp.LQV<0>(rsp.vpu.r[31], 0, 0X3);
    // lqv         $v30[0], 0x40($zero)
    rsp.LQV<0>(rsp.vpu.r[30], 0, 0X4);
    // lw          $4, 0xFC4($zero)
    r4 = RSP_MEM_W_LOAD(0XFC4, 0);
L_179C:
    // andi        $4, $4, 0x1
    r4 = r4 & 0X1;
    // bne         $4, $zero, L_187C
    if (r4 != 0) {
        // sw          $zero, 0xFC4($zero)
        RSP_MEM_W_STORE(0XFC4, 0, 0);
        goto L_187C;
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
L_17B8:
    // mfc0        $4, DPC_STATUS
    r4 = 0;
    // andi        $4, $4, 0x1
    r4 = r4 & 0X1;
    // bne         $4, $zero, L_17EC
    if (r4 != 0) {
        // mfc0        $4, DPC_END
        r4 = 0;
        goto L_17EC;
    }
    // mfc0        $4, DPC_END
    r4 = 0;
    // sub         $23, $23, $4
    r23 = RSP_SUB32(r23, r4);
    // bgtz        $23, L_17EC
    if (RSP_SIGNED(r23) > 0) {
        // mfc0        $5, DPC_CURRENT
        r5 = 0;
        goto L_17EC;
    }
    // mfc0        $5, DPC_CURRENT
    r5 = 0;
    // beq         $5, $zero, L_17EC
    if (r5 == 0) {
        // nop
    
        goto L_17EC;
    }
    // nop

    // beq         $5, $4, L_17EC
    if (r5 == r4) {
        // nop
    
        goto L_17EC;
    }
    // nop

    // j           L_17B8
    // ori         $3, $4, 0x0
    r3 = r4 | 0X0;
    goto L_17B8;
    // ori         $3, $4, 0x0
    r3 = r4 | 0X0;
L_17EC:
    // mfc0        $4, DPC_STATUS
    r4 = 0;
    // andi        $4, $4, 0x400
    r4 = r4 & 0X400;
    // bne         $4, $zero, L_17EC
    if (r4 != 0) {
        // addi        $4, $zero, 0x1
        r4 = RSP_ADD32(0, 0X1);
        goto L_17EC;
    }
    // addi        $4, $zero, 0x1
    r4 = RSP_ADD32(0, 0X1);
    // mtc0        $4, DPC_STATUS
    // mtc0        $3, DPC_START
    // mtc0        $3, DPC_END
    // sw          $3, 0x18($29)
    RSP_MEM_W_STORE(0X18, r29, r3);
    // addi        $23, $zero, 0xCE0
    r23 = RSP_ADD32(0, 0XCE0);
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
    // sw          $3, 0x10($zero)
    RSP_MEM_W_STORE(0X10, 0, r3);
    // sw          $4, 0x18($zero)
    RSP_MEM_W_STORE(0X18, 0, r4);
    // sw          $6, 0x20($zero)
    RSP_MEM_W_STORE(0X20, 0, r6);
    // jal         0x10EC
    r31 = 0x184C;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
    goto L_10EC;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
L_184C:
    // jal         0x10C8
    r31 = 0x1854;
    // lw          $26, 0x30($1)
    r26 = RSP_MEM_W_LOAD(0X30, r1);
    goto L_10C8;
    // lw          $26, 0x30($1)
    r26 = RSP_MEM_W_LOAD(0X30, r1);
L_1854:
    // lw          $2, 0x20($1)
    r2 = RSP_MEM_W_LOAD(0X20, r1);
    // lw          $3, 0x24($1)
    r3 = RSP_MEM_W_LOAD(0X24, r1);
    // sw          $2, 0x20($29)
    RSP_MEM_W_STORE(0X20, r29, r2);
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

L_187C:
    // jal         0x10EC
    r31 = 0x1884;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
    goto L_10EC;
    // addi        $30, $zero, 0x8
    r30 = RSP_ADD32(0, 0X8);
L_1884:
    // lw          $23, 0xBF0($zero)
    r23 = RSP_MEM_W_LOAD(0XBF0, 0);
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
    // or          $11, $11, $8
    r11 = r11 | r8;
    // and         $12, $12, $9
    r12 = r12 & r9;
    // andi        $12, $12, 0x7070
    r12 = r12 & 0X7070;
    // bne         $12, $zero, L_1F6C
    if (r12 != 0) {
        // or          $11, $11, $9
        r11 = r11 | r9;
        goto L_1F6C;
    }
    // or          $11, $11, $9
    r11 = r11 | r9;
    // andi        $11, $11, 0x4343
    r11 = r11 & 0X4343;
    // bne         $11, $zero, L_1770
    if (r11 != 0) {
        // llv         $v13[0], 0x18($1)
        rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
        goto L_1770;
    }
    // llv         $v13[0], 0x18($1)
    rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
    // llv         $v14[0], 0x18($2)
    rsp.LLV<0>(rsp.vpu.r[14], r2, 0X6);
    // llv         $v15[0], 0x18($3)
    rsp.LLV<0>(rsp.vpu.r[15], r3, 0X6);
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
    // bltz        $15, L_1F68
    if (RSP_SIGNED(r15) < 0) {
        // sh          $3, 0xBE4($zero)
        RSP_MEM_H_STORE(0XBE4, 0, r3);
        goto L_1F68;
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
L_1A4C:
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
    // blez        $12, L_1A80
    if (RSP_SIGNED(r12) <= 0) {
        // llv         $v13[0], 0x18($1)
        rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
        goto L_1A80;
    }
    // llv         $v13[0], 0x18($1)
    rsp.LLV<0>(rsp.vpu.r[13], r1, 0X6);
    // vsubc       $v28, $v31, $v28
    rsp.VSUBC<0>(rsp.vpu.r[28], rsp.vpu.r[31], rsp.vpu.r[28]);
    // vsub        $v29, $v31, $v29
    rsp.VSUB<0>(rsp.vpu.r[29], rsp.vpu.r[31], rsp.vpu.r[29]);
L_1A80:
    // beq         $17, $zero, L_1F6C
    if (r17 == 0) {
        // vsub        $v4, $v15, $v14
        rsp.VSUB<0>(rsp.vpu.r[4], rsp.vpu.r[15], rsp.vpu.r[14]);
        goto L_1F6C;
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
    // bgez        $14, L_1F68
    if (RSP_SIGNED(r14) >= 0) {
        // vmov        $v28[3], $v28[0]
        rsp.VMOV<8>(rsp.vpu.r[28], 3, rsp.vpu.r[28]);
        goto L_1F68;
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
    r31 = 0x1AD0;
    // vmudn       $v2, $v13, $v31[4]
    rsp.VMUDN<12>(rsp.vpu.r[2], rsp.vpu.r[13], rsp.vpu.r[31]);
    goto L_1000;
    // vmudn       $v2, $v13, $v31[4]
    rsp.VMUDN<12>(rsp.vpu.r[2], rsp.vpu.r[13], rsp.vpu.r[31]);
L_1AD0:
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
L_1B38:
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
    // blez        $7, L_1CB0
    if (RSP_SIGNED(r7) <= 0) {
        // vmudl       $v20, $v20, $v30[5]
        rsp.VMUDL<13>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[30]);
        goto L_1CB0;
    }
    // vmudl       $v20, $v20, $v30[5]
    rsp.VMUDL<13>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[30]);
    // lw          $14, 0x0($15)
    r14 = RSP_MEM_W_LOAD(0X0, r15);
    // vmadm       $v19, $v19, $v30[5]
    rsp.VMADM<13>(rsp.vpu.r[19], rsp.vpu.r[19], rsp.vpu.r[30]);
L_1BF8:
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
L_1CB0:
    // slv         $v17[0], 0x40($8)
    rsp.SLV<0>(rsp.vpu.r[17], r8, 0X10);
    // slv         $v18[0], 0x48($8)
    rsp.SLV<0>(rsp.vpu.r[18], r8, 0X12);
    // andi        $7, $5, 0x7
    r7 = r5 & 0X7;
    // blez        $7, L_1F60
    if (RSP_SIGNED(r7) <= 0) {
        // vxor        $v18, $v31, $v31
        rsp.VXOR<0>(rsp.vpu.r[18], rsp.vpu.r[31], rsp.vpu.r[31]);
        goto L_1F60;
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
    // bgtz        $7, L_1CF0
    if (RSP_SIGNED(r7) > 0) {
        // luv         $v23[0], 0x10($2)
        rsp.LUV<0>(rsp.vpu.r[23], r2, 0X2);
        goto L_1CF0;
    }
    // luv         $v23[0], 0x10($2)
    rsp.LUV<0>(rsp.vpu.r[23], r2, 0X2);
    // luv         $v25[0], 0x10($4)
    rsp.LUV<0>(rsp.vpu.r[25], r4, 0X2);
    // luv         $v15[0], 0x10($4)
    rsp.LUV<0>(rsp.vpu.r[15], r4, 0X2);
    // luv         $v23[0], 0x10($4)
    rsp.LUV<0>(rsp.vpu.r[23], r4, 0X2);
L_1CF0:
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
L_1D6C:
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
L_1DB0:
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
    // blez        $7, L_1DF8
    if (RSP_SIGNED(r7) <= 0) {
        // andi        $7, $5, 0x2
        r7 = r5 & 0X2;
        goto L_1DF8;
    }
    // andi        $7, $5, 0x2
    r7 = r5 & 0X2;
    // addi        $23, $23, 0x40
    r23 = RSP_ADD32(r23, 0X40);
    // sdv         $v17[0], 0x3C0($23)
    rsp.SDV<0>(rsp.vpu.r[17], r23, -0X8);
    // sdv         $v18[0], 0x3D0($23)
    rsp.SDV<0>(rsp.vpu.r[18], r23, -0X6);
L_1DF8:
    // blez        $7, L_1EF4
    if (RSP_SIGNED(r7) <= 0) {
        // andi        $7, $5, 0x1
        r7 = r5 & 0X1;
        goto L_1EF4;
    }
    // andi        $7, $5, 0x1
    r7 = r5 & 0X1;
L_1E00:
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
L_1E24:
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
L_1EF4:
    // blez        $7, L_1F60
    if (RSP_SIGNED(r7) <= 0) {
        // vmudn       $v14, $v14, $v30[4]
        rsp.VMUDN<12>(rsp.vpu.r[14], rsp.vpu.r[14], rsp.vpu.r[30]);
        goto L_1F60;
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
L_1F40:
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
L_1F60:
    // jal         0x1128
    r31 = 0x1F68;
    // addi        $ra, $24, 0x0
    r31 = RSP_ADD32(r24, 0X0);
    goto L_1128;
    // addi        $ra, $24, 0x0
    r31 = RSP_ADD32(r24, 0X0);
L_1F68:
    // addi        $ra, $zero, 0x0
    r31 = RSP_ADD32(0, 0X0);
L_1F6C:
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

L_1FC4:
    // nop

    // nop

    // nop

    // nop

L_1FD4:
    // nop

L_1FD8:
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
    r31 = 0x1FFC;
    // addi        $18, $zero, 0xF17
    r18 = RSP_ADD32(0, 0XF17);
    goto L_1FD4;
    // addi        $18, $zero, 0xF17
    r18 = RSP_ADD32(0, 0XF17);
L_1FFC:
    // lw          $19, 0x10C($zero)
    r19 = RSP_MEM_W_LOAD(0X10C, 0);
    return RspExitReason::ImemOverrun;
do_indirect_jump:
    switch ((jump_target | 0x1000) & 0X1FFF) { 
        case 0x1080: goto L_1080;
        case 0x1F40: goto L_1F40;
        case 0x1D6C: goto L_1D6C;
        case 0x1BF8: goto L_1BF8;
        case 0x170C: goto L_170C;
        case 0x148C: goto L_148C;
        case 0x1470: goto L_1470;
        case 0x144C: goto L_144C;
        case 0x1DB0: goto L_1DB0;
        case 0x138C: goto L_138C;
        case 0x1B38: goto L_1B38;
        case 0x1A4C: goto L_1A4C;
        case 0x12EC: goto L_12EC;
        case 0x12D0: goto L_12D0;
        case 0x1294: goto L_1294;
        case 0x179C: goto L_179C;
        case 0x1214: goto L_1214;
        case 0x11DC: goto L_11DC;
        case 0x115C: goto L_115C;
        case 0x1E00: goto L_1E00;
        case 0x1118: goto L_1118;
        case 0x10B4: goto L_10B4;
        case 0x1278: goto L_1278;
        case 0x1034: goto L_1034;
        case 0x13D4: goto L_13D4;
        case 0x11F0: goto L_11F0;
        case 0x1E24: goto L_1E24;
        case 0x1FFC: goto L_1FFC;
        case 0x1158: goto L_1158;
        case 0x1134: goto L_1134;
        case 0x10E0: goto L_10E0;
        case 0x1388: goto L_1388;
        case 0x10C4: goto L_10C4;
        case 0x13F0: goto L_13F0;
        case 0x13C0: goto L_13C0;
        case 0x11E8: goto L_11E8;
        case 0x1254: goto L_1254;
        case 0x107C: goto L_107C;
        case 0x1328: goto L_1328;
        case 0x1150: goto L_1150;
        case 0x123C: goto L_123C;
        case 0x15EC: goto L_15EC;
        case 0x1418: goto L_1418;
        case 0x1338: goto L_1338;
        case 0x1424: goto L_1424;
        case 0x187C: goto L_187C;
        case 0x14CC: goto L_14CC;
        case 0x140C: goto L_140C;
        case 0x15E4: goto L_15E4;
        case 0x1280: goto L_1280;
        case 0x1F68: goto L_1F68;
        case 0x1638: goto L_1638;
        case 0x1760: goto L_1760;
        case 0x1000: goto L_1000;
        case 0x184C: goto L_184C;
        case 0x1854: goto L_1854;
        case 0x1884: goto L_1884;
        case 0x1AD0: goto L_1AD0;
    }
    printf("Unhandled jump target 0x%04X in microcode battletanx_audio, coming from [%s:%d]\n", jump_target, debug_file, debug_line);
    printf("Register dump: r0  = %08X r1  = %08X r2  = %08X r3  = %08X r4  = %08X r5  = %08X r6  = %08X r7  = %08X\n"
           "               r8  = %08X r9  = %08X r10 = %08X r11 = %08X r12 = %08X r13 = %08X r14 = %08X r15 = %08X\n"
           "               r16 = %08X r17 = %08X r18 = %08X r19 = %08X r20 = %08X r21 = %08X r22 = %08X r23 = %08X\n"
           "               r24 = %08X r25 = %08X r26 = %08X r27 = %08X r28 = %08X r29 = %08X r30 = %08X r31 = %08X\n",
           0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16,
           r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31);
    return RspExitReason::UnhandledJumpTarget;
do_overlay_swap:
                    ctx->r1 = r1;   ctx->r2 = r2;   ctx->r3 = r3;   ctx->r4 = r4;   ctx->r5 = r5;   ctx->r6 = r6;   ctx->r7 = r7;
    ctx->r8 = r8;   ctx->r9 = r9;   ctx->r10 = r10; ctx->r11 = r11; ctx->r12 = r12; ctx->r13 = r13; ctx->r14 = r14; ctx->r15 = r15;
    ctx->r16 = r16; ctx->r17 = r17; ctx->r18 = r18; ctx->r19 = r19; ctx->r20 = r20; ctx->r21 = r21; ctx->r22 = r22; ctx->r23 = r23;
    ctx->r24 = r24; ctx->r25 = r25; ctx->r26 = r26; ctx->r27 = r27; ctx->r28 = r28; ctx->r29 = r29; ctx->r30 = r30; ctx->r31 = r31;
    ctx->dma_mem_address = dma_mem_address;
    ctx->dma_dram_address = dma_dram_address;
    ctx->jump_target = jump_target;
    ctx->rsp = rsp;
    return RspExitReason::SwapOverlay;
}
RspExitReason battletanx_audio0(uint8_t* rdram, RspContext* ctx) {
    uint32_t                 r1 = ctx->r1,   r2 = ctx->r2,   r3 = ctx->r3,   r4 = ctx->r4,   r5 = ctx->r5,   r6 = ctx->r6,   r7 = ctx->r7;
    uint32_t  r8 = ctx->r8,  r9 = ctx->r9,   r10 = ctx->r10, r11 = ctx->r11, r12 = ctx->r12, r13 = ctx->r13, r14 = ctx->r14, r15 = ctx->r15;
    uint32_t r16 = ctx->r16, r17 = ctx->r17, r18 = ctx->r18, r19 = ctx->r19, r20 = ctx->r20, r21 = ctx->r21, r22 = ctx->r22, r23 = ctx->r23;
    uint32_t r24 = ctx->r24, r25 = ctx->r25, r26 = ctx->r26, r27 = ctx->r27, r28 = ctx->r28, r29 = ctx->r29, r30 = ctx->r30, r31 = ctx->r31;
    uint32_t dma_mem_address = ctx->dma_mem_address, dma_dram_address = ctx->dma_dram_address, jump_target = ctx->jump_target;
    const char * debug_file = NULL; int debug_line = 0;
    RSP rsp = ctx->rsp;
    if (ctx->resume_delay) {
        switch (ctx->resume_address) {
        }
    } else {
        switch (ctx->resume_address) {
            case 0x11A4: goto R_11A4;
            case 0x101C: goto R_101C;
        }
    }
    printf("Unhandled resume target 0x%04X (delay slot: %d) in microcode battletanx_audio\n", ctx->resume_address, ctx->resume_delay);
    return RspExitReason::UnhandledResumeTarget;
    r1 = 0xFC0;
L_1000:
    // j           L_1064
    // addi        $1, $zero, 0xFC0
    r1 = RSP_ADD32(0, 0XFC0);
    goto L_1064;
    // addi        $1, $zero, 0xFC0
    r1 = RSP_ADD32(0, 0XFC0);
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
    if (dma_mem_address & 0x1000) {
        ctx->resume_address = 0x101C;
        ctx->resume_delay = false;
        goto do_overlay_swap;
    }
    DO_DMA_READ(r3);
R_101C:
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
    // nop

    // jal         0x103C
    r31 = 0x107C;
    // nop

    goto L_103C;
    // nop

L_107C:
    // mfc0        $2, DPC_STATUS
    r2 = 0;
L_1080:
    // addi        $24, $zero, 0x360
    r24 = RSP_ADD32(0, 0X360);
    // addi        $23, $zero, 0xF90
    r23 = RSP_ADD32(0, 0XF90);
    // lw          $28, 0x30($1)
    r28 = RSP_MEM_W_LOAD(0X30, r1);
L_108C:
    // lw          $27, 0x34($1)
    r27 = RSP_MEM_W_LOAD(0X34, r1);
    // mfc0        $5, DPC_STATUS
    r5 = 0;
    // andi        $4, $5, 0x1
    r4 = r5 & 0X1;
    // beq         $4, $zero, L_10B4
    if (r4 == 0) {
        // andi        $4, $5, 0x100
        r4 = r5 & 0X100;
        goto L_10B4;
    }
    // andi        $4, $5, 0x100
    r4 = r5 & 0X100;
    // beq         $4, $zero, L_10B4
    if (r4 == 0) {
        // nop
    
        goto L_10B4;
    }
    // nop

L_10A8:
    // mfc0        $4, DPC_STATUS
    r4 = 0;
    // andi        $4, $4, 0x100
    r4 = r4 & 0X100;
    // bgtz        $4, L_10A8
    if (RSP_SIGNED(r4) > 0) {
        // nop
    
        goto L_10A8;
    }
L_10B4:
    // nop

    // jal         0x1150
    r31 = 0x10C0;
    // nop

    goto L_1150;
    // nop

L_10C0:
    // addi        $2, $zero, 0xF
    r2 = RSP_ADD32(0, 0XF);
    // addi        $1, $zero, 0x320
    r1 = RSP_ADD32(0, 0X320);
L_10C8:
    // sw          $zero, 0x0($1)
    RSP_MEM_W_STORE(0X0, r1, 0);
    // bgtz        $2, L_10C8
    if (RSP_SIGNED(r2) > 0) {
        // addi        $2, $2, -0x1
        r2 = RSP_ADD32(r2, -0X1);
        goto L_10C8;
    }
    // addi        $2, $2, -0x1
    r2 = RSP_ADD32(r2, -0X1);
L_10D4:
    // mfc0        $2, SP_DMA_BUSY
    r2 = 0;
    // bne         $2, $zero, L_10D4
    if (r2 != 0) {
        // addi        $29, $zero, 0x380
        r29 = RSP_ADD32(0, 0X380);
        goto L_10D4;
    }
    // addi        $29, $zero, 0x380
    r29 = RSP_ADD32(0, 0X380);
    // mtc0        $zero, SP_SEMAPHORE
L_10E4:
    // lw          $26, 0x0($29)
    r26 = RSP_MEM_W_LOAD(0X0, r29);
    // lw          $25, 0x4($29)
    r25 = RSP_MEM_W_LOAD(0X4, r29);
    // srl         $1, $26, 23
    r1 = S32(U32(r26) >> 23);
    // andi        $1, $1, 0xFE
    r1 = r1 & 0XFE;
    // addi        $28, $28, 0x8
    r28 = RSP_ADD32(r28, 0X8);
    // addi        $27, $27, -0x8
    r27 = RSP_ADD32(r27, -0X8);
    // addi        $29, $29, 0x8
    r29 = RSP_ADD32(r29, 0X8);
    // addi        $30, $30, -0x8
    r30 = RSP_ADD32(r30, -0X8);
    // add         $2, $zero, $1
    r2 = RSP_ADD32(0, r1);
    // lh          $2, 0x10($2)
    r2 = RSP_MEM_H_LOAD(0X10, r2);
    // jr          $2
    jump_target = r2;
    debug_file = __FILE__; debug_line = __LINE__;
    // nop

    goto do_indirect_jump;
    // nop

    // break       0
    return RspExitReason::Broke;
L_1118:
    // bgtz        $30, L_10E4
    if (RSP_SIGNED(r30) > 0) {
        // nop
    
        goto L_10E4;
    }
    // nop

    // blez        $27, L_1138
    if (RSP_SIGNED(r27) <= 0) {
        // nop
    
        goto L_1138;
    }
    // nop

    // jal         0x1150
    r31 = 0x1130;
    // nop

    goto L_1150;
    // nop

L_1130:
    // j           L_10D4
    // nop

    goto L_10D4;
    // nop

L_1138:
    // ori         $1, $zero, 0x4000
    r1 = 0 | 0X4000;
    // mtc0        $1, SP_STATUS
    set_sp_status(r1);
    // break       0
    return RspExitReason::Broke;
    // nop

L_1148:
    // b           L_1148
    // nop

    goto L_1148;
    // nop

L_1150:
    // addi        $5, $ra, 0x0
    r5 = RSP_ADD32(r31, 0X0);
    // add         $2, $zero, $28
    r2 = RSP_ADD32(0, r28);
    // addi        $3, $27, 0x0
    r3 = RSP_ADD32(r27, 0X0);
L_115C:
    // addi        $4, $3, -0x140
    r4 = RSP_ADD32(r3, -0X140);
    // blez        $4, L_116C
    if (RSP_SIGNED(r4) <= 0) {
        // addi        $1, $zero, 0x380
        r1 = RSP_ADD32(0, 0X380);
        goto L_116C;
    }
    // addi        $1, $zero, 0x380
    r1 = RSP_ADD32(0, 0X380);
    // addi        $3, $zero, 0x140
    r3 = RSP_ADD32(0, 0X140);
L_116C:
    // addi        $30, $3, 0x0
    r30 = RSP_ADD32(r3, 0X0);
    // jal         0x1184
    r31 = 0x1178;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
    goto L_1184;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
L_1178:
    // addi        $29, $zero, 0x380
    r29 = RSP_ADD32(0, 0X380);
    // jr          $5
    jump_target = r5;
    debug_file = __FILE__; debug_line = __LINE__;
    // nop

    goto do_indirect_jump;
    // nop

L_1184:
    // mfc0        $4, SP_SEMAPHORE
    r4 = 0;
    // bne         $4, $zero, L_1184
    if (r4 != 0) {
        // nop
    
        goto L_1184;
    }
    // nop

L_1190:
    // mfc0        $4, SP_DMA_FULL
    r4 = 0;
    // bne         $4, $zero, L_1190
    if (r4 != 0) {
        // nop
    
        goto L_1190;
    }
    // nop

    // mtc0        $1, SP_MEM_ADDR
    SET_DMA_MEM(r1);
    // mtc0        $2, SP_DRAM_ADDR
    SET_DMA_DRAM(r2);
    // mtc0        $3, SP_RD_LEN
    if (dma_mem_address & 0x1000) {
        ctx->resume_address = 0x11A4;
        ctx->resume_delay = false;
        goto do_overlay_swap;
    }
    DO_DMA_READ(r3);
R_11A4:
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // nop

    goto do_indirect_jump;
    // nop

L_11B0:
    // mfc0        $4, SP_SEMAPHORE
    r4 = 0;
    // bne         $4, $zero, L_11B0
    if (r4 != 0) {
        // nop
    
        goto L_11B0;
    }
    // nop

L_11BC:
    // mfc0        $4, SP_DMA_FULL
    r4 = 0;
    // bne         $4, $zero, L_11BC
    if (r4 != 0) {
        // nop
    
        goto L_11BC;
    }
    // nop

    // mtc0        $1, SP_MEM_ADDR
    SET_DMA_MEM(r1);
    // mtc0        $2, SP_DRAM_ADDR
    SET_DMA_DRAM(r2);
    // mtc0        $3, SP_WR_LEN
    DO_DMA_WRITE(r3);
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // nop

    goto do_indirect_jump;
    // nop

L_11DC:
    // andi        $3, $25, 0xFFFF
    r3 = r25 & 0XFFFF;
    // beq         $3, $zero, L_1118
    if (r3 == 0) {
        // addi        $4, $zero, 0x5C0
        r4 = RSP_ADD32(0, 0X5C0);
        goto L_1118;
    }
    // addi        $4, $zero, 0x5C0
    r4 = RSP_ADD32(0, 0X5C0);
    // andi        $2, $26, 0xFFFF
    r2 = r26 & 0XFFFF;
    // add         $2, $2, $4
    r2 = RSP_ADD32(r2, r4);
L_11F0:
    // vxor        $v1, $v1, $v1
    rsp.VXOR<0>(rsp.vpu.r[1], rsp.vpu.r[1], rsp.vpu.r[1]);
    // addi        $3, $3, -0x10
    r3 = RSP_ADD32(r3, -0X10);
L_11F8:
    // sdv         $v1[0], 0x0($2)
    rsp.SDV<0>(rsp.vpu.r[1], r2, 0X0);
    // sdv         $v1[0], 0x8($2)
    rsp.SDV<0>(rsp.vpu.r[1], r2, 0X1);
    // addi        $2, $2, 0x10
    r2 = RSP_ADD32(r2, 0X10);
    // bgtz        $3, L_11F8
    if (RSP_SIGNED(r3) > 0) {
        // addi        $3, $3, -0x10
        r3 = RSP_ADD32(r3, -0X10);
        goto L_11F8;
    }
    // addi        $3, $3, -0x10
    r3 = RSP_ADD32(r3, -0X10);
    // j           L_1118
    // nop

    goto L_1118;
    // nop

L_1214:
    // lhu         $3, 0x4($24)
    r3 = RSP_MEM_HU_LOAD(0X4, r24);
    // beq         $3, $zero, L_1118
    if (r3 == 0) {
        // sll         $2, $25, 8
        r2 = S32(r25) << 8;
        goto L_1118;
    }
    // sll         $2, $25, 8
    r2 = S32(r25) << 8;
    // srl         $2, $2, 8
    r2 = S32(U32(r2) >> 8);
    // srl         $4, $25, 24
    r4 = S32(U32(r25) >> 24);
    // sll         $4, $4, 2
    r4 = S32(r4) << 2;
    // lw          $5, 0x320($4)
    r5 = RSP_MEM_W_LOAD(0X320, r4);
    // add         $2, $2, $5
    r2 = RSP_ADD32(r2, r5);
    // lhu         $1, 0x0($24)
    r1 = RSP_MEM_HU_LOAD(0X0, r24);
    // jal         0x1184
    r31 = 0x1240;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
    goto L_1184;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
L_1240:
    // mfc0        $1, SP_DMA_BUSY
    r1 = 0;
    // bne         $1, $zero, L_1240
    if (r1 != 0) {
        // nop
    
        goto L_1240;
    }
    // nop

    // j           L_1118
    // mtc0        $zero, SP_SEMAPHORE
    goto L_1118;
    // mtc0        $zero, SP_SEMAPHORE
L_1254:
    // lhu         $3, 0x4($24)
    r3 = RSP_MEM_HU_LOAD(0X4, r24);
    // beq         $3, $zero, L_1118
    if (r3 == 0) {
        // sll         $2, $25, 8
        r2 = S32(r25) << 8;
        goto L_1118;
    }
    // sll         $2, $25, 8
    r2 = S32(r25) << 8;
    // srl         $2, $2, 8
    r2 = S32(U32(r2) >> 8);
    // srl         $4, $25, 24
    r4 = S32(U32(r25) >> 24);
    // sll         $4, $4, 2
    r4 = S32(r4) << 2;
    // lw          $5, 0x320($4)
    r5 = RSP_MEM_W_LOAD(0X320, r4);
    // add         $2, $2, $5
    r2 = RSP_ADD32(r2, r5);
    // lhu         $1, 0x2($24)
    r1 = RSP_MEM_HU_LOAD(0X2, r24);
    // jal         0x11B0
    r31 = 0x1280;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
    goto L_11B0;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
L_1280:
    // mfc0        $1, SP_DMA_BUSY
    r1 = 0;
    // bne         $1, $zero, L_1280
    if (r1 != 0) {
        // nop
    
        goto L_1280;
    }
    // nop

    // j           L_1118
    // mtc0        $zero, SP_SEMAPHORE
    goto L_1118;
    // mtc0        $zero, SP_SEMAPHORE
L_1294:
    // sll         $2, $25, 8
    r2 = S32(r25) << 8;
    // srl         $2, $2, 8
    r2 = S32(U32(r2) >> 8);
    // srl         $4, $25, 24
    r4 = S32(U32(r25) >> 24);
    // sll         $4, $4, 2
    r4 = S32(r4) << 2;
    // lw          $5, 0x320($4)
    r5 = RSP_MEM_W_LOAD(0X320, r4);
    // add         $2, $2, $5
    r2 = RSP_ADD32(r2, r5);
    // addi        $1, $zero, 0x4C0
    r1 = RSP_ADD32(0, 0X4C0);
    // andi        $3, $26, 0xFFFF
    r3 = r26 & 0XFFFF;
    // jal         0x1184
    r31 = 0x12BC;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
    goto L_1184;
    // addi        $3, $3, -0x1
    r3 = RSP_ADD32(r3, -0X1);
L_12BC:
    // mfc0        $1, SP_DMA_BUSY
    r1 = 0;
    // bne         $1, $zero, L_12BC
    if (r1 != 0) {
        // nop
    
        goto L_12BC;
    }
    // nop

    // j           L_1118
    // mtc0        $zero, SP_SEMAPHORE
    goto L_1118;
    // mtc0        $zero, SP_SEMAPHORE
L_12D0:
    // sll         $3, $25, 8
    r3 = S32(r25) << 8;
    // srl         $3, $3, 8
    r3 = S32(U32(r3) >> 8);
    // srl         $2, $25, 24
    r2 = S32(U32(r25) >> 24);
    // sll         $2, $2, 2
    r2 = S32(r2) << 2;
    // add         $4, $zero, $2
    r4 = RSP_ADD32(0, r2);
    // j           L_1118
    // sw          $3, 0x320($4)
    RSP_MEM_W_STORE(0X320, r4, r3);
    goto L_1118;
    // sw          $3, 0x320($4)
    RSP_MEM_W_STORE(0X320, r4, r3);
L_12EC:
    // addi        $1, $26, 0x5C0
    r1 = RSP_ADD32(r26, 0X5C0);
    // srl         $2, $25, 16
    r2 = S32(U32(r25) >> 16);
    // addi        $2, $2, 0x5C0
    r2 = RSP_ADD32(r2, 0X5C0);
    // srl         $4, $26, 16
    r4 = S32(U32(r26) >> 16);
    // andi        $4, $4, 0x8
    r4 = r4 & 0X8;
    // bgtz        $4, L_1318
    if (RSP_SIGNED(r4) > 0) {
        // addi        $3, $25, 0x5C0
        r3 = RSP_ADD32(r25, 0X5C0);
        goto L_1318;
    }
    // addi        $3, $25, 0x5C0
    r3 = RSP_ADD32(r25, 0X5C0);
    // sh          $1, 0x0($24)
    RSP_MEM_H_STORE(0X0, r24, r1);
    // sh          $2, 0x2($24)
    RSP_MEM_H_STORE(0X2, r24, r2);
    // j           L_1118
    // sh          $25, 0x4($24)
    RSP_MEM_H_STORE(0X4, r24, r25);
    goto L_1118;
    // sh          $25, 0x4($24)
    RSP_MEM_H_STORE(0X4, r24, r25);
L_1318:
    // sh          $3, 0xE($24)
    RSP_MEM_H_STORE(0XE, r24, r3);
    // sh          $1, 0xA($24)
    RSP_MEM_H_STORE(0XA, r24, r1);
    // j           L_1118
    // sh          $2, 0xC($24)
    RSP_MEM_H_STORE(0XC, r24, r2);
    goto L_1118;
    // sh          $2, 0xC($24)
    RSP_MEM_H_STORE(0XC, r24, r2);
L_1328:
    // srl         $2, $26, 16
    r2 = S32(U32(r26) >> 16);
    // andi        $1, $2, 0x8
    r1 = r2 & 0X8;
    // beq         $1, $zero, L_1344
    if (r1 == 0) {
        // andi        $1, $2, 0x4
        r1 = r2 & 0X4;
        goto L_1344;
    }
    // andi        $1, $2, 0x4
    r1 = r2 & 0X4;
L_1338:
    // sh          $26, 0x1C($24)
    RSP_MEM_H_STORE(0X1C, r24, r26);
    // j           L_1118
    // sh          $25, 0x1E($24)
    RSP_MEM_H_STORE(0X1E, r24, r25);
    goto L_1118;
    // sh          $25, 0x1E($24)
    RSP_MEM_H_STORE(0X1E, r24, r25);
L_1344:
    // beq         $1, $zero, L_1364
    if (r1 == 0) {
        // andi        $1, $2, 0x2
        r1 = r2 & 0X2;
        goto L_1364;
    }
    // andi        $1, $2, 0x2
    r1 = r2 & 0X2;
    // beq         $1, $zero, L_135C
    if (r1 == 0) {
        // nop
    
        goto L_135C;
    }
    // nop

    // j           L_1118
    // sh          $26, 0x6($24)
    RSP_MEM_H_STORE(0X6, r24, r26);
    goto L_1118;
    // sh          $26, 0x6($24)
    RSP_MEM_H_STORE(0X6, r24, r26);
L_135C:
    // j           L_1118
    // sh          $26, 0x8($24)
    RSP_MEM_H_STORE(0X8, r24, r26);
    goto L_1118;
    // sh          $26, 0x8($24)
    RSP_MEM_H_STORE(0X8, r24, r26);
L_1364:
    // beq         $1, $zero, L_137C
    if (r1 == 0) {
        // srl         $1, $25, 16
        r1 = S32(U32(r25) >> 16);
        goto L_137C;
    }
    // srl         $1, $25, 16
    r1 = S32(U32(r25) >> 16);
    // sh          $26, 0x10($24)
    RSP_MEM_H_STORE(0X10, r24, r26);
    // sh          $1, 0x12($24)
    RSP_MEM_H_STORE(0X12, r24, r1);
    // j           L_1118
    // sh          $25, 0x14($24)
    RSP_MEM_H_STORE(0X14, r24, r25);
    goto L_1118;
    // sh          $25, 0x14($24)
    RSP_MEM_H_STORE(0X14, r24, r25);
L_137C:
    // sh          $26, 0x16($24)
    RSP_MEM_H_STORE(0X16, r24, r26);
    // sh          $1, 0x18($24)
    RSP_MEM_H_STORE(0X18, r24, r1);
    // j           L_1118
    // sh          $25, 0x1A($24)
    RSP_MEM_H_STORE(0X1A, r24, r25);
    goto L_1118;
L_1388:
    // sh          $25, 0x1A($24)
    RSP_MEM_H_STORE(0X1A, r24, r25);
L_138C:
    // lhu         $1, 0x4($24)
    r1 = RSP_MEM_HU_LOAD(0X4, r24);
    // lhu         $4, 0x2($24)
    r4 = RSP_MEM_HU_LOAD(0X2, r24);
    // beq         $1, $zero, L_1118
    if (r1 == 0) {
        // andi        $3, $25, 0xFFFF
        r3 = r25 & 0XFFFF;
        goto L_1118;
    }
    // andi        $3, $25, 0xFFFF
    r3 = r25 & 0XFFFF;
    // addi        $3, $3, 0x5C0
    r3 = RSP_ADD32(r3, 0X5C0);
    // srl         $2, $25, 16
    r2 = S32(U32(r25) >> 16);
    // addi        $2, $2, 0x5C0
    r2 = RSP_ADD32(r2, 0X5C0);
L_13A8:
    // lqv         $v1[0], 0x0($2)
    rsp.LQV<0>(rsp.vpu.r[1], r2, 0X0);
    // lqv         $v2[0], 0x0($3)
    rsp.LQV<0>(rsp.vpu.r[2], r3, 0X0);
    // ssv         $v1[0], 0x0($4)
    rsp.SSV<0>(rsp.vpu.r[1], r4, 0X0);
    // ssv         $v2[0], 0x2($4)
    rsp.SSV<0>(rsp.vpu.r[2], r4, 0X1);
    // ssv         $v1[2], 0x4($4)
    rsp.SSV<2>(rsp.vpu.r[1], r4, 0X2);
    // ssv         $v2[2], 0x6($4)
    rsp.SSV<2>(rsp.vpu.r[2], r4, 0X3);
    // ssv         $v1[4], 0x8($4)
    rsp.SSV<4>(rsp.vpu.r[1], r4, 0X4);
    // ssv         $v2[4], 0xA($4)
    rsp.SSV<4>(rsp.vpu.r[2], r4, 0X5);
    // ssv         $v1[6], 0xC($4)
    rsp.SSV<6>(rsp.vpu.r[1], r4, 0X6);
    // ssv         $v2[6], 0xE($4)
    rsp.SSV<6>(rsp.vpu.r[2], r4, 0X7);
    // ssv         $v1[8], 0x10($4)
    rsp.SSV<8>(rsp.vpu.r[1], r4, 0X8);
    // ssv         $v2[8], 0x12($4)
    rsp.SSV<8>(rsp.vpu.r[2], r4, 0X9);
    // ssv         $v1[10], 0x14($4)
    rsp.SSV<10>(rsp.vpu.r[1], r4, 0XA);
    // ssv         $v2[10], 0x16($4)
    rsp.SSV<10>(rsp.vpu.r[2], r4, 0XB);
    // ssv         $v1[12], 0x18($4)
    rsp.SSV<12>(rsp.vpu.r[1], r4, 0XC);
    // ssv         $v2[12], 0x1A($4)
    rsp.SSV<12>(rsp.vpu.r[2], r4, 0XD);
    // ssv         $v1[14], 0x1C($4)
    rsp.SSV<14>(rsp.vpu.r[1], r4, 0XE);
    // ssv         $v2[14], 0x1E($4)
    rsp.SSV<14>(rsp.vpu.r[2], r4, 0XF);
    // addi        $1, $1, -0x10
    r1 = RSP_ADD32(r1, -0X10);
    // addi        $2, $2, 0x10
    r2 = RSP_ADD32(r2, 0X10);
    // addi        $3, $3, 0x10
    r3 = RSP_ADD32(r3, 0X10);
    // bgtz        $1, L_13A8
    if (RSP_SIGNED(r1) > 0) {
        // addi        $4, $4, 0x20
        r4 = RSP_ADD32(r4, 0X20);
        goto L_13A8;
    }
    // addi        $4, $4, 0x20
    r4 = RSP_ADD32(r4, 0X20);
    // j           L_1118
    // nop

    goto L_1118;
    // nop

L_140C:
    // andi        $1, $25, 0xFFFF
    r1 = r25 & 0XFFFF;
    // beq         $1, $zero, L_1118
    if (r1 == 0) {
        // andi        $2, $26, 0xFFFF
        r2 = r26 & 0XFFFF;
        goto L_1118;
    }
    // andi        $2, $26, 0xFFFF
    r2 = r26 & 0XFFFF;
    // addi        $2, $2, 0x5C0
    r2 = RSP_ADD32(r2, 0X5C0);
    // srl         $3, $25, 16
    r3 = S32(U32(r25) >> 16);
    // addi        $3, $3, 0x5C0
    r3 = RSP_ADD32(r3, 0X5C0);
L_1424:
    // ldv         $v1[0], 0x0($2)
    rsp.LDV<0>(rsp.vpu.r[1], r2, 0X0);
    // ldv         $v2[0], 0x8($2)
    rsp.LDV<0>(rsp.vpu.r[2], r2, 0X1);
    // addi        $1, $1, -0x10
    r1 = RSP_ADD32(r1, -0X10);
    // addi        $2, $2, 0x10
    r2 = RSP_ADD32(r2, 0X10);
    // sdv         $v1[0], 0x0($3)
    rsp.SDV<0>(rsp.vpu.r[1], r3, 0X0);
    // sdv         $v2[0], 0x8($3)
    rsp.SDV<0>(rsp.vpu.r[2], r3, 0X1);
    // bgtz        $1, L_1424
    if (RSP_SIGNED(r1) > 0) {
        // addi        $3, $3, 0x10
        r3 = RSP_ADD32(r3, 0X10);
        goto L_1424;
    }
    // addi        $3, $3, 0x10
    r3 = RSP_ADD32(r3, 0X10);
    // j           L_1118
    // nop

    goto L_1118;
    // nop

L_144C:
    // sll         $1, $25, 8
    r1 = S32(r25) << 8;
    // srl         $1, $1, 8
    r1 = S32(U32(r1) >> 8);
    // srl         $3, $25, 24
    r3 = S32(U32(r25) >> 24);
    // sll         $3, $3, 2
    r3 = S32(r3) << 2;
    // lw          $2, 0x320($3)
    r2 = RSP_MEM_W_LOAD(0X320, r3);
    // add         $1, $1, $2
    r1 = RSP_ADD32(r1, r2);
    // sw          $1, 0x10($24)
    RSP_MEM_W_STORE(0X10, r24, r1);
    // j           L_1118
    // nop

    goto L_1118;
    // nop

L_1470:
    // lqv         $v31[0], 0x0($zero)
    rsp.LQV<0>(rsp.vpu.r[31], 0, 0X0);
    // vxor        $v27, $v27, $v27
    rsp.VXOR<0>(rsp.vpu.r[27], rsp.vpu.r[27], rsp.vpu.r[27]);
    // lhu         $21, 0x0($24)
    r21 = RSP_MEM_HU_LOAD(0X0, r24);
    // vxor        $v25, $v25, $v25
    rsp.VXOR<0>(rsp.vpu.r[25], rsp.vpu.r[25], rsp.vpu.r[25]);
    // vxor        $v24, $v24, $v24
    rsp.VXOR<0>(rsp.vpu.r[24], rsp.vpu.r[24], rsp.vpu.r[24]);
    // addi        $20, $21, 0x1
    r20 = RSP_ADD32(r21, 0X1);
    // lhu         $19, 0x2($24)
    r19 = RSP_MEM_HU_LOAD(0X2, r24);
L_148C:
    // vxor        $v13, $v13, $v13
    rsp.VXOR<0>(rsp.vpu.r[13], rsp.vpu.r[13], rsp.vpu.r[13]);
    // vxor        $v14, $v14, $v14
    rsp.VXOR<0>(rsp.vpu.r[14], rsp.vpu.r[14], rsp.vpu.r[14]);
    // lhu         $18, 0x4($24)
    r18 = RSP_MEM_HU_LOAD(0X4, r24);
    // vxor        $v15, $v15, $v15
    rsp.VXOR<0>(rsp.vpu.r[15], rsp.vpu.r[15], rsp.vpu.r[15]);
    // lui         $1, 0xFF
    r1 = S32(0XFF << 16);
    // vxor        $v16, $v16, $v16
    rsp.VXOR<0>(rsp.vpu.r[16], rsp.vpu.r[16], rsp.vpu.r[16]);
    // ori         $1, $1, 0xFFFF
    r1 = r1 | 0XFFFF;
    // vxor        $v17, $v17, $v17
    rsp.VXOR<0>(rsp.vpu.r[17], rsp.vpu.r[17], rsp.vpu.r[17]);
    // and         $17, $25, $1
    r17 = r25 & r1;
    // vxor        $v18, $v18, $v18
    rsp.VXOR<0>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[18]);
    // srl         $2, $25, 24
    r2 = S32(U32(r25) >> 24);
    // vxor        $v19, $v19, $v19
    rsp.VXOR<0>(rsp.vpu.r[19], rsp.vpu.r[19], rsp.vpu.r[19]);
    // sll         $2, $2, 2
    r2 = S32(r2) << 2;
    // lw          $3, 0x320($2)
    r3 = RSP_MEM_W_LOAD(0X320, r2);
    // add         $17, $17, $3
    r17 = RSP_ADD32(r17, r3);
    // sqv         $v27[0], 0x0($19)
    rsp.SQV<0>(rsp.vpu.r[27], r19, 0X0);
    // sqv         $v27[0], 0x10($19)
    rsp.SQV<0>(rsp.vpu.r[27], r19, 0X1);
    // srl         $1, $26, 16
    r1 = S32(U32(r26) >> 16);
    // andi        $1, $1, 0x1
    r1 = r1 & 0X1;
    // bgtz        $1, L_150C
    if (RSP_SIGNED(r1) > 0) {
        // srl         $1, $26, 16
        r1 = S32(U32(r26) >> 16);
        goto L_150C;
    }
    // srl         $1, $26, 16
    r1 = S32(U32(r26) >> 16);
    // andi        $1, $1, 0x2
    r1 = r1 & 0X2;
    // beq         $zero, $1, L_14F0
    if (0 == r1) {
        // addi        $2, $17, 0x0
        r2 = RSP_ADD32(r17, 0X0);
        goto L_14F0;
    }
    // addi        $2, $17, 0x0
    r2 = RSP_ADD32(r17, 0X0);
    // lw          $2, 0x10($24)
    r2 = RSP_MEM_W_LOAD(0X10, r24);
L_14F0:
    // addi        $1, $19, 0x0
    r1 = RSP_ADD32(r19, 0X0);
    // jal         0x1184
    r31 = 0x14FC;
    // addi        $3, $zero, 0x1F
    r3 = RSP_ADD32(0, 0X1F);
    goto L_1184;
    // addi        $3, $zero, 0x1F
    r3 = RSP_ADD32(0, 0X1F);
L_14FC:
    // mfc0        $5, SP_DMA_BUSY
    r5 = 0;
    // bne         $5, $zero, L_14FC
    if (r5 != 0) {
        // nop
    
        goto L_14FC;
    }
    // nop

    // mtc0        $zero, SP_SEMAPHORE
L_150C:
    // addi        $16, $zero, 0x30
    r16 = RSP_ADD32(0, 0X30);
    // addi        $15, $zero, 0x4C0
    r15 = RSP_ADD32(0, 0X4C0);
    // ldv         $v25[0], 0x0($16)
    rsp.LDV<0>(rsp.vpu.r[25], r16, 0X0);
    // ldv         $v24[8], 0x0($16)
    rsp.LDV<8>(rsp.vpu.r[24], r16, 0X0);
    // ldv         $v23[0], 0x8($16)
    rsp.LDV<0>(rsp.vpu.r[23], r16, 0X1);
    // ldv         $v23[8], 0x8($16)
    rsp.LDV<8>(rsp.vpu.r[23], r16, 0X1);
    // lqv         $v27[0], 0x10($19)
    rsp.LQV<0>(rsp.vpu.r[27], r19, 0X1);
    // addi        $19, $19, 0x20
    r19 = RSP_ADD32(r19, 0X20);
    // beq         $18, $zero, L_16E8
    if (r18 == 0) {
        // ldv         $v1[0], 0x0($20)
        rsp.LDV<0>(rsp.vpu.r[1], r20, 0X0);
        goto L_16E8;
    }
    // ldv         $v1[0], 0x0($20)
    rsp.LDV<0>(rsp.vpu.r[1], r20, 0X0);
    // lbu         $1, 0x0($21)
    r1 = RSP_MEM_BU(0X0, r21);
    // andi        $11, $1, 0xF
    r11 = r1 & 0XF;
    // sll         $11, $11, 5
    r11 = S32(r11) << 5;
    // vand        $v3, $v25, $v1[0]
    rsp.VAND<8>(rsp.vpu.r[3], rsp.vpu.r[25], rsp.vpu.r[1]);
    // add         $13, $11, $15
    r13 = RSP_ADD32(r11, r15);
    // vand        $v4, $v24, $v1[1]
    rsp.VAND<9>(rsp.vpu.r[4], rsp.vpu.r[24], rsp.vpu.r[1]);
    // srl         $14, $1, 4
    r14 = S32(U32(r1) >> 4);
    // vand        $v5, $v25, $v1[2]
    rsp.VAND<10>(rsp.vpu.r[5], rsp.vpu.r[25], rsp.vpu.r[1]);
    // addi        $2, $zero, 0xC
    r2 = RSP_ADD32(0, 0XC);
    // vand        $v6, $v24, $v1[3]
    rsp.VAND<11>(rsp.vpu.r[6], rsp.vpu.r[24], rsp.vpu.r[1]);
    // sub         $14, $2, $14
    r14 = RSP_SUB32(r2, r14);
    // addi        $2, $14, -0x1
    r2 = RSP_ADD32(r14, -0X1);
    // addi        $3, $zero, 0x1
    r3 = RSP_ADD32(0, 0X1);
    // sll         $3, $3, 15
    r3 = S32(r3) << 15;
    // srlv        $4, $3, $2
    r4 = S32(U32(r3) >> (r2 & 31));
    // mtc2        $4, $v22[0]
    rsp.MTC2<0>(r4, rsp.vpu.r[22]);
    // lqv         $v21[0], 0x0($13)
    rsp.LQV<0>(rsp.vpu.r[21], r13, 0X0);
    // lqv         $v20[0], 0x10($13)
    rsp.LQV<0>(rsp.vpu.r[20], r13, 0X1);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v19[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[19], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v18[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[18], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v17[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[17], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v16[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[16], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v15[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[15], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v14[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[14], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v13[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[13], r13, 0X2);
L_15B4:
    // addi        $20, $20, 0x9
    r20 = RSP_ADD32(r20, 0X9);
    // vmudn       $v30, $v3, $v23
    rsp.VMUDN<0>(rsp.vpu.r[30], rsp.vpu.r[3], rsp.vpu.r[23]);
    // addi        $21, $21, 0x9
    r21 = RSP_ADD32(r21, 0X9);
    // vmadn       $v30, $v4, $v23
    rsp.VMADN<0>(rsp.vpu.r[30], rsp.vpu.r[4], rsp.vpu.r[23]);
    // ldv         $v1[0], 0x0($20)
    rsp.LDV<0>(rsp.vpu.r[1], r20, 0X0);
    // vmudn       $v29, $v5, $v23
    rsp.VMUDN<0>(rsp.vpu.r[29], rsp.vpu.r[5], rsp.vpu.r[23]);
    // lbu         $1, 0x0($21)
    r1 = RSP_MEM_BU(0X0, r21);
    // vmadn       $v29, $v6, $v23
    rsp.VMADN<0>(rsp.vpu.r[29], rsp.vpu.r[6], rsp.vpu.r[23]);
    // blez        $14, L_15E4
    if (RSP_SIGNED(r14) <= 0) {
        // andi        $11, $1, 0xF
        r11 = r1 & 0XF;
        goto L_15E4;
    }
    // andi        $11, $1, 0xF
    r11 = r1 & 0XF;
    // vmudm       $v30, $v30, $v22[0]
    rsp.VMUDM<8>(rsp.vpu.r[30], rsp.vpu.r[30], rsp.vpu.r[22]);
    // vmudm       $v29, $v29, $v22[0]
    rsp.VMUDM<8>(rsp.vpu.r[29], rsp.vpu.r[29], rsp.vpu.r[22]);
L_15E4:
    // sll         $11, $11, 5
    r11 = S32(r11) << 5;
    // vand        $v3, $v25, $v1[0]
    rsp.VAND<8>(rsp.vpu.r[3], rsp.vpu.r[25], rsp.vpu.r[1]);
    // add         $13, $11, $15
    r13 = RSP_ADD32(r11, r15);
    // vand        $v4, $v24, $v1[1]
    rsp.VAND<9>(rsp.vpu.r[4], rsp.vpu.r[24], rsp.vpu.r[1]);
    // vand        $v5, $v25, $v1[2]
    rsp.VAND<10>(rsp.vpu.r[5], rsp.vpu.r[25], rsp.vpu.r[1]);
    // vand        $v6, $v24, $v1[3]
    rsp.VAND<11>(rsp.vpu.r[6], rsp.vpu.r[24], rsp.vpu.r[1]);
    // srl         $14, $1, 4
    r14 = S32(U32(r1) >> 4);
    // vmudh       $v2, $v21, $v27[6]
    rsp.VMUDH<14>(rsp.vpu.r[2], rsp.vpu.r[21], rsp.vpu.r[27]);
    // addi        $2, $zero, 0xC
    r2 = RSP_ADD32(0, 0XC);
    // vmadh       $v2, $v20, $v27[7]
    rsp.VMADH<15>(rsp.vpu.r[2], rsp.vpu.r[20], rsp.vpu.r[27]);
    // sub         $14, $2, $14
    r14 = RSP_SUB32(r2, r14);
    // vmadh       $v2, $v19, $v30[0]
    rsp.VMADH<8>(rsp.vpu.r[2], rsp.vpu.r[19], rsp.vpu.r[30]);
    // addi        $2, $14, -0x1
    r2 = RSP_ADD32(r14, -0X1);
    // vmadh       $v2, $v18, $v30[1]
    rsp.VMADH<9>(rsp.vpu.r[2], rsp.vpu.r[18], rsp.vpu.r[30]);
    // addi        $3, $zero, 0x1
    r3 = RSP_ADD32(0, 0X1);
    // vmadh       $v2, $v17, $v30[2]
    rsp.VMADH<10>(rsp.vpu.r[2], rsp.vpu.r[17], rsp.vpu.r[30]);
    // sll         $3, $3, 15
    r3 = S32(r3) << 15;
    // vmadh       $v2, $v16, $v30[3]
    rsp.VMADH<11>(rsp.vpu.r[2], rsp.vpu.r[16], rsp.vpu.r[30]);
    // srlv        $4, $3, $2
    r4 = S32(U32(r3) >> (r2 & 31));
    // vmadh       $v28, $v15, $v30[4]
    rsp.VMADH<12>(rsp.vpu.r[28], rsp.vpu.r[15], rsp.vpu.r[30]);
    // mtc2        $4, $v22[0]
    rsp.MTC2<0>(r4, rsp.vpu.r[22]);
    // vmadh       $v2, $v14, $v30[5]
    rsp.VMADH<13>(rsp.vpu.r[2], rsp.vpu.r[14], rsp.vpu.r[30]);
    // vmadh       $v2, $v13, $v30[6]
    rsp.VMADH<14>(rsp.vpu.r[2], rsp.vpu.r[13], rsp.vpu.r[30]);
    // vmadh       $v2, $v30, $v31[5]
    rsp.VMADH<13>(rsp.vpu.r[2], rsp.vpu.r[30], rsp.vpu.r[31]);
    // vsar        $v26, $v7, $v28[1]
    rsp.VSAR<9>(rsp.vpu.r[26], rsp.vpu.r[7]);
    // vsar        $v28, $v7, $v28[0]
    rsp.VSAR<8>(rsp.vpu.r[28], rsp.vpu.r[7]);
    // vmudn       $v2, $v26, $v31[4]
    rsp.VMUDN<12>(rsp.vpu.r[2], rsp.vpu.r[26], rsp.vpu.r[31]);
    // vmadh       $v28, $v28, $v31[4]
    rsp.VMADH<12>(rsp.vpu.r[28], rsp.vpu.r[28], rsp.vpu.r[31]);
    // vmudh       $v2, $v19, $v29[0]
    rsp.VMUDH<8>(rsp.vpu.r[2], rsp.vpu.r[19], rsp.vpu.r[29]);
    // addi        $12, $13, -0x2
    r12 = RSP_ADD32(r13, -0X2);
    // vmadh       $v2, $v18, $v29[1]
    rsp.VMADH<9>(rsp.vpu.r[2], rsp.vpu.r[18], rsp.vpu.r[29]);
    // lrv         $v19[0], 0x20($12)
    rsp.LRV<0>(rsp.vpu.r[19], r12, 0X2);
    // vmadh       $v2, $v17, $v29[2]
    rsp.VMADH<10>(rsp.vpu.r[2], rsp.vpu.r[17], rsp.vpu.r[29]);
    // addi        $12, $12, -0x2
    r12 = RSP_ADD32(r12, -0X2);
    // vmadh       $v2, $v16, $v29[3]
    rsp.VMADH<11>(rsp.vpu.r[2], rsp.vpu.r[16], rsp.vpu.r[29]);
    // lrv         $v18[0], 0x20($12)
    rsp.LRV<0>(rsp.vpu.r[18], r12, 0X2);
    // vmadh       $v2, $v15, $v29[4]
    rsp.VMADH<12>(rsp.vpu.r[2], rsp.vpu.r[15], rsp.vpu.r[29]);
    // addi        $12, $12, -0x2
    r12 = RSP_ADD32(r12, -0X2);
    // vmadh       $v2, $v14, $v29[5]
    rsp.VMADH<13>(rsp.vpu.r[2], rsp.vpu.r[14], rsp.vpu.r[29]);
    // lrv         $v17[0], 0x20($12)
    rsp.LRV<0>(rsp.vpu.r[17], r12, 0X2);
    // vmadh       $v2, $v13, $v29[6]
    rsp.VMADH<14>(rsp.vpu.r[2], rsp.vpu.r[13], rsp.vpu.r[29]);
    // addi        $12, $12, -0x2
    r12 = RSP_ADD32(r12, -0X2);
    // vmadh       $v2, $v29, $v31[5]
    rsp.VMADH<13>(rsp.vpu.r[2], rsp.vpu.r[29], rsp.vpu.r[31]);
    // lrv         $v16[0], 0x20($12)
    rsp.LRV<0>(rsp.vpu.r[16], r12, 0X2);
    // vmadh       $v2, $v21, $v28[6]
    rsp.VMADH<14>(rsp.vpu.r[2], rsp.vpu.r[21], rsp.vpu.r[28]);
    // addi        $12, $12, -0x2
    r12 = RSP_ADD32(r12, -0X2);
    // vmadh       $v2, $v20, $v28[7]
    rsp.VMADH<15>(rsp.vpu.r[2], rsp.vpu.r[20], rsp.vpu.r[28]);
    // lrv         $v15[0], 0x20($12)
    rsp.LRV<0>(rsp.vpu.r[15], r12, 0X2);
    // vsar        $v26, $v7, $v27[1]
    rsp.VSAR<9>(rsp.vpu.r[26], rsp.vpu.r[7]);
    // addi        $12, $12, -0x2
    r12 = RSP_ADD32(r12, -0X2);
    // vsar        $v27, $v7, $v27[0]
    rsp.VSAR<8>(rsp.vpu.r[27], rsp.vpu.r[7]);
    // lrv         $v14[0], 0x20($12)
    rsp.LRV<0>(rsp.vpu.r[14], r12, 0X2);
    // addi        $12, $12, -0x2
    r12 = RSP_ADD32(r12, -0X2);
    // lrv         $v13[0], 0x20($12)
    rsp.LRV<0>(rsp.vpu.r[13], r12, 0X2);
    // lqv         $v21[0], 0x0($13)
    rsp.LQV<0>(rsp.vpu.r[21], r13, 0X0);
    // vmudn       $v2, $v26, $v31[4]
    rsp.VMUDN<12>(rsp.vpu.r[2], rsp.vpu.r[26], rsp.vpu.r[31]);
    // lqv         $v20[0], 0x10($13)
    rsp.LQV<0>(rsp.vpu.r[20], r13, 0X1);
    // vmadh       $v27, $v27, $v31[4]
    rsp.VMADH<12>(rsp.vpu.r[27], rsp.vpu.r[27], rsp.vpu.r[31]);
    // addi        $18, $18, -0x20
    r18 = RSP_ADD32(r18, -0X20);
    // sdv         $v28[0], 0x0($19)
    rsp.SDV<0>(rsp.vpu.r[28], r19, 0X0);
    // sdv         $v28[8], 0x8($19)
    rsp.SDV<8>(rsp.vpu.r[28], r19, 0X1);
    // sdv         $v27[0], 0x10($19)
    rsp.SDV<0>(rsp.vpu.r[27], r19, 0X2);
    // sdv         $v27[8], 0x18($19)
    rsp.SDV<8>(rsp.vpu.r[27], r19, 0X3);
    // bgtz        $18, L_15B4
    if (RSP_SIGNED(r18) > 0) {
        // addi        $19, $19, 0x20
        r19 = RSP_ADD32(r19, 0X20);
        goto L_15B4;
    }
    // addi        $19, $19, 0x20
    r19 = RSP_ADD32(r19, 0X20);
L_16E8:
    // addi        $1, $19, -0x20
    r1 = RSP_ADD32(r19, -0X20);
    // addi        $2, $17, 0x0
    r2 = RSP_ADD32(r17, 0X0);
    // jal         0x11B0
    r31 = 0x16F8;
    // addi        $3, $zero, 0x1F
    r3 = RSP_ADD32(0, 0X1F);
    goto L_11B0;
    // addi        $3, $zero, 0x1F
    r3 = RSP_ADD32(0, 0X1F);
L_16F8:
    // mfc0        $5, SP_DMA_BUSY
    r5 = 0;
    // bne         $5, $zero, L_16F8
    if (r5 != 0) {
        // nop
    
        goto L_16F8;
    }
    // nop

    // j           L_1118
    // mtc0        $zero, SP_SEMAPHORE
    goto L_1118;
    // mtc0        $zero, SP_SEMAPHORE
L_170C:
    // lqv         $v31[0], 0x0($zero)
    rsp.LQV<0>(rsp.vpu.r[31], 0, 0X0);
    // vxor        $v28, $v28, $v28
    rsp.VXOR<0>(rsp.vpu.r[28], rsp.vpu.r[28], rsp.vpu.r[28]);
    // lhu         $21, 0x0($24)
    r21 = RSP_MEM_HU_LOAD(0X0, r24);
    // vxor        $v17, $v17, $v17
    rsp.VXOR<0>(rsp.vpu.r[17], rsp.vpu.r[17], rsp.vpu.r[17]);
    // lhu         $20, 0x2($24)
    r20 = RSP_MEM_HU_LOAD(0X2, r24);
    // vxor        $v18, $v18, $v18
    rsp.VXOR<0>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[18]);
    // lhu         $19, 0x4($24)
    r19 = RSP_MEM_HU_LOAD(0X4, r24);
    // vxor        $v19, $v19, $v19
    rsp.VXOR<0>(rsp.vpu.r[19], rsp.vpu.r[19], rsp.vpu.r[19]);
    // beq         $19, $zero, L_1874
    if (r19 == 0) {
        // andi        $14, $26, 0xFFFF
        r14 = r26 & 0XFFFF;
        goto L_1874;
    }
    // andi        $14, $26, 0xFFFF
    r14 = r26 & 0XFFFF;
    // mtc2        $14, $v31[10]
    rsp.MTC2<10>(r14, rsp.vpu.r[31]);
    // sll         $14, $14, 2
    r14 = S32(r14) << 2;
    // mtc2        $14, $v16[0]
    rsp.MTC2<0>(r14, rsp.vpu.r[16]);
    // lui         $1, 0xFF
    r1 = S32(0XFF << 16);
    // vxor        $v20, $v20, $v20
    rsp.VXOR<0>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[20]);
    // ori         $1, $1, 0xFFFF
    r1 = r1 | 0XFFFF;
    // vxor        $v21, $v21, $v21
    rsp.VXOR<0>(rsp.vpu.r[21], rsp.vpu.r[21], rsp.vpu.r[21]);
    // and         $18, $25, $1
    r18 = r25 & r1;
    // vxor        $v22, $v22, $v22
    rsp.VXOR<0>(rsp.vpu.r[22], rsp.vpu.r[22], rsp.vpu.r[22]);
    // srl         $2, $25, 24
    r2 = S32(U32(r25) >> 24);
    // vxor        $v23, $v23, $v23
    rsp.VXOR<0>(rsp.vpu.r[23], rsp.vpu.r[23], rsp.vpu.r[23]);
    // sll         $2, $2, 2
    r2 = S32(r2) << 2;
    // lw          $3, 0x320($2)
    r3 = RSP_MEM_W_LOAD(0X320, r2);
    // add         $18, $18, $3
    r18 = RSP_ADD32(r18, r3);
    // slv         $v28[0], 0x0($23)
    rsp.SLV<0>(rsp.vpu.r[28], r23, 0X0);
    // srl         $1, $26, 16
    r1 = S32(U32(r26) >> 16);
    // andi        $1, $1, 0x1
    r1 = r1 & 0X1;
    // bgtz        $1, L_17A0
    if (RSP_SIGNED(r1) > 0) {
        // nop
    
        goto L_17A0;
    }
    // nop

    // addi        $1, $23, 0x0
    r1 = RSP_ADD32(r23, 0X0);
    // addi        $2, $18, 0x0
    r2 = RSP_ADD32(r18, 0X0);
    // jal         0x1184
    r31 = 0x1790;
    // addi        $3, $zero, 0x7
    r3 = RSP_ADD32(0, 0X7);
    goto L_1184;
    // addi        $3, $zero, 0x7
    r3 = RSP_ADD32(0, 0X7);
L_1790:
    // mfc0        $5, SP_DMA_BUSY
    r5 = 0;
    // bne         $5, $zero, L_1790
    if (r5 != 0) {
        // nop
    
        goto L_1790;
    }
    // nop

L_179C:
    // mtc0        $zero, SP_SEMAPHORE
L_17A0:
    // addi        $13, $zero, 0x4C0
    r13 = RSP_ADD32(0, 0X4C0);
    // addi        $1, $zero, 0x4
    r1 = RSP_ADD32(0, 0X4);
    // mtc2        $1, $v14[0]
    rsp.MTC2<0>(r1, rsp.vpu.r[14]);
    // lqv         $v24[0], 0x10($13)
    rsp.LQV<0>(rsp.vpu.r[24], r13, 0X1);
    // vmudm       $v16, $v24, $v16[0]
    rsp.VMUDM<8>(rsp.vpu.r[16], rsp.vpu.r[24], rsp.vpu.r[16]);
    // ldv         $v28[8], 0x0($23)
    rsp.LDV<8>(rsp.vpu.r[28], r23, 0X0);
    // sqv         $v16[0], 0x10($13)
    rsp.SQV<0>(rsp.vpu.r[16], r13, 0X1);
    // lqv         $v25[0], 0x0($13)
    rsp.LQV<0>(rsp.vpu.r[25], r13, 0X0);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v23[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[23], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v22[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[22], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v21[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[21], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v20[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[20], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v19[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[19], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v18[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[18], r13, 0X2);
    // addi        $13, $13, -0x2
    r13 = RSP_ADD32(r13, -0X2);
    // lrv         $v17[0], 0x20($13)
    rsp.LRV<0>(rsp.vpu.r[17], r13, 0X2);
    // ldv         $v30[0], 0x0($21)
    rsp.LDV<0>(rsp.vpu.r[30], r21, 0X0);
    // ldv         $v30[8], 0x8($21)
    rsp.LDV<8>(rsp.vpu.r[30], r21, 0X1);
L_1800:
    // vmudh       $v16, $v25, $v28[6]
    rsp.VMUDH<14>(rsp.vpu.r[16], rsp.vpu.r[25], rsp.vpu.r[28]);
    // addi        $21, $21, 0x10
    r21 = RSP_ADD32(r21, 0X10);
    // vmadh       $v16, $v24, $v28[7]
    rsp.VMADH<15>(rsp.vpu.r[16], rsp.vpu.r[24], rsp.vpu.r[28]);
    // addi        $19, $19, -0x10
    r19 = RSP_ADD32(r19, -0X10);
    // vmadh       $v16, $v23, $v30[0]
    rsp.VMADH<8>(rsp.vpu.r[16], rsp.vpu.r[23], rsp.vpu.r[30]);
    // vmadh       $v16, $v22, $v30[1]
    rsp.VMADH<9>(rsp.vpu.r[16], rsp.vpu.r[22], rsp.vpu.r[30]);
    // vmadh       $v16, $v21, $v30[2]
    rsp.VMADH<10>(rsp.vpu.r[16], rsp.vpu.r[21], rsp.vpu.r[30]);
    // vmadh       $v16, $v20, $v30[3]
    rsp.VMADH<11>(rsp.vpu.r[16], rsp.vpu.r[20], rsp.vpu.r[30]);
    // vmadh       $v28, $v19, $v30[4]
    rsp.VMADH<12>(rsp.vpu.r[28], rsp.vpu.r[19], rsp.vpu.r[30]);
    // vmadh       $v16, $v18, $v30[5]
    rsp.VMADH<13>(rsp.vpu.r[16], rsp.vpu.r[18], rsp.vpu.r[30]);
    // vmadh       $v16, $v17, $v30[6]
    rsp.VMADH<14>(rsp.vpu.r[16], rsp.vpu.r[17], rsp.vpu.r[30]);
    // vmadh       $v16, $v30, $v31[5]
    rsp.VMADH<13>(rsp.vpu.r[16], rsp.vpu.r[30], rsp.vpu.r[31]);
    // ldv         $v30[0], 0x0($21)
    rsp.LDV<0>(rsp.vpu.r[30], r21, 0X0);
    // vsar        $v26, $v15, $v28[1]
    rsp.VSAR<9>(rsp.vpu.r[26], rsp.vpu.r[15]);
    // ldv         $v30[8], 0x8($21)
    rsp.LDV<8>(rsp.vpu.r[30], r21, 0X1);
    // vsar        $v28, $v15, $v28[0]
    rsp.VSAR<8>(rsp.vpu.r[28], rsp.vpu.r[15]);
    // vmudn       $v16, $v26, $v14[0]
    rsp.VMUDN<8>(rsp.vpu.r[16], rsp.vpu.r[26], rsp.vpu.r[14]);
    // vmadh       $v28, $v28, $v14[0]
    rsp.VMADH<8>(rsp.vpu.r[28], rsp.vpu.r[28], rsp.vpu.r[14]);
    // sdv         $v28[0], 0x0($20)
    rsp.SDV<0>(rsp.vpu.r[28], r20, 0X0);
    // sdv         $v28[8], 0x8($20)
    rsp.SDV<8>(rsp.vpu.r[28], r20, 0X1);
    // bgtz        $19, L_1800
    if (RSP_SIGNED(r19) > 0) {
        // addi        $20, $20, 0x10
        r20 = RSP_ADD32(r20, 0X10);
        goto L_1800;
    }
    // addi        $20, $20, 0x10
    r20 = RSP_ADD32(r20, 0X10);
    // addi        $1, $20, -0x8
    r1 = RSP_ADD32(r20, -0X8);
    // addi        $2, $18, 0x0
    r2 = RSP_ADD32(r18, 0X0);
    // jal         0x11B0
    r31 = 0x1868;
    // addi        $3, $zero, 0x7
    r3 = RSP_ADD32(0, 0X7);
    goto L_11B0;
    // addi        $3, $zero, 0x7
    r3 = RSP_ADD32(0, 0X7);
L_1868:
    // mfc0        $5, SP_DMA_BUSY
    r5 = 0;
    // bne         $5, $zero, L_1868
    if (r5 != 0) {
        // nop
    
        goto L_1868;
    }
    // nop

L_1874:
    // j           L_1118
    // mtc0        $zero, SP_SEMAPHORE
    goto L_1118;
    // mtc0        $zero, SP_SEMAPHORE
L_187C:
    // lh          $8, 0x0($24)
    r8 = RSP_MEM_H_LOAD(0X0, r24);
    // lh          $19, 0x2($24)
    r19 = RSP_MEM_H_LOAD(0X2, r24);
    // lh          $18, 0x4($24)
    r18 = RSP_MEM_H_LOAD(0X4, r24);
    // lui         $4, 0xFF
    r4 = S32(0XFF << 16);
    // ori         $4, $4, 0xFFFF
    r4 = r4 | 0XFFFF;
    // and         $2, $25, $4
    r2 = r25 & r4;
    // srl         $5, $25, 24
    r5 = S32(U32(r25) >> 24);
    // sll         $5, $5, 2
    r5 = S32(r5) << 2;
    // lw          $6, 0x320($5)
    r6 = RSP_MEM_W_LOAD(0X320, r5);
    // add         $2, $2, $6
    r2 = RSP_ADD32(r2, r6);
    // addi        $1, $23, 0x0
    r1 = RSP_ADD32(r23, 0X0);
    // sw          $2, 0x40($23)
    RSP_MEM_W_STORE(0X40, r23, r2);
    // addi        $3, $zero, 0x1F
    r3 = RSP_ADD32(0, 0X1F);
    // srl         $7, $26, 16
    r7 = S32(U32(r26) >> 16);
    // andi        $10, $7, 0x1
    r10 = r7 & 0X1;
    // bgtz        $10, L_18DC
    if (RSP_SIGNED(r10) > 0) {
        // nop
    
        goto L_18DC;
    }
    // nop

    // jal         0x1184
    r31 = 0x18C8;
    // nop

    goto L_1184;
    // nop

L_18C8:
    // mfc0        $1, SP_DMA_BUSY
    r1 = 0;
    // bne         $1, $zero, L_18C8
    if (r1 != 0) {
        // nop
    
        goto L_18C8;
    }
    // nop

    // j           L_18E8
    // mtc0        $zero, SP_SEMAPHORE
    goto L_18E8;
    // mtc0        $zero, SP_SEMAPHORE
L_18DC:
    // sh          $zero, 0x8($23)
    RSP_MEM_H_STORE(0X8, r23, 0);
    // vxor        $v16, $v16, $v16
    rsp.VXOR<0>(rsp.vpu.r[16], rsp.vpu.r[16], rsp.vpu.r[16]);
    // sdv         $v16[0], 0x0($23)
    rsp.SDV<0>(rsp.vpu.r[16], r23, 0X0);
L_18E8:
    // andi        $10, $7, 0x2
    r10 = r7 & 0X2;
    // beq         $10, $zero, L_1908
    if (r10 == 0) {
        // nop
    
        goto L_1908;
    }
    // nop

    // lh          $11, 0xA($23)
    r11 = RSP_MEM_H_LOAD(0XA, r23);
    // lqv         $v3[0], 0x10($23)
    rsp.LQV<0>(rsp.vpu.r[3], r23, 0X1);
    // sdv         $v3[0], 0x3F0($8)
    rsp.SDV<0>(rsp.vpu.r[3], r8, -0X2);
    // sdv         $v3[8], 0x3F8($8)
    rsp.SDV<8>(rsp.vpu.r[3], r8, -0X1);
    // sub         $8, $8, $11
    r8 = RSP_SUB32(r8, r11);
L_1908:
    // addi        $8, $8, -0x8
    r8 = RSP_ADD32(r8, -0X8);
    // lsv         $v23[14], 0x8($23)
    rsp.LSV<14>(rsp.vpu.r[23], r23, 0X4);
    // ldv         $v16[0], 0x0($23)
    rsp.LDV<0>(rsp.vpu.r[16], r23, 0X0);
    // sdv         $v16[0], 0x0($8)
    rsp.SDV<0>(rsp.vpu.r[16], r8, 0X0);
    // mtc2        $8, $v18[4]
    rsp.MTC2<4>(r8, rsp.vpu.r[18]);
    // addi        $10, $zero, 0xC0
    r10 = RSP_ADD32(0, 0XC0);
    // mtc2        $10, $v18[6]
    rsp.MTC2<6>(r10, rsp.vpu.r[18]);
    // mtc2        $26, $v18[8]
    rsp.MTC2<8>(r26, rsp.vpu.r[18]);
    // addi        $10, $zero, 0x40
    r10 = RSP_ADD32(0, 0X40);
    // mtc2        $10, $v18[10]
    rsp.MTC2<10>(r10, rsp.vpu.r[18]);
    // addi        $9, $zero, 0x40
    r9 = RSP_ADD32(0, 0X40);
    // lqv         $v31[0], 0x10($9)
    rsp.LQV<0>(rsp.vpu.r[31], r9, 0X1);
    // lqv         $v25[0], 0x0($9)
    rsp.LQV<0>(rsp.vpu.r[25], r9, 0X0);
    // vsub        $v25, $v25, $v31
    rsp.VSUB<0>(rsp.vpu.r[25], rsp.vpu.r[25], rsp.vpu.r[31]);
    // lqv         $v30[0], 0x20($9)
    rsp.LQV<0>(rsp.vpu.r[30], r9, 0X2);
    // lqv         $v29[0], 0x30($9)
    rsp.LQV<0>(rsp.vpu.r[29], r9, 0X3);
    // lqv         $v28[0], 0x40($9)
    rsp.LQV<0>(rsp.vpu.r[28], r9, 0X4);
    // lqv         $v27[0], 0x50($9)
    rsp.LQV<0>(rsp.vpu.r[27], r9, 0X5);
    // lqv         $v26[0], 0x60($9)
    rsp.LQV<0>(rsp.vpu.r[26], r9, 0X6);
    // vsub        $v25, $v25, $v31
    rsp.VSUB<0>(rsp.vpu.r[25], rsp.vpu.r[25], rsp.vpu.r[31]);
    // lqv         $v24[0], 0x70($9)
    rsp.LQV<0>(rsp.vpu.r[24], r9, 0X7);
    // addi        $21, $23, 0x20
    r21 = RSP_ADD32(r23, 0X20);
    // addi        $20, $23, 0x30
    r20 = RSP_ADD32(r23, 0X30);
    // vxor        $v22, $v22, $v22
    rsp.VXOR<0>(rsp.vpu.r[22], rsp.vpu.r[22], rsp.vpu.r[22]);
    // vmudm       $v23, $v31, $v23[7]
    rsp.VMUDM<15>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[23]);
    // vmadm       $v22, $v25, $v18[4]
    rsp.VMADM<12>(rsp.vpu.r[22], rsp.vpu.r[25], rsp.vpu.r[18]);
    // vmadn       $v23, $v31, $v30[0]
    rsp.VMADN<8>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[30]);
    // vmudn       $v21, $v31, $v18[2]
    rsp.VMUDN<10>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[18]);
    // vmadn       $v21, $v22, $v30[2]
    rsp.VMADN<10>(rsp.vpu.r[21], rsp.vpu.r[22], rsp.vpu.r[30]);
    // vmudl       $v17, $v23, $v18[5]
    rsp.VMUDL<13>(rsp.vpu.r[17], rsp.vpu.r[23], rsp.vpu.r[18]);
    // vmudn       $v17, $v17, $v30[4]
    rsp.VMUDN<12>(rsp.vpu.r[17], rsp.vpu.r[17], rsp.vpu.r[30]);
    // vmadn       $v17, $v31, $v18[3]
    rsp.VMADN<11>(rsp.vpu.r[17], rsp.vpu.r[31], rsp.vpu.r[18]);
    // lqv         $v25[0], 0x0($9)
    rsp.LQV<0>(rsp.vpu.r[25], r9, 0X0);
    // sqv         $v21[0], 0x0($21)
    rsp.SQV<0>(rsp.vpu.r[21], r21, 0X0);
    // sqv         $v17[0], 0x0($20)
    rsp.SQV<0>(rsp.vpu.r[17], r20, 0X0);
    // ssv         $v23[7], 0x8($23)
    rsp.SSV<7>(rsp.vpu.r[23], r23, 0X4);
    // lh          $17, 0x0($21)
    r17 = RSP_MEM_H_LOAD(0X0, r21);
    // lh          $9, 0x0($20)
    r9 = RSP_MEM_H_LOAD(0X0, r20);
    // lh          $13, 0x8($21)
    r13 = RSP_MEM_H_LOAD(0X8, r21);
    // lh          $5, 0x8($20)
    r5 = RSP_MEM_H_LOAD(0X8, r20);
    // lh          $16, 0x2($21)
    r16 = RSP_MEM_H_LOAD(0X2, r21);
    // lh          $8, 0x2($20)
    r8 = RSP_MEM_H_LOAD(0X2, r20);
    // lh          $12, 0xA($21)
    r12 = RSP_MEM_H_LOAD(0XA, r21);
    // lh          $4, 0xA($20)
    r4 = RSP_MEM_H_LOAD(0XA, r20);
    // lh          $15, 0x4($21)
    r15 = RSP_MEM_H_LOAD(0X4, r21);
    // lh          $7, 0x4($20)
    r7 = RSP_MEM_H_LOAD(0X4, r20);
    // lh          $11, 0xC($21)
    r11 = RSP_MEM_H_LOAD(0XC, r21);
    // lh          $3, 0xC($20)
    r3 = RSP_MEM_H_LOAD(0XC, r20);
    // lh          $14, 0x6($21)
    r14 = RSP_MEM_H_LOAD(0X6, r21);
    // lh          $6, 0x6($20)
    r6 = RSP_MEM_H_LOAD(0X6, r20);
    // lh          $10, 0xE($21)
    r10 = RSP_MEM_H_LOAD(0XE, r21);
    // lh          $2, 0xE($20)
    r2 = RSP_MEM_H_LOAD(0XE, r20);
L_19D8:
    // ldv         $v16[0], 0x0($17)
    rsp.LDV<0>(rsp.vpu.r[16], r17, 0X0);
    // vmudm       $v23, $v31, $v23[7]
    rsp.VMUDM<15>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[23]);
    // ldv         $v15[0], 0x0($9)
    rsp.LDV<0>(rsp.vpu.r[15], r9, 0X0);
    // vmadh       $v23, $v31, $v22[7]
    rsp.VMADH<15>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[22]);
    // ldv         $v16[8], 0x0($13)
    rsp.LDV<8>(rsp.vpu.r[16], r13, 0X0);
    // vmadm       $v22, $v25, $v18[4]
    rsp.VMADM<12>(rsp.vpu.r[22], rsp.vpu.r[25], rsp.vpu.r[18]);
    // ldv         $v15[8], 0x0($5)
    rsp.LDV<8>(rsp.vpu.r[15], r5, 0X0);
    // vmadn       $v23, $v31, $v30[0]
    rsp.VMADN<8>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[30]);
    // ldv         $v14[0], 0x0($16)
    rsp.LDV<0>(rsp.vpu.r[14], r16, 0X0);
    // vmudn       $v21, $v31, $v18[2]
    rsp.VMUDN<10>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[18]);
    // ldv         $v13[0], 0x0($8)
    rsp.LDV<0>(rsp.vpu.r[13], r8, 0X0);
    // vmadn       $v21, $v22, $v30[2]
    rsp.VMADN<10>(rsp.vpu.r[21], rsp.vpu.r[22], rsp.vpu.r[30]);
    // ldv         $v14[8], 0x0($12)
    rsp.LDV<8>(rsp.vpu.r[14], r12, 0X0);
    // vmudl       $v17, $v23, $v18[5]
    rsp.VMUDL<13>(rsp.vpu.r[17], rsp.vpu.r[23], rsp.vpu.r[18]);
    // ldv         $v13[8], 0x0($4)
    rsp.LDV<8>(rsp.vpu.r[13], r4, 0X0);
    // ldv         $v12[0], 0x0($15)
    rsp.LDV<0>(rsp.vpu.r[12], r15, 0X0);
    // ldv         $v11[0], 0x0($7)
    rsp.LDV<0>(rsp.vpu.r[11], r7, 0X0);
    // ldv         $v12[8], 0x0($11)
    rsp.LDV<8>(rsp.vpu.r[12], r11, 0X0);
    // vmudn       $v17, $v17, $v30[4]
    rsp.VMUDN<12>(rsp.vpu.r[17], rsp.vpu.r[17], rsp.vpu.r[30]);
    // ldv         $v11[8], 0x0($3)
    rsp.LDV<8>(rsp.vpu.r[11], r3, 0X0);
    // ldv         $v10[0], 0x0($14)
    rsp.LDV<0>(rsp.vpu.r[10], r14, 0X0);
    // ldv         $v9[0], 0x0($6)
    rsp.LDV<0>(rsp.vpu.r[9], r6, 0X0);
    // vmadn       $v17, $v31, $v18[3]
    rsp.VMADN<11>(rsp.vpu.r[17], rsp.vpu.r[31], rsp.vpu.r[18]);
    // ldv         $v10[8], 0x0($10)
    rsp.LDV<8>(rsp.vpu.r[10], r10, 0X0);
    // vmulf       $v8, $v16, $v15
    rsp.VMULF<0>(rsp.vpu.r[8], rsp.vpu.r[16], rsp.vpu.r[15]);
    // ldv         $v9[8], 0x0($2)
    rsp.LDV<8>(rsp.vpu.r[9], r2, 0X0);
    // vmulf       $v7, $v14, $v13
    rsp.VMULF<0>(rsp.vpu.r[7], rsp.vpu.r[14], rsp.vpu.r[13]);
    // sqv         $v21[0], 0x0($21)
    rsp.SQV<0>(rsp.vpu.r[21], r21, 0X0);
    // vmulf       $v6, $v12, $v11
    rsp.VMULF<0>(rsp.vpu.r[6], rsp.vpu.r[12], rsp.vpu.r[11]);
L_1A4C:
    // sqv         $v17[0], 0x0($20)
    rsp.SQV<0>(rsp.vpu.r[17], r20, 0X0);
    // lh          $17, 0x0($21)
    r17 = RSP_MEM_H_LOAD(0X0, r21);
    // vmulf       $v5, $v10, $v9
    rsp.VMULF<0>(rsp.vpu.r[5], rsp.vpu.r[10], rsp.vpu.r[9]);
    // lh          $9, 0x0($20)
    r9 = RSP_MEM_H_LOAD(0X0, r20);
    // vadd        $v8, $v8, $v8[1q]
    rsp.VADD<3>(rsp.vpu.r[8], rsp.vpu.r[8], rsp.vpu.r[8]);
    // lh          $13, 0x8($21)
    r13 = RSP_MEM_H_LOAD(0X8, r21);
    // vadd        $v7, $v7, $v7[1q]
    rsp.VADD<3>(rsp.vpu.r[7], rsp.vpu.r[7], rsp.vpu.r[7]);
    // lh          $5, 0x8($20)
    r5 = RSP_MEM_H_LOAD(0X8, r20);
    // vadd        $v6, $v6, $v6[1q]
    rsp.VADD<3>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[6]);
    // lh          $16, 0x2($21)
    r16 = RSP_MEM_H_LOAD(0X2, r21);
    // vadd        $v5, $v5, $v5[1q]
    rsp.VADD<3>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[5]);
    // lh          $8, 0x2($20)
    r8 = RSP_MEM_H_LOAD(0X2, r20);
    // vadd        $v8, $v8, $v8[2h]
    rsp.VADD<6>(rsp.vpu.r[8], rsp.vpu.r[8], rsp.vpu.r[8]);
    // lh          $12, 0xA($21)
    r12 = RSP_MEM_H_LOAD(0XA, r21);
    // vadd        $v7, $v7, $v7[2h]
    rsp.VADD<6>(rsp.vpu.r[7], rsp.vpu.r[7], rsp.vpu.r[7]);
    // lh          $4, 0xA($20)
    r4 = RSP_MEM_H_LOAD(0XA, r20);
    // vadd        $v6, $v6, $v6[2h]
    rsp.VADD<6>(rsp.vpu.r[6], rsp.vpu.r[6], rsp.vpu.r[6]);
    // lh          $15, 0x4($21)
    r15 = RSP_MEM_H_LOAD(0X4, r21);
    // vadd        $v5, $v5, $v5[2h]
    rsp.VADD<6>(rsp.vpu.r[5], rsp.vpu.r[5], rsp.vpu.r[5]);
    // lh          $7, 0x4($20)
    r7 = RSP_MEM_H_LOAD(0X4, r20);
    // vmudn       $v4, $v29, $v8[0h]
    rsp.VMUDN<4>(rsp.vpu.r[4], rsp.vpu.r[29], rsp.vpu.r[8]);
    // lh          $11, 0xC($21)
    r11 = RSP_MEM_H_LOAD(0XC, r21);
    // vmadn       $v4, $v28, $v7[0h]
    rsp.VMADN<4>(rsp.vpu.r[4], rsp.vpu.r[28], rsp.vpu.r[7]);
    // lh          $3, 0xC($20)
    r3 = RSP_MEM_H_LOAD(0XC, r20);
    // vmadn       $v4, $v27, $v6[0h]
    rsp.VMADN<4>(rsp.vpu.r[4], rsp.vpu.r[27], rsp.vpu.r[6]);
    // lh          $14, 0x6($21)
    r14 = RSP_MEM_H_LOAD(0X6, r21);
    // vmadn       $v4, $v26, $v5[0h]
    rsp.VMADN<4>(rsp.vpu.r[4], rsp.vpu.r[26], rsp.vpu.r[5]);
    // lh          $6, 0x6($20)
    r6 = RSP_MEM_H_LOAD(0X6, r20);
    // lh          $10, 0xE($21)
    r10 = RSP_MEM_H_LOAD(0XE, r21);
    // addi        $18, $18, -0x10
    r18 = RSP_ADD32(r18, -0X10);
    // sqv         $v4[0], 0x0($19)
    rsp.SQV<0>(rsp.vpu.r[4], r19, 0X0);
    // blez        $18, L_1AD8
    if (RSP_SIGNED(r18) <= 0) {
        // lh          $2, 0xE($20)
        r2 = RSP_MEM_H_LOAD(0XE, r20);
        goto L_1AD8;
    }
    // lh          $2, 0xE($20)
    r2 = RSP_MEM_H_LOAD(0XE, r20);
    // j           L_19D8
    // addi        $19, $19, 0x10
    r19 = RSP_ADD32(r19, 0X10);
    goto L_19D8;
    // addi        $19, $19, 0x10
    r19 = RSP_ADD32(r19, 0X10);
L_1AD8:
    // ssv         $v23[0], 0x8($23)
    rsp.SSV<0>(rsp.vpu.r[23], r23, 0X4);
    // ldv         $v16[0], 0x0($17)
    rsp.LDV<0>(rsp.vpu.r[16], r17, 0X0);
    // sdv         $v16[0], 0x0($23)
    rsp.SDV<0>(rsp.vpu.r[16], r23, 0X0);
    // lh          $6, 0x0($24)
    r6 = RSP_MEM_H_LOAD(0X0, r24);
    // addi        $17, $17, 0x8
    r17 = RSP_ADD32(r17, 0X8);
    // sub         $5, $17, $6
    r5 = RSP_SUB32(r17, r6);
    // andi        $4, $5, 0xF
    r4 = r5 & 0XF;
    // sub         $17, $17, $4
    r17 = RSP_SUB32(r17, r4);
    // beq         $4, $zero, L_1B04
    if (r4 == 0) {
        // addi        $7, $zero, 0x10
        r7 = RSP_ADD32(0, 0X10);
        goto L_1B04;
    }
    // addi        $7, $zero, 0x10
    r7 = RSP_ADD32(0, 0X10);
    // sub         $4, $7, $4
    r4 = RSP_SUB32(r7, r4);
L_1B04:
    // sh          $4, 0xA($23)
    RSP_MEM_H_STORE(0XA, r23, r4);
    // ldv         $v3[0], 0x0($17)
    rsp.LDV<0>(rsp.vpu.r[3], r17, 0X0);
    // ldv         $v3[8], 0x8($17)
    rsp.LDV<8>(rsp.vpu.r[3], r17, 0X1);
    // sqv         $v3[0], 0x10($23)
    rsp.SQV<0>(rsp.vpu.r[3], r23, 0X1);
    // lw          $2, 0x40($23)
    r2 = RSP_MEM_W_LOAD(0X40, r23);
    // addi        $1, $23, 0x0
    r1 = RSP_ADD32(r23, 0X0);
    // jal         0x11B0
    r31 = 0x1B24;
    // addi        $3, $zero, 0x1F
    r3 = RSP_ADD32(0, 0X1F);
    goto L_11B0;
    // addi        $3, $zero, 0x1F
    r3 = RSP_ADD32(0, 0X1F);
L_1B24:
    // mfc0        $5, SP_DMA_BUSY
    r5 = 0;
    // bne         $5, $zero, L_1B24
    if (r5 != 0) {
        // nop
    
        goto L_1B24;
    }
    // nop

    // j           L_1118
    // mtc0        $zero, SP_SEMAPHORE
    goto L_1118;
    // mtc0        $zero, SP_SEMAPHORE
L_1B38:
    // lui         $4, 0xFF
    r4 = S32(0XFF << 16);
    // ori         $4, $4, 0xFFFF
    r4 = r4 | 0XFFFF;
    // and         $2, $25, $4
    r2 = r25 & r4;
    // srl         $5, $25, 24
    r5 = S32(U32(r25) >> 24);
    // sll         $5, $5, 2
    r5 = S32(r5) << 2;
    // lw          $6, 0x320($5)
    r6 = RSP_MEM_W_LOAD(0X320, r5);
    // add         $2, $2, $6
    r2 = RSP_ADD32(r2, r6);
    // addi        $1, $23, 0x0
    r1 = RSP_ADD32(r23, 0X0);
    // addi        $3, $zero, 0x4F
    r3 = RSP_ADD32(0, 0X4F);
    // vxor        $v0, $v0, $v0
    rsp.VXOR<0>(rsp.vpu.r[0], rsp.vpu.r[0], rsp.vpu.r[0]);
    // addi        $11, $zero, 0x40
    r11 = RSP_ADD32(0, 0X40);
    // lqv         $v31[0], 0x10($11)
    rsp.LQV<0>(rsp.vpu.r[31], r11, 0X1);
    // lqv         $v10[0], 0x0($zero)
    rsp.LQV<0>(rsp.vpu.r[10], 0, 0X0);
    // srl         $12, $26, 16
    r12 = S32(U32(r26) >> 16);
    // andi        $10, $12, 0x1
    r10 = r12 & 0X1;
    // beq         $10, $zero, L_1B84
    if (r10 == 0) {
        // lqv         $v24[0], 0x10($24)
        rsp.LQV<0>(rsp.vpu.r[24], r24, 0X1);
        goto L_1B84;
    }
    // lqv         $v24[0], 0x10($24)
    rsp.LQV<0>(rsp.vpu.r[24], r24, 0X1);
    // j           L_1BB0
    // nop

    goto L_1BB0;
    // nop

L_1B84:
    // jal         0x1184
    r31 = 0x1B8C;
    // nop

    goto L_1184;
    // nop

L_1B8C:
    // mfc0        $5, SP_DMA_BUSY
    r5 = 0;
    // bne         $5, $zero, L_1B8C
    if (r5 != 0) {
        // nop
    
        goto L_1B8C;
    }
    // nop

    // mtc0        $zero, SP_SEMAPHORE
    // lqv         $v20[0], 0x0($23)
    rsp.LQV<0>(rsp.vpu.r[20], r23, 0X0);
    // lqv         $v21[0], 0x10($23)
    rsp.LQV<0>(rsp.vpu.r[21], r23, 0X1);
    // lqv         $v18[0], 0x20($23)
    rsp.LQV<0>(rsp.vpu.r[18], r23, 0X2);
    // lqv         $v19[0], 0x30($23)
    rsp.LQV<0>(rsp.vpu.r[19], r23, 0X3);
    // lqv         $v24[0], 0x40($23)
    rsp.LQV<0>(rsp.vpu.r[24], r23, 0X4);
L_1BB0:
    // lh          $13, 0x0($24)
    r13 = RSP_MEM_H_LOAD(0X0, r24);
    // lh          $19, 0x2($24)
    r19 = RSP_MEM_H_LOAD(0X2, r24);
    // lh          $18, 0xA($24)
    r18 = RSP_MEM_H_LOAD(0XA, r24);
    // lh          $17, 0xC($24)
    r17 = RSP_MEM_H_LOAD(0XC, r24);
    // lh          $16, 0xE($24)
    r16 = RSP_MEM_H_LOAD(0XE, r24);
    // lh          $14, 0x4($24)
    r14 = RSP_MEM_H_LOAD(0X4, r24);
    // addi        $15, $zero, 0x10
    r15 = RSP_ADD32(0, 0X10);
    // mfc2        $21, $v24[2]
    rsp.MFC2<2>(r21, rsp.vpu.r[24]);
    // mfc2        $20, $v24[8]
    rsp.MFC2<8>(r20, rsp.vpu.r[24]);
    // andi        $9, $12, 0x8
    r9 = r12 & 0X8;
    // bgtz        $9, L_1BEC
    if (RSP_SIGNED(r9) > 0) {
        // nop
    
        goto L_1BEC;
    }
    // nop

    // addi        $17, $23, 0x50
    r17 = RSP_ADD32(r23, 0X50);
    // add         $16, $zero, $17
    r16 = RSP_ADD32(0, r17);
    // addi        $15, $zero, 0x0
    r15 = RSP_ADD32(0, 0X0);
L_1BEC:
    // beq         $10, $zero, L_1CF0
    if (r10 == 0) {
        // lqv         $v30[0], 0x70($11)
        rsp.LQV<0>(rsp.vpu.r[30], r11, 0X7);
        goto L_1CF0;
    }
    // lqv         $v30[0], 0x70($11)
    rsp.LQV<0>(rsp.vpu.r[30], r11, 0X7);
    // lqv         $v17[0], 0x0($13)
    rsp.LQV<0>(rsp.vpu.r[17], r13, 0X0);
L_1BF8:
    // lqv         $v29[0], 0x0($19)
    rsp.LQV<0>(rsp.vpu.r[29], r19, 0X0);
    // lqv         $v27[0], 0x0($17)
    rsp.LQV<0>(rsp.vpu.r[27], r17, 0X0);
    // vxor        $v21, $v21, $v21
    rsp.VXOR<0>(rsp.vpu.r[21], rsp.vpu.r[21], rsp.vpu.r[21]);
    // lsv         $v20[14], 0x6($24)
    rsp.LSV<14>(rsp.vpu.r[20], r24, 0X3);
    // vmudm       $v23, $v20, $v24[2]
    rsp.VMUDM<10>(rsp.vpu.r[23], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmadh       $v22, $v20, $v24[1]
    rsp.VMADH<9>(rsp.vpu.r[22], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmadn       $v23, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[0]);
    // vsubc       $v23, $v23, $v21
    rsp.VSUBC<0>(rsp.vpu.r[23], rsp.vpu.r[23], rsp.vpu.r[21]);
    // vsub        $v22, $v22, $v20
    rsp.VSUB<0>(rsp.vpu.r[22], rsp.vpu.r[22], rsp.vpu.r[20]);
    // vmudl       $v23, $v30, $v23[7]
    rsp.VMUDL<15>(rsp.vpu.r[23], rsp.vpu.r[30], rsp.vpu.r[23]);
    // vmadn       $v23, $v30, $v22[7]
    rsp.VMADN<15>(rsp.vpu.r[23], rsp.vpu.r[30], rsp.vpu.r[22]);
    // vmadm       $v22, $v31, $v0[0]
    rsp.VMADM<8>(rsp.vpu.r[22], rsp.vpu.r[31], rsp.vpu.r[0]);
    // vmadm       $v21, $v31, $v21[7]
    rsp.VMADM<15>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[21]);
    // vmadh       $v20, $v31, $v20[7]
    rsp.VMADH<15>(rsp.vpu.r[20], rsp.vpu.r[31], rsp.vpu.r[20]);
    // bgtz        $21, L_1C44
    if (RSP_SIGNED(r21) > 0) {
        // vmadn       $v21, $v31, $v0[0]
        rsp.VMADN<8>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[0]);
        goto L_1C44;
    }
    // vmadn       $v21, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[0]);
    // vge         $v20, $v20, $v24[0]
    rsp.VGE<8>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[24]);
    // j           L_1C48
    // nop

    goto L_1C48;
    // nop

L_1C44:
    // vcl         $v20, $v20, $v24[0]
    rsp.VCL<8>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[24]);
L_1C48:
    // vmulf       $v16, $v20, $v24[6]
    rsp.VMULF<14>(rsp.vpu.r[16], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmulf       $v15, $v20, $v24[7]
    rsp.VMULF<15>(rsp.vpu.r[15], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmulf       $v29, $v29, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[29], rsp.vpu.r[29], rsp.vpu.r[10]);
    // vmacf       $v29, $v17, $v16
    rsp.VMACF<0>(rsp.vpu.r[29], rsp.vpu.r[17], rsp.vpu.r[16]);
    // vmulf       $v27, $v27, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[27], rsp.vpu.r[27], rsp.vpu.r[10]);
    // vmacf       $v27, $v17, $v15
    rsp.VMACF<0>(rsp.vpu.r[27], rsp.vpu.r[17], rsp.vpu.r[15]);
    // sqv         $v29[0], 0x0($19)
    rsp.SQV<0>(rsp.vpu.r[29], r19, 0X0);
    // sqv         $v27[0], 0x0($17)
    rsp.SQV<0>(rsp.vpu.r[27], r17, 0X0);
    // lqv         $v28[0], 0x0($18)
    rsp.LQV<0>(rsp.vpu.r[28], r18, 0X0);
    // lqv         $v26[0], 0x0($16)
    rsp.LQV<0>(rsp.vpu.r[26], r16, 0X0);
    // vxor        $v19, $v19, $v19
    rsp.VXOR<0>(rsp.vpu.r[19], rsp.vpu.r[19], rsp.vpu.r[19]);
    // lsv         $v18[14], 0x8($24)
    rsp.LSV<14>(rsp.vpu.r[18], r24, 0X4);
    // vmudm       $v23, $v18, $v24[5]
    rsp.VMUDM<13>(rsp.vpu.r[23], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmadh       $v22, $v18, $v24[4]
    rsp.VMADH<12>(rsp.vpu.r[22], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmadn       $v23, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[23], rsp.vpu.r[31], rsp.vpu.r[0]);
    // vsubc       $v23, $v23, $v19
    rsp.VSUBC<0>(rsp.vpu.r[23], rsp.vpu.r[23], rsp.vpu.r[19]);
    // vsub        $v22, $v22, $v18
    rsp.VSUB<0>(rsp.vpu.r[22], rsp.vpu.r[22], rsp.vpu.r[18]);
    // vmudl       $v23, $v30, $v23[7]
    rsp.VMUDL<15>(rsp.vpu.r[23], rsp.vpu.r[30], rsp.vpu.r[23]);
    // vmadn       $v23, $v30, $v22[7]
    rsp.VMADN<15>(rsp.vpu.r[23], rsp.vpu.r[30], rsp.vpu.r[22]);
    // vmadm       $v22, $v31, $v0[0]
    rsp.VMADM<8>(rsp.vpu.r[22], rsp.vpu.r[31], rsp.vpu.r[0]);
    // vmadm       $v19, $v31, $v19[7]
    rsp.VMADM<15>(rsp.vpu.r[19], rsp.vpu.r[31], rsp.vpu.r[19]);
    // vmadh       $v18, $v31, $v18[7]
    rsp.VMADH<15>(rsp.vpu.r[18], rsp.vpu.r[31], rsp.vpu.r[18]);
    // bgtz        $20, L_1CB4
    if (RSP_SIGNED(r20) > 0) {
        // vmadn       $v19, $v31, $v0[0]
        rsp.VMADN<8>(rsp.vpu.r[19], rsp.vpu.r[31], rsp.vpu.r[0]);
        goto L_1CB4;
    }
    // vmadn       $v19, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[19], rsp.vpu.r[31], rsp.vpu.r[0]);
    // vge         $v18, $v18, $v24[3]
    rsp.VGE<11>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[24]);
    // j           L_1CB8
    // nop

    goto L_1CB8;
    // nop

L_1CB4:
    // vcl         $v18, $v18, $v24[3]
    rsp.VCL<11>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[24]);
L_1CB8:
    // vmulf       $v16, $v18, $v24[6]
    rsp.VMULF<14>(rsp.vpu.r[16], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmulf       $v15, $v18, $v24[7]
    rsp.VMULF<15>(rsp.vpu.r[15], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmulf       $v28, $v28, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[28], rsp.vpu.r[28], rsp.vpu.r[10]);
    // vmacf       $v28, $v17, $v16
    rsp.VMACF<0>(rsp.vpu.r[28], rsp.vpu.r[17], rsp.vpu.r[16]);
    // vmulf       $v26, $v26, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[26], rsp.vpu.r[26], rsp.vpu.r[10]);
    // vmacf       $v26, $v17, $v15
    rsp.VMACF<0>(rsp.vpu.r[26], rsp.vpu.r[17], rsp.vpu.r[15]);
    // sqv         $v28[0], 0x0($18)
    rsp.SQV<0>(rsp.vpu.r[28], r18, 0X0);
    // sqv         $v26[0], 0x0($16)
    rsp.SQV<0>(rsp.vpu.r[26], r16, 0X0);
    // addi        $14, $14, -0x10
    r14 = RSP_ADD32(r14, -0X10);
    // addi        $13, $13, 0x10
    r13 = RSP_ADD32(r13, 0X10);
    // addi        $19, $19, 0x10
    r19 = RSP_ADD32(r19, 0X10);
    // addi        $18, $18, 0x10
    r18 = RSP_ADD32(r18, 0X10);
    // add         $17, $17, $15
    r17 = RSP_ADD32(r17, r15);
    // add         $16, $16, $15
    r16 = RSP_ADD32(r16, r15);
L_1CF0:
    // vmudl       $v23, $v21, $v24[2]
    rsp.VMUDL<10>(rsp.vpu.r[23], rsp.vpu.r[21], rsp.vpu.r[24]);
    // vmadm       $v23, $v20, $v24[2]
    rsp.VMADM<10>(rsp.vpu.r[23], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmadn       $v23, $v21, $v24[1]
    rsp.VMADN<9>(rsp.vpu.r[23], rsp.vpu.r[21], rsp.vpu.r[24]);
    // vmadh       $v20, $v20, $v24[1]
    rsp.VMADH<9>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmadn       $v21, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[0]);
L_1D04:
    // bgtz        $21, L_1D30
    if (RSP_SIGNED(r21) > 0) {
        // lqv         $v17[0], 0x0($13)
        rsp.LQV<0>(rsp.vpu.r[17], r13, 0X0);
        goto L_1D30;
    }
    // lqv         $v17[0], 0x0($13)
    rsp.LQV<0>(rsp.vpu.r[17], r13, 0X0);
    // vge         $v20, $v20, $v24[0]
    rsp.VGE<8>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmudl       $v23, $v19, $v24[5]
    rsp.VMUDL<13>(rsp.vpu.r[23], rsp.vpu.r[19], rsp.vpu.r[24]);
    // vmadm       $v23, $v18, $v24[5]
    rsp.VMADM<13>(rsp.vpu.r[23], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmadn       $v23, $v19, $v24[4]
    rsp.VMADN<12>(rsp.vpu.r[23], rsp.vpu.r[19], rsp.vpu.r[24]);
    // lqv         $v29[0], 0x0($19)
    rsp.LQV<0>(rsp.vpu.r[29], r19, 0X0);
    // vmadh       $v18, $v18, $v24[4]
    rsp.VMADH<12>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[24]);
    // lqv         $v27[0], 0x0($17)
    rsp.LQV<0>(rsp.vpu.r[27], r17, 0X0);
    // j           L_1D50
    // vmadn       $v19, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[19], rsp.vpu.r[31], rsp.vpu.r[0]);
    goto L_1D50;
    // vmadn       $v19, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[19], rsp.vpu.r[31], rsp.vpu.r[0]);
L_1D30:
    // vcl         $v20, $v20, $v24[0]
    rsp.VCL<8>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmudl       $v23, $v19, $v24[5]
    rsp.VMUDL<13>(rsp.vpu.r[23], rsp.vpu.r[19], rsp.vpu.r[24]);
    // vmadm       $v23, $v18, $v24[5]
    rsp.VMADM<13>(rsp.vpu.r[23], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmadn       $v23, $v19, $v24[4]
    rsp.VMADN<12>(rsp.vpu.r[23], rsp.vpu.r[19], rsp.vpu.r[24]);
    // lqv         $v29[0], 0x0($19)
    rsp.LQV<0>(rsp.vpu.r[29], r19, 0X0);
    // vmadh       $v18, $v18, $v24[4]
    rsp.VMADH<12>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[24]);
    // lqv         $v27[0], 0x0($17)
    rsp.LQV<0>(rsp.vpu.r[27], r17, 0X0);
    // vmadn       $v19, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[19], rsp.vpu.r[31], rsp.vpu.r[0]);
L_1D50:
    // vmulf       $v16, $v20, $v24[6]
    rsp.VMULF<14>(rsp.vpu.r[16], rsp.vpu.r[20], rsp.vpu.r[24]);
    // sqv         $v20[0], 0x0($23)
    rsp.SQV<0>(rsp.vpu.r[20], r23, 0X0);
    // vmulf       $v15, $v20, $v24[7]
    rsp.VMULF<15>(rsp.vpu.r[15], rsp.vpu.r[20], rsp.vpu.r[24]);
    // sqv         $v21[0], 0x10($23)
    rsp.SQV<0>(rsp.vpu.r[21], r23, 0X1);
    // vmulf       $v29, $v29, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[29], rsp.vpu.r[29], rsp.vpu.r[10]);
    // vmacf       $v29, $v17, $v16
    rsp.VMACF<0>(rsp.vpu.r[29], rsp.vpu.r[17], rsp.vpu.r[16]);
    // lqv         $v28[0], 0x0($18)
    rsp.LQV<0>(rsp.vpu.r[28], r18, 0X0);
L_1D6C:
    // vmulf       $v27, $v27, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[27], rsp.vpu.r[27], rsp.vpu.r[10]);
    // lqv         $v26[0], 0x0($16)
    rsp.LQV<0>(rsp.vpu.r[26], r16, 0X0);
    // vmacf       $v27, $v17, $v15
    rsp.VMACF<0>(rsp.vpu.r[27], rsp.vpu.r[17], rsp.vpu.r[15]);
    // bgtz        $20, L_1DA0
    if (RSP_SIGNED(r20) > 0) {
        // sqv         $v29[0], 0x0($19)
        rsp.SQV<0>(rsp.vpu.r[29], r19, 0X0);
        goto L_1DA0;
    }
    // sqv         $v29[0], 0x0($19)
    rsp.SQV<0>(rsp.vpu.r[29], r19, 0X0);
    // vge         $v18, $v18, $v24[3]
    rsp.VGE<11>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmudl       $v23, $v21, $v24[2]
    rsp.VMUDL<10>(rsp.vpu.r[23], rsp.vpu.r[21], rsp.vpu.r[24]);
    // sqv         $v27[0], 0x0($17)
    rsp.SQV<0>(rsp.vpu.r[27], r17, 0X0);
    // vmadm       $v23, $v20, $v24[2]
    rsp.VMADM<10>(rsp.vpu.r[23], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmadn       $v23, $v21, $v24[1]
    rsp.VMADN<9>(rsp.vpu.r[23], rsp.vpu.r[21], rsp.vpu.r[24]);
    // vmadh       $v20, $v20, $v24[1]
    rsp.VMADH<9>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[24]);
    // j           L_1DBC
    // vmadn       $v21, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[0]);
    goto L_1DBC;
    // vmadn       $v21, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[0]);
L_1DA0:
    // vcl         $v18, $v18, $v24[3]
    rsp.VCL<11>(rsp.vpu.r[18], rsp.vpu.r[18], rsp.vpu.r[24]);
    // vmudl       $v23, $v21, $v24[2]
    rsp.VMUDL<10>(rsp.vpu.r[23], rsp.vpu.r[21], rsp.vpu.r[24]);
    // sqv         $v27[0], 0x0($17)
    rsp.SQV<0>(rsp.vpu.r[27], r17, 0X0);
    // vmadm       $v23, $v20, $v24[2]
    rsp.VMADM<10>(rsp.vpu.r[23], rsp.vpu.r[20], rsp.vpu.r[24]);
L_1DB0:
    // vmadn       $v23, $v21, $v24[1]
    rsp.VMADN<9>(rsp.vpu.r[23], rsp.vpu.r[21], rsp.vpu.r[24]);
    // vmadh       $v20, $v20, $v24[1]
    rsp.VMADH<9>(rsp.vpu.r[20], rsp.vpu.r[20], rsp.vpu.r[24]);
    // vmadn       $v21, $v31, $v0[0]
    rsp.VMADN<8>(rsp.vpu.r[21], rsp.vpu.r[31], rsp.vpu.r[0]);
L_1DBC:
    // vmulf       $v16, $v18, $v24[6]
    rsp.VMULF<14>(rsp.vpu.r[16], rsp.vpu.r[18], rsp.vpu.r[24]);
    // addi        $14, $14, -0x10
    r14 = RSP_ADD32(r14, -0X10);
    // vmulf       $v15, $v18, $v24[7]
    rsp.VMULF<15>(rsp.vpu.r[15], rsp.vpu.r[18], rsp.vpu.r[24]);
    // addi        $19, $19, 0x10
    r19 = RSP_ADD32(r19, 0X10);
    // vmulf       $v28, $v28, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[28], rsp.vpu.r[28], rsp.vpu.r[10]);
    // add         $17, $17, $15
    r17 = RSP_ADD32(r17, r15);
    // vmacf       $v28, $v17, $v16
    rsp.VMACF<0>(rsp.vpu.r[28], rsp.vpu.r[17], rsp.vpu.r[16]);
    // addi        $13, $13, 0x10
    r13 = RSP_ADD32(r13, 0X10);
    // vmulf       $v26, $v26, $v10[6]
    rsp.VMULF<14>(rsp.vpu.r[26], rsp.vpu.r[26], rsp.vpu.r[10]);
    // vmacf       $v26, $v17, $v15
    rsp.VMACF<0>(rsp.vpu.r[26], rsp.vpu.r[17], rsp.vpu.r[15]);
    // sqv         $v28[0], 0x0($18)
    rsp.SQV<0>(rsp.vpu.r[28], r18, 0X0);
    // addi        $18, $18, 0x10
    r18 = RSP_ADD32(r18, 0X10);
    // blez        $14, L_1DFC
    if (RSP_SIGNED(r14) <= 0) {
        // sqv         $v26[0], 0x0($16)
        rsp.SQV<0>(rsp.vpu.r[26], r16, 0X0);
        goto L_1DFC;
    }
    // sqv         $v26[0], 0x0($16)
    rsp.SQV<0>(rsp.vpu.r[26], r16, 0X0);
    // j           L_1D04
    // add         $16, $16, $15
    r16 = RSP_ADD32(r16, r15);
    goto L_1D04;
    // add         $16, $16, $15
    r16 = RSP_ADD32(r16, r15);
L_1DFC:
    // sqv         $v18[0], 0x20($23)
    rsp.SQV<0>(rsp.vpu.r[18], r23, 0X2);
L_1E00:
    // sqv         $v19[0], 0x30($23)
    rsp.SQV<0>(rsp.vpu.r[19], r23, 0X3);
    // sqv         $v24[0], 0x40($23)
    rsp.SQV<0>(rsp.vpu.r[24], r23, 0X4);
    // jal         0x11B0
    r31 = 0x1E10;
    // addi        $3, $zero, 0x4F
    r3 = RSP_ADD32(0, 0X4F);
    goto L_11B0;
    // addi        $3, $zero, 0x4F
    r3 = RSP_ADD32(0, 0X4F);
L_1E10:
    // mfc0        $5, SP_DMA_BUSY
    r5 = 0;
    // bne         $5, $zero, L_1E10
    if (r5 != 0) {
        // nop
    
        goto L_1E10;
    }
    // nop

    // j           L_1118
    // mtc0        $zero, SP_SEMAPHORE
    goto L_1118;
    // mtc0        $zero, SP_SEMAPHORE
L_1E24:
    // lqv         $v31[0], 0x0($zero)
    rsp.LQV<0>(rsp.vpu.r[31], 0, 0X0);
    // lhu         $18, 0x4($24)
    r18 = RSP_MEM_HU_LOAD(0X4, r24);
    // beq         $18, $zero, L_1E94
    if (r18 == 0) {
        // nop
    
        goto L_1E94;
    }
    // nop

    // andi        $19, $25, 0xFFFF
    r19 = r25 & 0XFFFF;
    // addi        $19, $19, 0x5C0
    r19 = RSP_ADD32(r19, 0X5C0);
    // srl         $20, $25, 16
    r20 = S32(U32(r25) >> 16);
    // addi        $20, $20, 0x5C0
    r20 = RSP_ADD32(r20, 0X5C0);
    // andi        $17, $26, 0xFFFF
    r17 = r26 & 0XFFFF;
    // mtc2        $17, $v30[0]
    rsp.MTC2<0>(r17, rsp.vpu.r[30]);
    // lqv         $v27[0], 0x0($19)
    rsp.LQV<0>(rsp.vpu.r[27], r19, 0X0);
    // lqv         $v29[0], 0x0($20)
    rsp.LQV<0>(rsp.vpu.r[29], r20, 0X0);
    // lqv         $v26[0], 0x10($19)
    rsp.LQV<0>(rsp.vpu.r[26], r19, 0X1);
    // lqv         $v28[0], 0x10($20)
    rsp.LQV<0>(rsp.vpu.r[28], r20, 0X1);
L_1E5C:
    // vmulf       $v27, $v27, $v31[6]
    rsp.VMULF<14>(rsp.vpu.r[27], rsp.vpu.r[27], rsp.vpu.r[31]);
    // addi        $18, $18, -0x20
    r18 = RSP_ADD32(r18, -0X20);
    // vmacf       $v27, $v29, $v30[0]
    rsp.VMACF<8>(rsp.vpu.r[27], rsp.vpu.r[29], rsp.vpu.r[30]);
    // addi        $20, $20, 0x20
    r20 = RSP_ADD32(r20, 0X20);
    // sqv         $v27[0], 0x0($19)
    rsp.SQV<0>(rsp.vpu.r[27], r19, 0X0);
    // vmulf       $v26, $v26, $v31[6]
    rsp.VMULF<14>(rsp.vpu.r[26], rsp.vpu.r[26], rsp.vpu.r[31]);
    // lqv         $v29[0], 0x0($20)
    rsp.LQV<0>(rsp.vpu.r[29], r20, 0X0);
    // vmacf       $v26, $v28, $v30[0]
    rsp.VMACF<8>(rsp.vpu.r[26], rsp.vpu.r[28], rsp.vpu.r[30]);
    // lqv         $v28[0], 0x10($20)
    rsp.LQV<0>(rsp.vpu.r[28], r20, 0X1);
    // sqv         $v26[0], 0x10($19)
    rsp.SQV<0>(rsp.vpu.r[26], r19, 0X1);
    // addi        $19, $19, 0x20
    r19 = RSP_ADD32(r19, 0X20);
    // lqv         $v27[0], 0x0($19)
    rsp.LQV<0>(rsp.vpu.r[27], r19, 0X0);
    // bgtz        $18, L_1E5C
    if (RSP_SIGNED(r18) > 0) {
        // lqv         $v26[0], 0x10($19)
        rsp.LQV<0>(rsp.vpu.r[26], r19, 0X1);
        goto L_1E5C;
    }
    // lqv         $v26[0], 0x10($19)
    rsp.LQV<0>(rsp.vpu.r[26], r19, 0X1);
L_1E94:
    // j           L_1118
    // nop

    goto L_1118;
    // nop

    // nop

    // lui         $14, 0xA450
    r14 = S32(0XA450 << 16);
    // jr          $ra
    jump_target = r31;
    debug_file = __FILE__; debug_line = __LINE__;
    // lw          $2, 0x4($14)
    r2 = RSP_MEM_W_LOAD(0X4, r14);
    goto do_indirect_jump;
    // lw          $2, 0x4($14)
    r2 = RSP_MEM_W_LOAD(0X4, r14);
    // nop

    // lui         $14, 0x8014
    r14 = S32(0X8014 << 16);
    // lw          $14, 0x5DA8($14)
    r14 = RSP_MEM_W_LOAD(0X5DA8, r14);
    // .word       0x44844000                   # INVALID     $v0, $v8, $v4[0h] # 00000000 <InstrIdType: RSP_COP1>
    // addiu       $29, $29, -0x10
    r29 = RSP_ADD32(r29, -0X10);
    // .word       0x448E2000                   # INVALID     $v0, $v4, $v14[0h] # 00000000 <InstrIdType: RSP_COP1>
    // .word       0x468042A0                   # INVALID     $v10, $v8, $v0[0h] # 02000020 <InstrIdType: RSP_COP1>
    // bgez        $4, L_1EE0
    if (RSP_SIGNED(r4) >= 0) {
        // .word       0x468021A0                   # INVALID     $v6, $v4, $v0[0h] # 02000020 <InstrIdType: RSP_COP1>
            goto L_1EE0;
    }
    // .word       0x468021A0                   # INVALID     $v6, $v4, $v0[0h] # 02000020 <InstrIdType: RSP_COP1>
    // lui         $1, 0x4F80
    r1 = S32(0X4F80 << 16);
    // .word       0x44818000                   # INVALID     $v0, $v16, $v1[0h] # 00000000 <InstrIdType: RSP_COP1>
    // nop

    // .word       0x46105280                   # INVALID     $v10, $v10, $v16 # 02000000 <InstrIdType: RSP_COP1>
L_1EE0:
    // .word       0x460A3483                   # INVALID     $v18, $v6, $v10 # 02000003 <InstrIdType: RSP_COP1>
    // lui         $1, 0x3F00
    r1 = S32(0X3F00 << 16);
    // .word       0x44812000                   # INVALID     $v0, $v4, $v1[0h] # 00000000 <InstrIdType: RSP_COP1>
    // addiu       $5, $zero, 0x1
    r5 = RSP_ADD32(0, 0X1);
    // .word       0x46049300                   # INVALID     $v12, $v18, $v4 # 02000000 <InstrIdType: RSP_COP1>
    // .word       0x444FF800                   # INVALID     $v0, $v31, $v15[0q] # 00000000 <InstrIdType: RSP_COP1>
    // .word       0x44C5F800                   # INVALID     $v0, $v31, $v5[2h] # 00000000 <InstrIdType: RSP_COP1>
    // nop

    // .word       0x46006224                   # INVALID     $v8, $v12, $v0 # 02000024 <InstrIdType: RSP_COP1>
    // .word       0x4445F800                   # INVALID     $v0, $v31, $v5[0q] # 00000000 <InstrIdType: RSP_COP1>
    // nop

    // andi        $1, $5, 0x4
    r1 = r5 & 0X4;
    // andi        $5, $5, 0x78
    r5 = r5 & 0X78;
    // beq         $5, $zero, L_1F64
    if (r5 == 0) {
        // lui         $1, 0x4F00
        r1 = S32(0X4F00 << 16);
        goto L_1F64;
    }
    // lui         $1, 0x4F00
    r1 = S32(0X4F00 << 16);
    // .word       0x44814000                   # INVALID     $v0, $v8, $v1[0h] # 00000000 <InstrIdType: RSP_COP1>
    // addiu       $5, $zero, 0x1
    r5 = RSP_ADD32(0, 0X1);
    // .word       0x46086201                   # INVALID     $v8, $v12, $v8 # 02000001 <InstrIdType: RSP_COP1>
    // .word       0x44C5F800                   # INVALID     $v0, $v31, $v5[2h] # 00000000 <InstrIdType: RSP_COP1>
    // nop

    // .word       0x46004224                   # INVALID     $v8, $v8, $v0 # 02000024 <InstrIdType: RSP_COP1>
    // .word       0x4445F800                   # INVALID     $v0, $v31, $v5[0q] # 00000000 <InstrIdType: RSP_COP1>
    // nop

    // andi        $1, $5, 0x4
    r1 = r5 & 0X4;
L_1F40:
    // andi        $5, $5, 0x78
    r5 = r5 & 0X78;
    // bne         $5, $zero, L_1F5C
    if (r5 != 0) {
        // nop
    
        goto L_1F5C;
    }
    // nop

    // .word       0x44054000                   # INVALID     $v0, $v8, $v5 # 00000000 <InstrIdType: RSP_COP1>
    // lui         $1, 0x8000
    r1 = S32(0X8000 << 16);
    // b           L_1F74
    // or          $5, $5, $1
    r5 = r5 | r1;
    goto L_1F74;
    // or          $5, $5, $1
    r5 = r5 | r1;
L_1F5C:
    // b           L_1F74
    // addiu       $5, $zero, -0x1
    r5 = RSP_ADD32(0, -0X1);
    goto L_1F74;
    // addiu       $5, $zero, -0x1
    r5 = RSP_ADD32(0, -0X1);
L_1F64:
    // .word       0x44054000                   # INVALID     $v0, $v8, $v5 # 00000000 <InstrIdType: RSP_COP1>
    // nop

    // bltz        $5, L_1F5C
    if (RSP_SIGNED(r5) < 0) {
        // nop
    
        goto L_1F5C;
    }
    // nop

L_1F74:
    // .word       0x44CFF800                   # INVALID     $v0, $v31, $v15[2h] # 00000000 <InstrIdType: RSP_COP1>
    // sltiu       $1, $5, 0x84
    r1 = r5 < 0X84 ? 1 : 0;
    // beq         $1, $zero, L_1F8C
    if (r1 == 0) {
        // nop
    
        goto L_1F8C;
    }
    // nop

    // b           L_0008
    // addiu       $2, $zero, -0x1
    r2 = RSP_ADD32(0, -0X1);
    goto L_0008;
    // addiu       $2, $zero, -0x1
    r2 = RSP_ADD32(0, -0X1);
L_1F8C:
    // addiu       $1, $zero, 0x42
    r1 = RSP_ADD32(0, 0X42);
    // .word       0x00A1001B                   # INVALID     $v0, $v0, $v1[1h] # 00000000 <InstrIdType: RSP_SPECIAL>
    // .word       0x00003012                   # INVALID     $v0, $v6, $v0 # 00000000 <InstrIdType: RSP_SPECIAL>
    // andi        $24, $6, 0xFF
    r24 = r6 & 0XFF;
    // or          $6, $24, $zero
    r6 = r24 | 0;
    // slti        $1, $6, 0x11
    r1 = RSP_SIGNED(r6) < 0X11 ? 1 : 0;
    // bne         $1, $zero, L_1FB0
    if (r1 != 0) {
        // nop
    
        goto L_1FB0;
    }
    // nop

    // addiu       $6, $zero, 0x10
    r6 = RSP_ADD32(0, 0X10);
L_1FB0:
    // addiu       $25, $5, -0x1
    r25 = RSP_ADD32(r5, -0X1);
    // lui         $8, 0xA450
    r8 = S32(0XA450 << 16);
    // sw          $25, 0x10($8)
    RSP_MEM_W_STORE(0X10, r8, r25);
    // addiu       $9, $6, -0x1
    r9 = RSP_ADD32(r6, -0X1);
    // lui         $10, 0xA450
    r10 = S32(0XA450 << 16);
    // sw          $9, 0x14($10)
    RSP_MEM_W_STORE(0X14, r10, r9);
    // addiu       $11, $zero, 0x1
    r11 = RSP_ADD32(0, 0X1);
    // lui         $12, 0xA450
    r12 = S32(0XA450 << 16);
    // sw          $11, 0x8($12)
    RSP_MEM_W_STORE(0X8, r12, r11);
    // lui         $13, 0x8014
    r13 = S32(0X8014 << 16);
    // lw          $13, 0x5DA8($13)
    r13 = RSP_MEM_W_LOAD(0X5DA8, r13);
    // .word       0x01A5001A                   # INVALID     $v0, $v0, $v5[5] # 00000000 <InstrIdType: RSP_SPECIAL>
    // .word       0x00001012                   # INVALID     $v0, $v2, $v0 # 00000000 <InstrIdType: RSP_SPECIAL>
    // bne         $5, $zero, L_1FF0
    if (r5 != 0) {
        // nop
    
        goto L_1FF0;
    }
    // nop

    // break       7
    return RspExitReason::Broke;
L_1FF0:
    // addiu       $1, $zero, -0x1
    r1 = RSP_ADD32(0, -0X1);
    // bne         $5, $1, L_0008
    if (r5 != r1) {
        // lui         $1, 0x8000
        r1 = S32(0X8000 << 16);
        goto L_0008;
    }
    // lui         $1, 0x8000
    r1 = S32(0X8000 << 16);
    // bne         $13, $1, L_0008
    if (r13 != r1) {
            goto L_0008;
    }
L_0008:
    printf("RSP branch to unmapped IMEM address 0x0008 (dead or data word?) in microcode battletanx_audio\n");
    return RspExitReason::UnhandledJumpTarget;
    return RspExitReason::ImemOverrun;
do_indirect_jump:
    switch ((jump_target | 0x1000) & 0X1FFF) { 
        case 0x1080: goto L_1080;
        case 0x1F40: goto L_1F40;
        case 0x1E24: goto L_1E24;
        case 0x1BF8: goto L_1BF8;
        case 0x170C: goto L_170C;
        case 0x148C: goto L_148C;
        case 0x144C: goto L_144C;
        case 0x140C: goto L_140C;
        case 0x1DB0: goto L_1DB0;
        case 0x138C: goto L_138C;
        case 0x1388: goto L_1388;
        case 0x14FC: goto L_14FC;
        case 0x1000: goto L_1000;
        case 0x115C: goto L_115C;
        case 0x12BC: goto L_12BC;
        case 0x1D6C: goto L_1D6C;
        case 0x16F8: goto L_16F8;
        case 0x1280: goto L_1280;
        case 0x1240: goto L_1240;
        case 0x1328: goto L_1328;
        case 0x187C: goto L_187C;
        case 0x1790: goto L_1790;
        case 0x1294: goto L_1294;
        case 0x1178: goto L_1178;
        case 0x1130: goto L_1130;
        case 0x1470: goto L_1470;
        case 0x10C0: goto L_10C0;
        case 0x1868: goto L_1868;
        case 0x10B4: goto L_10B4;
        case 0x1034: goto L_1034;
        case 0x1B38: goto L_1B38;
        case 0x1A4C: goto L_1A4C;
        case 0x12EC: goto L_12EC;
        case 0x1B24: goto L_1B24;
        case 0x179C: goto L_179C;
        case 0x1E10: goto L_1E10;
        case 0x1214: goto L_1214;
        case 0x1E00: goto L_1E00;
        case 0x1118: goto L_1118;
        case 0x11DC: goto L_11DC;
        case 0x1338: goto L_1338;
        case 0x11F0: goto L_11F0;
        case 0x18C8: goto L_18C8;
        case 0x107C: goto L_107C;
        case 0x1B8C: goto L_1B8C;
        case 0x1254: goto L_1254;
        case 0x12D0: goto L_12D0;
    }
    printf("Unhandled jump target 0x%04X in microcode battletanx_audio, coming from [%s:%d]\n", jump_target, debug_file, debug_line);
    printf("Register dump: r0  = %08X r1  = %08X r2  = %08X r3  = %08X r4  = %08X r5  = %08X r6  = %08X r7  = %08X\n"
           "               r8  = %08X r9  = %08X r10 = %08X r11 = %08X r12 = %08X r13 = %08X r14 = %08X r15 = %08X\n"
           "               r16 = %08X r17 = %08X r18 = %08X r19 = %08X r20 = %08X r21 = %08X r22 = %08X r23 = %08X\n"
           "               r24 = %08X r25 = %08X r26 = %08X r27 = %08X r28 = %08X r29 = %08X r30 = %08X r31 = %08X\n",
           0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16,
           r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31);
    return RspExitReason::UnhandledJumpTarget;
do_overlay_swap:
                    ctx->r1 = r1;   ctx->r2 = r2;   ctx->r3 = r3;   ctx->r4 = r4;   ctx->r5 = r5;   ctx->r6 = r6;   ctx->r7 = r7;
    ctx->r8 = r8;   ctx->r9 = r9;   ctx->r10 = r10; ctx->r11 = r11; ctx->r12 = r12; ctx->r13 = r13; ctx->r14 = r14; ctx->r15 = r15;
    ctx->r16 = r16; ctx->r17 = r17; ctx->r18 = r18; ctx->r19 = r19; ctx->r20 = r20; ctx->r21 = r21; ctx->r22 = r22; ctx->r23 = r23;
    ctx->r24 = r24; ctx->r25 = r25; ctx->r26 = r26; ctx->r27 = r27; ctx->r28 = r28; ctx->r29 = r29; ctx->r30 = r30; ctx->r31 = r31;
    ctx->dma_mem_address = dma_mem_address;
    ctx->dma_dram_address = dma_dram_address;
    ctx->jump_target = jump_target;
    ctx->rsp = rsp;
    return RspExitReason::SwapOverlay;
}
