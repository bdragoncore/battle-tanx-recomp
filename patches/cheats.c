#include "patches.h"
#include "misc_funcs.h"

// BattleTanx cheat + level-select support driven by the Recomp UI "Cheats" tab.
//
// The game's own cheat codes (entered on the code-entry screen, validated by
// libc_bgcoll_trace at 0x800DCA7C) are simple byte flags the game reads live and
// never clears. We apply the same flags from the config via
// recomp_get_cheat_enabled(); the level select unlock comes from
// recomp_get_level_select().
//
// Flag map (from the cheat validator's inline handlers, RecompiledFuncs/funcs_15.c):
//   [0x80135760] UNLIMITED AMMO      [0x80135770] RUN STORY
//   [0x80135765] ALL WEAPONS         [0x80135771] TRIPPY
//   [0x80135763] INVULNERABLE        [0x80135772] UNLIMITED LIVES
//   [0x8013576A] INVISIBLE           [0x80135773] STORM RAVENS
//   [0x8013576B] HURL MODE           [0x80135775] FROGS
//   [0x8012579A] STORM RAVENS +      [0x80135776] TOADS
//                CAMPAIGN GANGS
//   [0x80135767] LEVEL SELECT unlock (set by the hidden "TOD CHEAT" code). Read by
//                func_800CBDBC / func_800C7D74 to route the menus to the level select
//                screen (state 8), and by func_800DA9B4 to use the chosen level.
//
// Cheat index order matches the game's cheat code table (skipping the
// unenterable "TOD CHEAT" dummy at index 3):
//   0 UNLIMITED AMMO  1 ALL WEAPONS  2 INVULNERABLE  3 INVISIBLE
//   4 HURL MODE       5 STORM RAVENS 6 RUN STORY     7 TRIPPY
//   8 UNLIMITED LIVES 9 CAMPAIGN GANGS 10 FROGS      11 TOADS

// Also called every frame from the func_80079530 per-frame hook (battletanx_boot.c), so the
// Recomp UI is the single source of truth: toggles take effect next frame and anything set
// through the in-game Code Entry screen is overridden. Only the matcher writes these bytes
// in the original game (the game only reads them), so forcing them every frame is safe.
void battletanx_apply_cheats(void) {
    volatile unsigned char *flags = (volatile unsigned char *)0x80135760;
    volatile unsigned char *campaign = (volatile unsigned char *)0x8012579A;
    flags[0x00] = recomp_get_cheat_enabled(0);  // UNLIMITED AMMO
    flags[0x05] = recomp_get_cheat_enabled(1);  // ALL WEAPONS
    flags[0x03] = recomp_get_cheat_enabled(2);  // INVULNERABLE
    flags[0x0A] = recomp_get_cheat_enabled(3);  // INVISIBLE
    flags[0x0B] = recomp_get_cheat_enabled(4);  // HURL MODE
    *campaign = recomp_get_cheat_enabled(5) || recomp_get_cheat_enabled(9);  // STORM RAVENS / CAMPAIGN GANGS
    flags[0x13] = recomp_get_cheat_enabled(5);  // STORM RAVENS
    flags[0x10] = recomp_get_cheat_enabled(6);  // RUN STORY
    flags[0x11] = recomp_get_cheat_enabled(7);  // TRIPPY
    flags[0x12] = recomp_get_cheat_enabled(8);  // UNLIMITED LIVES
    flags[0x15] = recomp_get_cheat_enabled(10); // FROGS
    flags[0x16] = recomp_get_cheat_enabled(11); // TOADS
    flags[0x07] = recomp_get_level_select();    // LEVEL SELECT
}

// func_8007AA70 (0x8007AA70), the game's init: apply cheats before the boot state machine runs
// so that RUN STORY (read while the first frame is set up, state 0) and STORM RAVENS (read when
// the spawns are assigned) see the flags. The original calls func_8007AB04(),
// func_8007AC20(0) and func_8007C298().
extern void func_8007AB04(void);
extern void func_8007AC20(s32);
extern void func_8007C298(void);
RECOMP_PATCH void func_8007AA70(void) {
    battletanx_apply_cheats();
    func_8007AB04();
    func_8007AC20(0);
    func_8007C298();
}

// func_800D68D0 (0x800D68D0): called from func_800DA9B4 (level select, after the
// player confirms) and from the match start code (right before the level is entered). Re-apply the
// cheats at both sites. Original body replicated
// from RecompiledFuncs/funcs_14.c:15075.
RECOMP_PATCH void func_800D68D0(void) {
    battletanx_apply_cheats();
    *(volatile s32 *)0x803284C4 = 2;
    *(volatile s32 *)0x803284C0 = 2;
    *(volatile s32 *)0x80328400 = 0;
    *(volatile s32 *)0x80328404 = 1;
    *(volatile s32 *)0x801352A4 = 0;
}