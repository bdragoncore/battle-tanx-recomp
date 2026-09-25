# BattleTanx: Recompiled

A native PC port of BattleTanx, made by statically recompiling the game with
[N64Recomp](https://github.com/N64Recomp/N64Recomp) and running it on
[N64ModernRuntime](https://github.com/N64Recomp/N64ModernRuntime) with
[RT64](https://github.com/rt64/rt64) for rendering.

Unofficial, and not affiliated with the rights holders. **No game data is
included.** You need your own dump of the game to build or play this.

## Getting started

1. Download the build for your system from [Releases](../../releases).
2. Run it and pick your dump in the launcher.

The dump must be **BattleTanx (USA) 1.0**, SHA-1
`535860d941738ac1210c20a9b80114fea0e0ff17`, as a `.z64` file.

Linux, Windows and macOS builds come out of the project's CI. Only the Linux
build has been tested so far.

## What works

- The game runs, with its audio and save files intact.
- Keyboard and controller input, remappable in the launcher.
- Two controllers are detected separately for the game's 2-player modes.
- The game's cheats can be switched on from the **Cheats** tab in the launcher,
  including level select, instead of entering codes on the Code Entry screen.
- Mods and texture packs through the runtime's mod system.

## Building

See [BUILDING.md](BUILDING.md).

## Credits

- [N64Recomp and N64ModernRuntime](https://github.com/N64Recomp) by Mr-Wiseguy
  and contributors, which this port is built with
- [RT64](https://github.com/rt64/rt64) by Dario and contributors, the renderer
- [RecompFrontend](https://github.com/N64Recomp/RecompFrontend) by the N64Recomp
  contributors, the launcher and input layer
- [PromptFont](https://github.com/Shinmera/promptfont) by Yukari "Shinmera"
  Hafner, for the controller glyphs

## License

The project's own code is GPL-3.0; see [COPYING](COPYING). The executable
contains the game's code, recompiled from a dump, and the game itself remains
the property of its rights holders.
