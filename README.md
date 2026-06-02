# Dungeon Brood

Dungeon Brood is an Unreal Engine 5.7 C++ third-person action roguelite prototype about a newly born dungeon monster clearing rooms, consuming biomass, and evolving during a run.

The current repository contains a minimal playable vertical slice. It is intentionally code-first and avoids hand-authored binary assets where possible.

## Requirements

- Unreal Engine 5.7
- Windows for the provided `.bat` launcher scripts
- Standard UE plugins:
  - Gameplay Abilities
  - Enhanced Input

If UE 5.7 is installed outside the default Epic path, set either:

```bat
set UE_ROOT=C:\Path\To\UE_5.7
```

or:

```bat
set UE_5_7_ROOT=C:\Path\To\UE_5.7
```

## Generate Project Files

```bat
GenerateProjectFiles.bat
```

The script checks:

- `UE_ROOT`
- `UE_5_7_ROOT`
- `UE_5_3_ROOT`
- `C:\Program Files\Epic Games\UE_5.7.4`
- `C:\Program Files\Epic Games\UE_5.7`
- `C:\Program Files\Epic Games\UE_5.3`
- Epic Launcher manifests

## Build

```bat
BuildGame.bat
```

This builds the `DungeonBroodEditor Win64 Development` target.

## Run

```bat
RunGame.bat
```

The launcher opens the game in a `1280x720` window and runs:

```bat
UnrealEditor.exe DungeonBrood.uproject -game -log -windowed -ResX=1280 -ResY=720
```

## Smoke Test

```bat
SmokeTestAll.bat
SmokeTestGame.bat
SmokeTestCombat.bat
SmokeTestWavePressure.bat
SmokeTestMouseAim.bat
```

`SmokeTestAll.bat` builds the project and runs the complete suite. The headless smoke tests start the game without rendering or audio. `SmokeTestGame.bat` controls the player pawn through movement, attack, and dodge. `SmokeTestCombat.bat` attacks a real enemy, verifies the biomass drop remains uncollected until pickup, then collects it. `SmokeTestWavePressure.bat` verifies enemies reach and damage a stationary player. `SmokeTestMouseAim.bat` verifies the player can move while aiming in an independent direction and orbit the camera. `SmokeTestWindowed.ps1` validates the dungeon dressing in a real `1280x720` window, sends a real left click attack, drags with the right mouse button, and toggles the ESC menu.

The following console commands are also available:

```text
BroodControlSmokeTest
BroodCombatSmokeTest
BroodWavePressureSmokeTest
BroodMouseAimSmokeTest
BroodRestart
BroodQuit
```

## Controls

- Move: WASD
- Aim: Mouse cursor
- Orbit camera: Hold Right Mouse Button and drag
- Attack: Left Mouse Button or Space
- Dodge: Left Shift
- Choose evolution: 1, 2, 3
- Restart run: R
- Pause menu: Escape

## Current Vertical Slice

- Code-spawned test arena using built-in cube geometry.
- Playable `ABroodCharacter` with movement, camera, melee attack, dodge, stamina, health, biomass, and death state.
- Fixed angled camera with camera-relative WASD movement and independent mouse cursor aim.
- Right mouse drag camera orbit with constrained vertical angle.
- Fab visual paths wired for player, enemies, and dungeon dressing when local packs are present, with code fallback if assets are missing.
- Basic melee trace attack with debug feedback.
- Dodge uses stamina, movement direction, cooldown, and debug logs.
- `ABroodEnemyCharacter` with simple tick-based AI, health, melee damage, death, and biomass rewards.
- Enemies use lightweight AI controllers, move toward the player, and drop collectible biomass pickups on death.
- Enemy archetypes:
  - Melee Grub
  - Spitter, implemented as long-range pseudo-melee placeholder
  - Brute
  - Boss Brute final encounter
- Three normal encounters followed by a final boss brute.
- Evolution choice after each normal encounter using keys 1, 2, 3.
- Evolutions apply real gameplay effects:
  - Hardened Carapace
  - Rending Claws
  - Neural Overdrive
  - Digest Essence
  - Acid Blood
  - Brood Instinct
  - Venom Strike
  - Predator Leap
  - Thick Hide
- On-screen debug feedback for health, stamina, biomass, wave, choices, death, and run completion.

## Known Limitations

- No custom map asset is included yet; the playable arena is spawned from code when the game mode starts.
- No animation dependency is used; combat feedback is logs, debug shapes, and on-screen messages.
- Spitter does not spawn a projectile yet. It uses longer attack range as a placeholder.
- No polished UI yet.
- No inventory, save system, multiplayer, replication, or procedural dungeon.

