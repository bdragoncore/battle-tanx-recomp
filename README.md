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

## Optional generic N64 USB pad profile (Windows)

For the measured transparent-red, three-prong Chinese N64 USB pad, an
[optional SDL profile](docs/controller-profiles/generic-n64-windows.txt) preserves
its physical N64 labels. Check these raw SDL inputs, counting from zero, first:

| Physical control | Raw input |
| --- | --- |
| A / B / Start | buttons 5 / 4 / 9 |
| L / R / Z | buttons 6 / 7 / 8 |
| C up / right / down / left | buttons 0 / 1 / 2 / 3 |
| Stick X / Y | axes 0 / 1 |
| D-pad | hat 0 |

Close the game, back up `recompcontrollerdb.txt` beside
`BattleTanxRecompiled.exe`, append the optional profile's three lines to that
file, and restart. Keep the rest of the database. Use the default controller
bindings in the launcher's Controls page; existing remaps still apply.

This profile is specific to BattleTanx's defaults: physical B maps to SDL X,
L to SDL right-stick click, and R to the right trigger. C buttons map to signed
right-stick directions. Mapping L to SDL left shoulder instead would activate
C Down (change tank / rail controls), rather than L (change viewpoint). Z maps
to the left trigger. There is no measured Select button, so SDL Back is
unassigned; use the keyboard/mouse to access settings and any menu actions
without a matching physical button. Physical B still serves as N64 B.

Only opt in for this layout. The measured unit reports VID/PID `0079:0006`,
four axes, twelve declared buttons and one hat. Incompatible clones can share
these IDs and CRCs; older SDL versions can also fall back across CRCs. Restore
the database backup if any connected pad behaves differently.

The three entries cover Windows identities previously observed on the same unit:
SDL2 2.32.10 (`USB Joystick`, CRC `8630`), SDL2 2.26.3 (`USB Joystick`, `8e4f`)
and SDL3 3.4.18 (`Microntek USB Joystick`, `79be`). No Linux/macOS identity is
inferred. Raw measurements were supplied by MatyMeatBoy with permission to reuse
with attribution: eighteen controls, 206 samples over 10.005 seconds. Native
SDL2 virtual-device checks verify mapping translation; physical gameplay in
BattleTanx and the Windows build itself have not been verified here. The SDL
mapping does not configure a deadzone.

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
