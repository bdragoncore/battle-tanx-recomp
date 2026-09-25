#include "ovl_patches.hpp"
#include "../../RecompiledPatches/recomp_overlays.inl"

#include "librecomp/overlays.hpp"
#include "librecomp/game.hpp"

extern "C" {
    extern const char btx_patches_bin[];
    extern const size_t btx_patches_bin_size;
}

void battletanx::register_btx_patches() {
    recomp::overlays::register_patches(btx_patches_bin, btx_patches_bin_size, section_table, ARRLEN(section_table));
    recomp::overlays::register_base_exports(export_table);
    recomp::overlays::register_base_events(event_names);
    recomp::overlays::register_manual_patch_symbols(manual_patch_symbols);
}
