#ifndef __SCHEDULER_FUNCS_INTERNAL_H__
#define __SCHEDULER_FUNCS_INTERNAL_H__

#include "patch_helpers.h"

// Host-side cooperative yield export (manual patch symbol at 0x8F0000D0, provided by
// lib/N64ModernRuntime/librecomp/src/ultra_translation.cpp). Lets a RECOMP_PATCH replace a
// game busy-wait spin with a real scheduler yield - the ecosystem pattern (HarvestMoon64Recomp
// threading.c, GoldenRecomp audio.c) for making a game fit the stock cooperative scheduler.
DECLARE_FUNC(void, yield_self_1ms_recomp);

#endif
