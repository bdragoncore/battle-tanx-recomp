# Building Guide

This guide will help you build the project on your local machine. The process will require you to provide a decompressed ROM of the US version of the game.

These steps cover: decompressing the ROM, running the recompiler and finally building the project.

## 1. Clone the BattleTanxRecomp Repository
This project makes use of submodules so you will need to clone the repository with the `--recurse-submodules` flag.

```bash
git clone --recurse-submodules
# if you forgot to clone with --recurse-submodules
# cd /path/to/cloned/repo && git submodule update --init --recursive
```

## 2. Install Dependencies

### Linux
For Linux the instructions for Ubuntu are provided, but you can find the equivalent packages for your preferred distro.

```bash
# For Ubuntu, simply run:
sudo apt-get install cmake ninja-build libsdl2-dev libgtk-3-dev lld llvm clang
```

#### Arch Linux (paru)
```bash
# Build toolchain + runtime libraries
paru -S cmake ninja llvm clang lld sdl2-compat freetype2 gtk3

# MIPS cross toolchain (for ROM analysis / the MIPS patches)
paru -S mips64-elf-gcc mips64-elf-binutils mips64-elf-newlib mips-linux-gnu-binutils
```
Notes:
- `llvm` provides `llvm-ar`/`llvm-ranlib` (required by the CMake static-library steps) — a
  bare `clang` install is not enough.
- The `mips64-elf-gcc` AUR package is the full cross compiler; the `mips64-elf-gcc-stage1`
  AUR package is a build-stage-only toolchain and is NOT needed.

### Windows
You will need to install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
In the setup process you'll need to select the following options and tools for installation:
- Desktop development with C++
- C++ Clang Compiler for Windows
- C++ CMake tools for Windows

The other tool necessary will be `make` which can be installe via [Chocolatey](https://chocolatey.org/):
```bash
choco install make
```

## 3. Obtaining the target ROM
You will need the NTSC-U 1.0 N64 BattleTanx ROM (sha1: 535860d941738ac1210c20a9b80114fea0e0ff17). The ROM is already uncompressed, so no decompression step is required.

Copy the ROM to the root of the BattleTanxRecompiled repository with this filename:
- `BattleTanx (USA).z64`

## 4. Generating the C code

Now that you have the required files, you need the N64Recomp binary. Build it from the submodule at `lib/N64ModernRuntime/N64Recomp` (the build instructions are in its README) and copy the `N64Recomp` executable to the root of the BattleTanxRecompiled repository.

After that, go back to the repository root and run:
```bash
./N64Recomp battletanx.us.rev0.toml
```

Everything the game needs from the recompiler is in that toml file: `[[patches.instruction]]` and `[[patches.hook]]` entries (cop0 and eret nops, a divisor check at every division, and the yields the runtime needs) plus the `stubs`, `ignored` and `renamed` lists. `include/btx_recomp_hooks.h`, which CMake force-includes into the generated code, declares what the hooks call. `RecompiledFuncs/` is generated and gitignored, and a tree generated without those entries stalls or crashes, so the build fails with instructions if they are missing.

The recompiled RSP microcode, `rsp/battletanx_audio.cpp` and `rsp/f3dex.cpp`, is checked in, so RSPRecomp is not needed to build.

## 5. Building the Project

Finally, you can build the project! :rocket:

On Windows, you can open the repository folder with Visual Studio, and you'll be able to `[build / run / debug]` the project from there.

If you prefer the command line or you're on a Unix platform you can build the project using CMake:

```bash
cmake -S . -B build-cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -G Ninja -DCMAKE_BUILD_TYPE=Release # or Debug if you want to debug
cmake --build build-cmake --target BattleTanxRecompiled -j$(nproc) --config Release # or Debug
```

This build also compiles the game patches in `patches/` with clang targeting MIPS and links them with lld (`-DPATCHES_C_COMPILER` and `-DPATCHES_LD` select the compiler and linker, defaulting to `clang` and `ld.lld`), recompiles them with N64Recomp and embeds the result in the executable.

## 6. Success

Voilà! You should now have a `BattleTanxRecompiled` executable in the build directory! If you used Visual Studio this will be `out/build/x64-[Configuration]` and if you used the provided CMake commands then this will be `build-cmake`. You will need to run the executable out of the root folder of this project or copy the assets folder to the build folder to run it.
