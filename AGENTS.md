# Agent Notes

## Project

- Unreal version: 5.7
- Project file: `DungeonBrood.uproject`
- Module name: `DungeonBrood`
- Game type: single-player third-person monster-evolution action roguelite
- Unreal MCP source repo lives outside this project at `F:\Programming\unreal-engine-mcp`; do not clone or vendor that repo under `Dungeon-Brood`.
- This project should keep only the imported Unreal plugin at `Plugins/UnrealMCP` plus project-local Codex skills/configuration.

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

If Unreal Engine 5.7 is not in a default Epic install path, use:

```bat
set UE_ROOT=C:\Path\To\UE_5.7
```

or:

```bat
set UE_5_7_ROOT=C:\Path\To\UE_5.7
```

## Coding Rules

- Keep the project C++ first.
- Keep Blueprint extension points clean and minimal.
- Prefer Unreal Engine MCP/editor-first construction for level layout, dungeon architecture, lighting, and asset placement.
- Use the project Unreal MCP skills in `.agents/skills` for MCP workflows; if a documented MCP tool is not exposed in the current Codex session, discover the available `mcp__unreal_mcp` tools and use the closest supported editor workflow.
- Validate level edits in the editor, save them to a real project map, then test that the same content appears when the game runs.
- Keep gameplay logic C++ first; use code-spawned content only for temporary smoke tests, debug scaffolding, or when MCP/editor persistence is blocked.
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

