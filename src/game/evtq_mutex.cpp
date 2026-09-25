#include <mutex>

#include "recomp.h"

// BattleTanx audio event-queue mutex. The game's libultra audio event queues
// (ALEventQueue, used by func_8011B60C/func_8011B730/func_8011B4F0) rely on
// osSetIntMask(OS_IM_NONE) for mutual exclusion against the audio interrupt. In the
// recomp osSetIntMask is a no-op, so concurrent posts from the DEFRAG thread (playing
// SFX via func_8011CA30/func_8011CBB0) and the AUDIO thread (the mixer
// re-posting the API event) race on the intrusive func_8011C180/func_8011C150 lists - corrupting
// the freeList (silent drops) or creating an allocList cycle (infinite loop in the
// insertion scan, the observed freeze). These exports let RECOMP_PATCHes of the evtq
// functions serialize the queue operations with a real host mutex.
static std::mutex evtq_mutex;

extern "C" void evtq_lock_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    (void)ctx;
    evtq_mutex.lock();
}

extern "C" void evtq_unlock_recomp(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    (void)ctx;
    evtq_mutex.unlock();
}