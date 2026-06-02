# Dungeon Brood

Dungeon Brood is an Unreal Engine 5.3 C++ third-person action roguelite prototype about a newly born dungeon monster clearing rooms, consuming biomass, and evolving during a run.

The current repository contains a minimal playable vertical slice. It is intentionally code-first and avoids hand-authored binary assets where possible.

## Requirements

- Unreal Engine 5.3
- Windows for the provided `.bat` launcher scripts
- Standard UE plugins:
  - Gameplay Abilities
  - Enhanced Input

If UE 5.3 is installed outside the default Epic path, set:

```bat
set UE_5_3_ROOT=C:\Path\To\UE_5.3
```

## Generate Project Files

```bat
GenerateProjectFiles.bat
```

The script checks:

- `UE_5_3_ROOT`
- `C:\Program Files\Epic Games\UE_5.3`
- `C:\Program Files\Epic Games\UE_5.3EA`

## Build

```bat
BuildGame.bat
```

This builds the `DungeonBroodEditor Win64 Development` target.

## Run

```bat
RunGame.bat
```

The launcher runs:

```bat
UnrealEditor.exe DungeonBrood.uproject -game -log
```

## Controls

- Move: WASD
- Look: Mouse
- Attack: Left Mouse Button or Space
- Dodge: Left Shift
- Choose evolution: 1, 2, 3
- Restart run: R

## Current Vertical Slice

- Code-spawned test arena using built-in cube geometry.
- Playable `ABroodCharacter` with movement, camera, melee attack, dodge, stamina, health, biomass, and death state.
- Basic melee trace attack with debug feedback.
- Dodge uses stamina, movement direction, cooldown, and debug logs.
- `ABroodEnemyCharacter` with simple tick-based AI, health, melee damage, death, and biomass rewards.
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

