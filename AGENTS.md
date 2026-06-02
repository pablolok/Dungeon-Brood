# Agent Notes

## Project

- Unreal version: 5.3
- Project file: `DungeonBrood.uproject`
- Module name: `DungeonBrood`
- Game type: single-player third-person monster-evolution action roguelite

## Commands

Generate project files:

```bat
GenerateProjectFiles.bat
```

Build:

```bat
BuildGame.bat
```

Run:

```bat
RunGame.bat
```

If Unreal Engine 5.3 is not in a default Epic install path, use:

```bat
set UE_5_3_ROOT=C:\Path\To\UE_5.3
```

## Coding Rules

- Keep the project C++ first.
- Keep Blueprint extension points clean and minimal.
- Prefer code-spawned test content over binary assets until the vertical slice is stable.
- Keep gameplay logic small, readable, and directly tied to the playable loop.
- Use debug messages and logs for early feedback.
- Keep comments focused on extension points and intentional simplifications.

## Do Not Add Yet

- Multiplayer or replication
- Inventory
- Save system
- Large procedural dungeon
- Asset-heavy content
- Marketplace dependencies
- Complex animation requirements
- Large speculative systems not used by the current vertical slice

