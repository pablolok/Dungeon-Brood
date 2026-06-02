# Dungeon Brood

Dungeon Brood is an Unreal Engine 5 C++ third-person action roguelite prototype focused on single-player monster evolution.

## Current Architecture

- `ABroodCharacter`: third-person character foundation with camera, movement, Enhanced Input hooks, GAS component, basic attack placeholder, and dodge placeholder.
- `UBroodAttributeSet`: Gameplay Ability System attributes for health, stamina, attack power, armor, and movement speed.
- `UEvolutionComponent`: acquired evolution ID tracking with Blueprint-callable query and unlock APIs.
- `UEvolutionDataAsset`: designer-authored evolution data asset with prerequisites, stat modifiers, and optional granted GAS ability.
- `ARoomEncounterManager`: lightweight encounter skeleton that tracks enemies and broadcasts when a room is cleared.

## Scope

This first commit intentionally excludes multiplayer, inventory, and UI implementation. The code exposes clean Blueprint extension points for future gameplay systems.

## Requirements

- Unreal Engine 5.3 or compatible UE5 version
- Gameplay Abilities plugin enabled
- Enhanced Input plugin enabled

## Setup

1. Open `DungeonBrood.uproject` with Unreal Engine.
2. Generate project files if prompted.
3. Build the `DungeonBroodEditor` target.

