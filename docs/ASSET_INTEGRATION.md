# Asset integration

## Objective

Slice 3 upgrades the prototype without making the gameplay loop depend on optional binary assets.
The code-spawned dungeon dressing remains the fallback until each imported asset has been validated.

## Selected free Fab packs

### Medieval Dungeon

- Source: https://www.fab.com/listings/c13bd0dc-ac4d-4595-b284-f81386b2e6ef
- Publisher: Infuse Studio
- Use: modular dungeon floor, walls, pillars, crypt pieces and props.
- Fab status checked on 2 June 2026: free Unreal Engine format.

### Paragon: Minions

- Source: https://www.fab.com/listings/039ea035-9360-4e76-ad06-5d3a92da6f65
- Publisher: Epic Games
- Use: first enemy creature meshes, animations and effects.
- Fab status checked on 2 June 2026: free Unreal Engine format.
- Constraint: do not use the `PARAGON` trademark to advertise or name the game.

## Import workflow

1. Add both packs to the Epic/Fab library with the project owner's account.
2. Import `Medieval Dungeon` into `DungeonBrood.uproject`.
3. Import `Paragon: Minions` into `DungeonBrood.uproject`.
4. Commit only the assets actually used by the playable slice.
5. Replace one environment category at a time while keeping the code-spawned fallback.
6. Replace the first enemy archetype with a minion mesh and validate animations.
7. Run `SmokeTestAll.bat` after every integration step.

## Team and repository usage

If the imported `.uasset` files are committed to this project repository, collaborators do not need to add the same Fab products to their personal Fab library just to open, build, test, or package this project.

Fab's Standard License summary allows sharing the asset directly, through a private repository, or inside the project with collaborators who are working on that project.
It also allows distributing the finished project with the assets incorporated into it.
Reference: https://www.fab.com/eula

Do not redistribute either pack as standalone content, as a separate asset archive, or in a public repository whose purpose is to provide the raw pack files.
For public source releases, keep only the assets actually incorporated into the project and re-check the current license text before publishing.

Current repository policy: the full imported `MedievalDungeon` and `ParagonMinions` folders are not committed because the local import is about 7 GB and contains thousands of files. The code contains soft/fallback-style path wiring: if the packs are present locally, visuals load; if they are missing, gameplay falls back to code-spawned placeholders.

Until the project adopts Git LFS, Perforce, or a curated minimal asset commit, collaborators who want the upgraded visuals must add the two Fab products to their own library and import them into the same `Content/MedievalDungeon` and `Content/ParagonMinions` paths.

## Linked asset paths

### Player

- BaseBrood: `/Game/ParagonMinions/Characters/Buff/Buff_White/Meshes/Buff_White`
- BaseBrood idle: `/Game/ParagonMinions/Characters/Buff/Buff_White/Animations/Melee_Idle_A`
- BaseBrood attack: `/Game/ParagonMinions/Characters/Buff/Buff_White/Animations/Melee_Attack_01_A`
- ToxicBrood: `/Game/ParagonMinions/Characters/Buff/Buff_Green/Meshes/Buff_Green`
- ToxicBrood idle: `/Game/ParagonMinions/Characters/Buff/Buff_Green/Animations/Idle`
- InstinctBrood: `/Game/ParagonMinions/Characters/Buff/Buff_Blue/Meshes/Buff_Blue`
- InstinctBrood idle: `/Game/ParagonMinions/Characters/Buff/Buff_Blue/Animations/Idle_A`
- ClawBrood: `/Game/ParagonMinions/Characters/Buff/Buff_Red/Meshes/Buff_Red`
- ClawBrood idle: `/Game/ParagonMinions/Characters/Buff/Buff_Red/Animations/Idle`
- ClawBrood attack: `/Game/ParagonMinions/Characters/Buff/Buff_Red/Animations/Attack_BigSmash`
- ArmoredBrood: `/Game/ParagonMinions/Characters/Buff/Buff_Black/Meshes/Buff_Black`
- ArmoredBrood idle: `/Game/ParagonMinions/Characters/Buff/Buff_Black/Animations/Idle`
- ArmoredBrood attack: `/Game/ParagonMinions/Characters/Buff/Buff_Black/Animations/BiteAttack_A`
- LeapingBrood: `/Game/ParagonMinions/Characters/Buff/Buff_Green/Meshes/Buff_Green`
- LeapingBrood idle: `/Game/ParagonMinions/Characters/Buff/Buff_Green/Animations/Idle`

These player meshes are temporary mutation silhouettes. The final Slice 3 acceptance still requires a better player evolution line or curated variants that read as brood mutation stages.

### Enemies

- Grub: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Melee_Dawn`
- Spitter: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Ranged_Dawn`
- Brute: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Siege_Dawn`
- Boss: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/SK_Minion_Lane_Super`

### World

- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Floor`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Decorative_Arches`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Fire_Pit`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall_Decorative_A`
- `/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin`
- `/Game/MedievalDungeon/Meshes/Props/SM_Coffin_No_Lid`
- `/Game/MedievalDungeon/Meshes/Props/SM_Gargoyle_Statue_On_Stand`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_01`
- `/Game/MedievalDungeon/Meshes/Props/SM_Dirt_Mound_A`

## Acceptance notes

- The project must still boot when optional assets are absent.
- Imported asset paths must be recorded when they are wired into C++ or Blueprint extension points.
- Verify licenses and Fab listing status again before distribution.
