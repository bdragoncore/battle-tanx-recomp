#include "patches.h"
#include <libaudio.h>

// ------------------------------------------------------------------
// BattleTanx audio event-queue thread-safety patches.
//
// The game's libultra audio event queues (ALEventQueue, used by
// func_8011B60C / func_8011B730 / func_8011B4F0) rely on
// osSetIntMask(OS_IM_NONE) for mutual exclusion against the audio
// interrupt. In the recomp osSetIntMask_recomp is a no-op
// (librecomp/src/ultramodern.cpp:207-209), so concurrent posts
// from the DEFRAG thread (playing SFX via func_8011CA30/func_8011CBB0 in
// func_800796F0) and the AUDIO thread (re-posting the
// API event and posting DECAY/STOP/END) race on the
// intrusive func_8011C180/func_8011C150 doubly-linked-list operations.
//
// Two corruption modes:
//   1. freeList corruption -> freeList.next == NULL -> func_8011B60C
//      sees "no free slots", silently drops everything, sounds never
//      play, game logic waiting on func_8011C300 freezes.
//   2. allocList cycle -> the insertion scan at func_8011B60C
//      never terminates -> genuine infinite loop -> both threads
//      "stuck in func_8011B60C" (the observed freeze).
//
// Fix: wrap all evtq list-manipulation code in a host-side mutex
// (std::mutex) via RECOMP_EXPORT manual patch symbols. The patched
// functions are drop-in replacements for the originals, with
// osSetIntMask calls removed (no-ops in the recomp) and
// evtq_lock_recomp / evtq_unlock_recomp calls added.
// ------------------------------------------------------------------

// --- Host-side mutex exports (manual patch symbols) --------------
// Defined in src/game/evtq_mutex.cpp, addresses in syms.ld.
// The audio library's list and copy helpers, which the patched functions call.
extern void func_8011B830(void *src, void *dest, s32 len);
extern void func_8011C150(ALLink *element);
extern void func_8011C180(ALLink *element, ALLink *after);

#include "scheduler_funcs.h"
DECLARE_FUNC(void, evtq_lock_recomp);
DECLARE_FUNC(void, evtq_unlock_recomp);

// --- RECOMP_PATCH: func_8011B60C -------------------------------
RECOMP_PATCH void func_8011B60C(ALEventQueue *evtq, ALEvent *evt, ALMicroTime delta) {
    ALEventListItem *item;
    ALEventListItem *nextItem;
    ALLink *node;
    s32 postAtEnd = 0;

    evtq_lock_recomp();

    item = (ALEventListItem *)evtq->freeList.next;
    if (!item) {
        evtq_unlock_recomp();
        return;
    }

    func_8011C150((ALLink *)item);
    func_8011B830(evt, &item->evt, sizeof(*evt));

    if (delta == AL_EVTQ_END)
        postAtEnd = -1;

    for (node = &evtq->allocList; node != 0; node = node->next) {
        if (!node->next) {
            if (postAtEnd)
                item->delta = 0;
            else
                item->delta = delta;
            func_8011C180((ALLink *)item, node);
            break;
        } else {
            nextItem = (ALEventListItem *)node->next;

            if (delta < nextItem->delta) {
                item->delta = delta;
                nextItem->delta -= delta;
                func_8011C180((ALLink *)item, node);
                break;
            }

            delta -= nextItem->delta;
        }
    }

    evtq_unlock_recomp();
}

// --- RECOMP_PATCH: func_8011B730 -------------------------------
RECOMP_PATCH ALMicroTime func_8011B730(ALEventQueue *evtq, ALEvent *evt) {
    ALEventListItem *item;
    ALMicroTime delta;

    evtq_lock_recomp();

    item = (ALEventListItem *)evtq->allocList.next;

    if (item) {
        func_8011C150((ALLink *)item);
        func_8011B830(&item->evt, evt, sizeof(*evt));
        func_8011C180((ALLink *)item, &evtq->freeList);
        delta = item->delta;
    } else {
        evt->type = -1;
        delta = 0;
    }

    evtq_unlock_recomp();

    return delta;
}

// --- RECOMP_PATCH: func_8011B4F0 -------------------------------
RECOMP_PATCH void func_8011B4F0(ALEventQueue *evtq, s16 type) {
    ALLink *thisNode;
    ALLink *nextNode;
    ALEventListItem *thisItem;
    ALEventListItem *nextItem;

    evtq_lock_recomp();

    thisNode = evtq->allocList.next;
    while (thisNode != 0) {
        nextNode = thisNode->next;
        thisItem = (ALEventListItem *)thisNode;
        nextItem = (ALEventListItem *)nextNode;
        if (thisItem->evt.type == type) {
            if (nextItem)
                nextItem->delta += thisItem->delta;
            func_8011C150(thisNode);
            func_8011C180(thisNode, &evtq->freeList);
        }
        thisNode = nextNode;
    }

    evtq_unlock_recomp();
}
