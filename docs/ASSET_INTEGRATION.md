# Asset integration

## Objective

Slice 3 upgrades the prototype without making the gameplay loop depend on optional binary assets.
The code-spawned dungeon dressing remains the fallback until each imported asset has been validated.

## Selected free Fab packs

These packs are currently present on this workstation under `Content/MedievalDungeon` and `Content/ParagonMinions`, but the repository still treats them as optional local imports rather than committed source assets.

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

Current Dungeon Level 5 direction update: the starting player no longer uses an imported Paragon player mesh. It uses the code-spawned larva fallback, a dedicated `LarvaGlowLight`, and logs `BROOD_LARVA_FALLBACK_READY` plus `BROOD_LARVA_GLOW_LIGHT_READY`. Paragon `Buff_*` meshes remain temporary post-evolution placeholders only.

### Enemies

- Grub: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Melee_Dawn`
- Spitter: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Ranged_Dawn`
- Brute: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Siege_Dawn`
- Boss: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/SK_Minion_Lane_Super`
- Dungeon Level 5 bone torso dressing: `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage`
- Dungeon Level 5 bone head dressing: `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull`
- Dungeon Level 5 skeleton/archer arm dressing: `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm`
- Dungeon Level 5 brute/guardian weapon dressing: `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_FemurRight`
- Dungeon Level 5 Jailer Husk live body placeholder: `/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/SK_Minion_Lane_Super`
- Dungeon Level 5 Jailer Husk chain dressing: `/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A`, fallback `/Engine/BasicShapes/Cylinder`
- Dungeon Level 5 Jailer Husk fused key ring fallback: `/Engine/BasicShapes/Torus`, fallback `/Engine/BasicShapes/Cylinder`
- Dungeon Level 5 Jailer Husk key teeth fallback: `/Engine/BasicShapes/Cube`
- Dungeon Level 5 abandoned cell front: `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front`
- Dungeon Level 5 abandoned cell side: `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Side`
- Dungeon Level 5 rusty cell door: `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door`
- Dungeon Level 5 hanging chains: `/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A`, `/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B`
- Dungeon Level 5 jailer cage: `/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Open`
- Dungeon Level 5 jailer ball and chain: `/Game/MedievalDungeon/Meshes/Props/SM_Ball_And_Chain`
- Dungeon Level 5 lower stair floor composition: `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor`
- Dungeon Level 5 main modular floor layer: `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor`
- Dungeon Level 5 route half walls: `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall_Half`
- Dungeon Level 5 short route walls: `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/Wall_Short`

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

## Asset search pass - 2 June 2026

The first production level now needs assets that better match `docs/GAME_DESIGN_SPEC.md` and `docs/DEEPBORN_LEVEL_DESIGN.md`: alien larva/evolved alien, Dungeon Level 5 undead/beasts/hazards, Jailer Husk, crypt environment, and surface-village props for later levels. This pass records candidates and rejects; it does not wire new C++ paths because no new pack has been imported and validated locally yet.

Detailed local asset plan for the first production level now lives in `docs/DUNGEON_LEVEL5_ART_AND_ASSET_PLAN.md`.

### Selected next candidates

#### FANTASTIC - Village Pack

- Source: https://www.fab.com/listings/52529a12-e88e-41a0-8834-b87306f20c24
- Publisher: Tidal Flask Studios
- Evidence checked on 2 June 2026:
  - Tidal Flask states the Unreal Engine version is free on Fab and supports UE 4.22+ and UE 5.
  - Fab seller search result lists `FANTASTIC - Village Pack` as `From Free`.
- Intended use: village props, modular walls/fences/spikes, exterior set dressing, crates, food/market clutter.
- Fit: good for a stylized village layer if the project accepts a stylized look.
- Risk: may clash visually with the more realistic `Medieval Dungeon` pack; import only a few props first.
- Integration rule: do not replace the current code-spawned village dressing until specific mesh paths are verified in the local Content Browser.

#### Quantum Modular Character Free Sample

- Source: https://www.fab.com/listings/8e200050-3158-4762-b297-f785b5b1533d
- Publisher: Quantum Assets
- Evidence checked on 2 June 2026: Fab listing shows the sample as free.
- Intended use: possible temporary humanoid rig/sample for soldiers only.
- Fit: weak for medieval villagers; better as a technical retarget/modular-character reference than as final art.
- Risk: modern military look conflicts with the level 1 village fantasy.
- Integration rule: do not wire into enemy paths unless a medieval-compatible visual variant is available locally.

#### Project Titan sample assets

- Source: acquire through Fab/Epic Launcher sample projects.
- Evidence checked on 2 June 2026:
  - Epic documentation says sample projects can be acquired from Fab, the Epic Games Launcher Fab tab, or the Unreal Engine Fab plugin.
  - Epic announced Project Titan would be released as a free sample project.
- Intended use: broad search pool for props, materials, and possibly creature or village-adjacent assets.
- Fit: unknown until inspected locally.
- Risk: very large sample project; do not import wholesale into `DungeonBrood`.
- Integration rule: migrate only individual assets after checking size, license, skeleton compatibility, and visual fit.

### Rejected or not selected yet

#### Medieval Village by Marco Maria Rossi

- Source: https://www.fab.com/listings/f89fe420-1942-4c0e-8707-e7f41dbe3017
- Fit: strong environment candidate with modular village assets.
- Reason not selected: public reports indicate it had a time-limited free period ending 30 December 2025, and the current Fab listing requires license/price confirmation from the project owner's account.

#### Paid/uncertain villager and soldier packs

- Examples found: stylized peasants, medieval cloth/MetaHuman outfits, medieval soldiers, and realistic MetaHuman-compatible soldiers.
- Fit: often visually appropriate.
- Reason not selected: most visible search results were paid, MetaHuman-dependent, or not clearly free. The project should avoid adding paid or heavy character dependencies during this vertical slice.

#### Alien worm / acid creature listings

- Fit: thematically close to alien larva/crawler.
- Reason not selected: available listings found during this pass were not verified as free and may be too large or too specific for a player-evolution line.

### Current asset gap

The key unresolved art gap remains the player evolution line. The code fallback now covers the required larva start, but the project still needs a curated alien asset path for:

- larva start form;
- juvenile predator form;
- mature offensive/venom/armored/leaper variants.

Until a suitable free or custom asset is found, keep the code-spawned larva fallback as the authoritative starting visual and avoid binding gameplay to temporary Paragon player meshes.
