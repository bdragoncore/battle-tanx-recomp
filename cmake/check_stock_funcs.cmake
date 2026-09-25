# Build-time check, run as: cmake -DFUNCS_DIR=<RecompiledFuncs dir> -P check_stock_funcs.cmake
#
# On the stock runtime the game only runs if RecompiledFuncs/ was generated with the
# [[patches.hook]] entries in battletanx.us.rev0.toml. A tree generated before they existed, or
# with them removed, builds fine and then stalls after the first GFX task with no error, so fail
# here instead.
file(GLOB _btx_func_files "${FUNCS_DIR}/funcs_2.c" "${FUNCS_DIR}/funcs_*.c")
list(REMOVE_DUPLICATES _btx_func_files)
if(NOT _btx_func_files)
    return()  # Nothing generated yet. The missing sources will be reported elsewhere.
endif()

# The D4 wait in func_8007D850 is normally in funcs_2.c, but check the rest in case functions moved.
set(_btx_found FALSE)
foreach(_f IN LISTS _btx_func_files)
    file(STRINGS "${_f}" _btx_hit REGEX "btx-patch: yield in the D4 wait" LIMIT_COUNT 1)
    if(_btx_hit)
        set(_btx_found TRUE)
        break()
    endif()
endforeach()

if(NOT _btx_found)
    message(FATAL_ERROR
        "RecompiledFuncs/ was generated without the stock-runtime hooks from "
        "battletanx.us.rev0.toml, so the game will stall after the first GFX task.\n"
        "Regenerate it with N64Recomp battletanx.us.rev0.toml and "
        "rebuild.")
endif()

# Every division must carry the divisor check from the same config. Without it a divide by zero
# in the game (ldiv does the division before it tests the divisor) is a host SIGFPE.
set(_btx_div_found FALSE)
foreach(_f IN LISTS _btx_func_files)
    file(STRINGS "${_f}" _btx_hit REGEX "else \\{ lo = 0; hi = 0; \\}" LIMIT_COUNT 1)
    if(_btx_hit)
        set(_btx_div_found TRUE)
        break()
    endif()
endforeach()

if(NOT _btx_div_found)
    message(FATAL_ERROR
        "RecompiledFuncs/ has no divide-by-zero guards, so it was not generated from "
        "battletanx.us.rev0.toml as it stands. A divide by zero in the game would crash the "
        "host.\nRegenerate it with N64Recomp battletanx.us.rev0.toml and rebuild.")
endif()
