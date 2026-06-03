# Deepborn - Dungeon Level 5 Art and Asset Plan

## Purpose

This document defines the immediate visual target and asset plan for **Dungeon Level 5: Upper Threshold**. It supports Slice 2 and Slice 3 from `docs/DEEPBORN_VERTICAL_SLICES.md`.

The goal is to make the first playable level look intentional and attractive before building the deeper modular evolution system.

## Production Goal

Dungeon Level 5 must sell the fantasy in the first minute:

- the player is a fragile alien larva;
- the dungeon is old, physical, and close to the surface;
- the upper crypt contains bones, coffins, stairs, candlelight, water/rot mood, and hints of a village above;
- enemies are dungeon creatures or undead, not surface humans;
- gameplay remains readable from the current camera.

## Visual Pillars

### Ancient Threshold

This is not the deepest, strangest dungeon layer yet. It is the upper layer: old crypt stone, mausoleum pieces, broken doors, stairs, cages, candles, bones, and village intrusion props.

### Fragile Alien Intrusion

The player should feel visually wrong in the medieval space. The larva can still be a code fallback, but it needs strong color/material contrast and a low silhouette.

### Readable Combat

The level can be dark, but the player, enemies, biomass pickups, attack direction, and stairs must remain readable at 1280x720.

### Beautiful Before Complex

Use available assets to make one level good. Avoid building five mediocre levels before the first one works visually.

## Local Asset Inventory

Local imported folders found:

- `Content/MedievalDungeon`
- `Content/ParagonMinions`

These are local imports and may not be committed. Code must keep fallback behavior if they are missing.

## Environment Asset Candidates

Use `MedievalDungeon` as the main visual source for Dungeon Level 5.

### Core Architecture

- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Floor`
- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Floor_Concave`
- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Doorway`
- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Wall`
- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Wall_Half`
- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Wall_Decorative_A`
- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Wall_Decorative_B`
- `/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Pillar`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_5`
- `/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_10`

### Crypt Props

- `/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin`
- `/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin_Body`
- `/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin_Lid`
- `/Game/MedievalDungeon/Meshes/Props/SM_Coffin`
- `/Game/MedievalDungeon/Meshes/Props/SM_Coffin_No_Lid`
- `/Game/MedievalDungeon/Meshes/Props/SM_Coffin_Lid`
- `/Game/MedievalDungeon/Meshes/Props/SM_Candle`
- `/Game/MedievalDungeon/Meshes/Props/SM_Candle_Used_A`
- `/Game/MedievalDungeon/Meshes/Props/SM_Candle_Used_B`
- `/Game/MedievalDungeon/Meshes/Props/SM_Candle_Holder`
- `/Game/MedievalDungeon/Meshes/Props/SM_Torch`
- `/Game/MedievalDungeon/Meshes/Props/SM_Chandelier`
- `/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A`
- `/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B`
- `/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage`
- `/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Open`

### Bones and Death Dressing

- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_01`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_02`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_03`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_04`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm`
- `/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Leg`

### Surface Intrusion Props

- `/Game/MedievalDungeon/Meshes/Props/SM_Wood_A`
- `/Game/MedievalDungeon/Meshes/Props/SM_Wood_B`
- `/Game/MedievalDungeon/Meshes/Props/SM_Wood_C`
- `/Game/MedievalDungeon/Meshes/Props/SM_Table`
- `/Game/MedievalDungeon/Meshes/Props/SM_Pot_A_Complete`
- `/Game/MedievalDungeon/Meshes/Props/SM_Stocks_Game_Ready`

These should be used lightly. The first level is still dungeon-first, not village-first.

## Player Asset Direction

### Current State

The current starting player uses a code-spawned larva fallback and logs `BROOD_LARVA_FALLBACK_READY`.

### Short-Term Target

Improve the fallback before chasing a perfect imported mesh:

- lower body silhouette;
- wet organic color;
- clearer mandible/facing marker;
- optional small trailing body segments using primitive meshes;
- optional faint green biomass glow.

### Current Runtime Fallback

The project now uses a small custom modular larva fallback built from simple meshes:

- segmented body;
- front-facing bite/glow marker;
- mandibles, side spines and tail stinger;
- subtle runtime body motion;
- dedicated green `LarvaGlowLight` for readability in the dark dungeon;
- runtime markers `BROOD_LARVA_FALLBACK_READY`, `BROOD_LARVA_GLOW_LIGHT_READY`, `BROOD_LARVA_ALIEN_SILHOUETTE_READY`, and `BROOD_LARVA_ORGANIC_MOTION_READY`.

This is acceptable for the first Dungeon Level 5 pass. A final bespoke alien larva/crawler asset can replace it later if a better free asset is found or a custom model is produced.

## Enemy Asset Direction

### Current Runtime Labels

- `SKELETON`
- `SKELETON ARCHER`
- `ANIMATED REMAINS`
- `JAILER HUSK`

### Short-Term Visual Mapping

Use the current gameplay archetypes, but dress them as Dungeon Level 5 enemies:

- `MeleeGrub` mechanics -> weak skeleton or crawling bones.
- `Spitter` mechanics -> skeleton archer placeholder.
- `Brute` mechanics -> animated remains.
- `BossBrute` mechanics -> Jailer Husk.

### Available Local Enemy Candidates

`MedievalDungeon` has static skeleton and bone meshes. These are not animated characters, but they are useful for:

- enemy dressing;
- death piles;
- Jailer Husk decoration;
- static bone attachments around primitive enemy fallback.

`ParagonMinions` remains useful as temporary animated creature content, but it does not match Dungeon Level 5 undead well. It should not be treated as final art for this level.

## First Integration Pass

### Environment

1. Replace or extend current floor with more crypt-specific modules:
   - `SM_Crypt_Floor`
   - `SM_Crypt_Floor_Concave`
   - `SM_Floor` for the main modular dungeon floor layer.
2. Add clear up/down stair landmarks:
   - lit upward stair using `SM_Stairs_5` or `SM_Stairs_10`;
   - darker downward stair using same mesh with different lighting.
3. Add crypt doorway and wall variation:
   - `SM_Crypt_Doorway`
   - `SM_Crypt_Wall_Decorative_A/B`
4. Add coffins and skeleton piles near room edges.
5. Keep all dressing no-collision unless explicitly needed for gameplay.
6. Add Broken Threshold details from `docs/DEEPBORN_LEVEL_DESIGN.md`:
   - abandoned cells;
   - rusty doors;
   - hanging chains;
   - water pools;
   - roots from the ceiling;
   - darker lower-stair composition.

### Player

1. Improve code fallback with body segments and glow.
2. Keep it independent of optional imported assets.
3. Preserve `BROOD_LARVA_FALLBACK_READY`.

### Enemies

1. Keep current enemy primitive fallback for gameplay.
2. Add optional bone/skeleton static mesh decoration per enemy type if readable.
3. Use code-spawned bone, skull and weapon dressing when final undead animation assets are not available.
4. Add small code-spawned creature accents so the roster begins to suggest rats, slime, bats, and crawling remains before final assets are available.
5. Add simple tick-driven motion to the code-spawned fallback pieces so enemies do not read as static props.
6. Do not attempt complex skeletal retargeting yet.

### Lighting

1. Warm light near upward stair.
2. Sick green/cold light near downward stair and biomass.
3. Keep combat center readable.
4. Avoid over-darkening the player.

## Acceptance Criteria For Slice 2

- Dungeon Level 5 has a clear visual identity.
- Up stair and down stair are readable.
- Player larva reads as alien and low to the ground.
- Enemies read as dungeon undead/creatures, not villagers.
- Biomass pickup remains visible.
- No optional asset is required to boot.
- Any used imported mesh path is documented.
- `BuildGame.bat` passes.
- `SmokeTestAll.bat` passes.
- `SmokeTestWindowed.ps1` saves a real Unreal viewport screenshot to `Saved/Automation/WindowedSmokeViewport.png` for visual review.
- `SmokeTestDungeonLevel5EnvironmentVisual.ps1` saves a gameplay-camera ARPG environment review screenshot to `Saved/Automation/WindowedLevel5EnvironmentViewport.png`.

## Current Editor Map Pass - 3 June 2026

The current `/Game/Deepborn/Maps/L_Dungeon_Level5_Editor` pass moved Level 5 away from the rejected asset-dump layout and back to a layout-first dungeon construction:

- entrance is a wide combat corridor, not a cluttered prop room;
- first progression gate was removed so the opening area can breathe;
- later phase gate now uses `SM_Crypt_Doorway` instead of the oversized dungeon doorway duplication;
- central unsupported ceiling arch and chandelier were removed from the first sightline after screenshot review showed they read as floating/suspended clutter;
- warm lights are only attached to physical torch/candle sources;
- blue light is retained only as low-intensity depth/fill for unlit dungeon space;
- validator now rejects unsupported warm point lights and non-intentional floating pack actors.

Verified:

- `.agents/unreal_scripts/validate_level5_editor_map.py`: passed with 491 pack actors and 18 hidden collision actors.
- `SmokeTestDungeonLevel5EditorFloor.ps1`: passed; player stays above the saved editor map floor.
- `SmokeTestDungeonLevel5EnvironmentVisual.ps1`: passed; review screenshot saved to `Saved/Automation/WindowedLevel5EnvironmentViewport.png`.

Remaining art gap:

- the entrance is now technically coherent and grounded, but still needs a stronger final art pass: better vaulted/arched ceiling solution, less blue dominance in the first frame, tighter torch/candle exposure, and more reference-quality side-wall dressing before it should be called final.
- `SmokeTestDungeonLevel5Visual.ps1` saves the completion screenshot to `Saved/Automation/WindowedLevel5CompleteViewport.png`.
- `SmokeTestDungeonLevel5JailerVisual.ps1` saves a live Jailer Husk screenshot to `Saved/Automation/WindowedJailerHuskViewport.png`.
- `SmokeTestDungeonLevel5JailerAssetReview.ps1` saves a four-angle isolated Jailer Husk asset-review screenshot to `Saved/Automation/WindowedJailerHuskAssetReviewViewport.png`.

## Next Code Tasks

1. Add a `BuildDungeonLevel5Landmarks()` helper to spawn up/down stairs and crypt landmarks. Status: completed.
2. Add additional body segments to the larva fallback. Status: completed.
3. Add optional skeleton/bone dressing for enemies while preserving primitive fallback. Status: completed.
4. Update `SmokeTestWindowed.ps1` to validate a `BROOD_DUNGEON_LEVEL5_ART_READY` marker. Status: completed.
5. Run `BuildGame.bat` and `SmokeTestAll.bat`. Status: completed on 2 June 2026.
6. Add a real viewport screenshot artifact for graphical review. Status: completed on 2 June 2026.
7. Add Jailer Husk completion feedback and an unlocked descent visual toward Dungeon Level 4. Status: completed on 3 June 2026.
8. Hide remaining arena edges and improve the lower-stair exit composition. Status: partially completed on 3 June 2026.
9. Replace the debug-like floor read with a real modular `SM_Floor` layer from the MedievalDungeon pack. Status: completed on 3 June 2026.
10. Add a stronger Jailer Husk-specific visual silhouette, such as fused keys and jailer remains. Status: expanded pass completed on 3 June 2026 with fused key ring, lock-key prop, chain/ring props, skeletal ribcage/skull/arm pieces, hidden Paragon placeholder body, stabilized/grounded boss review capture, and dedicated live-boss screenshot smoke coverage.
11. Shape the room layout into a more deliberate dungeon route instead of a single open arena. Status: first visual pass completed on 3 June 2026.
12. Decide which route walls become gameplay collision after the visual route is stable. Status: first pass completed on 3 June 2026 with selective invisible blockers behind route walls, cells, abandoned cells, and the Jailer chamber; keep monitoring camera occlusion and encounter flow.
13. Reduce first-view camera occlusion after route walls and collision are added. Status: first pass completed on 3 June 2026 with a steeper gameplay camera, shorter spring arm, disabled spring-arm collision against invisible blockers, and `BROOD_DUNGEON_LEVEL5_CAMERA_READABILITY_READY` windowed smoke coverage.
14. Capture a clean first-look screenshot for art review. Status: completed on 3 June 2026 by hiding screen debug messages before `WindowedSmokeViewport.png` and verifying `BROOD_DUNGEON_LEVEL5_CLEAN_FIRST_LOOK_READY`.
15. Remove player nameplate clutter from the first-look art screenshot. Status: completed on 3 June 2026 by hiding player text render components before capture and verifying `BROOD_DUNGEON_LEVEL5_ART_REVIEW_NAMEPLATES_HIDDEN_READY`.
16. Add code-spawned creature variety accents for Dungeon Level 5 enemies. Status: first pass completed on 3 June 2026 with rat/slime/bat-corpse accent components and `BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY` smoke coverage.
17. Reduce washed-out lighting and central wall occlusion after screenshot review. Status: completed on 3 June 2026 by adding `BuildDungeonLevel5MoodLighting()`, switching central route walls to low-profile `SM_Wall_Floor_Trim` / `SM_Edge_Stones` assets, and verifying `BROOD_DUNGEON_LEVEL5_MOOD_LIGHTING_READY` plus `BROOD_DUNGEON_LEVEL5_LOW_PROFILE_ROUTE_READY`.
18. Strengthen first-look enemy silhouette without adding binary assets. Status: completed on 3 June 2026 by enlarging high-contrast code-spawned enemy accents, moving first-wave enemies slightly closer, adding readable center/enemy lights, and verifying `BROOD_DUNGEON_LEVEL5_ENEMY_SILHOUETTE_READY` plus `BROOD_DUNGEON_LEVEL5_READABLE_DARKNESS_READY`.
19. Clean up the Jailer Husk boss review screenshot. Status: completed on 3 June 2026 by hiding non-boss distractions, grounding the boss at gameplay height, disabling motion blur for the review, hiding the Paragon placeholder body, and verifying `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_STABILIZED_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_GROUNDED_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_DISTRACTIONS_HIDDEN_READY`, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_PLACEHOLDER_BODY_HIDDEN_READY`.
20. Strengthen the starting larva player silhouette. Status: completed on 3 June 2026 by adding code-spawned mandibles, side spines, a tail stinger, subtle organic motion, and verifying `BROOD_LARVA_ALIEN_SILHOUETTE_READY` plus `BROOD_LARVA_ORGANIC_MOTION_READY` in windowed and Level 5 smoke.
21. Add the surface temptation/death branch for Dungeon Level 5. Status: first pass completed on 3 June 2026 with an upper-stair trigger, warning prompt, code-spawned surface guard/archer/knight humanoid pressure placeholders, lethal damage, `THIS_BODY_WAS_NOT_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_HUMANOID_SILHOUETTES_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_STAGED_SILHOUETTES_READY`, `SmokeTestDungeonLevel5SurfaceTrial.bat`, and `SmokeTestDungeonLevel5SurfaceTrialVisual.ps1`. The visual smoke now uses an elevated staged review board so the screenshot clearly communicates guard, archer, and knight pressure. Remaining visual gap: replace the temporary surface attackers/review board with final humanoid assets or bespoke silhouettes.
22. Dress the prologue/spawn chamber. Status: first pass completed on 3 June 2026 by adding a fall scar, dry blood pools, loose skeleton parts, small carcasses, ancient biomass glow, and surface dust/light leak near the larva spawn, with `BROOD_DUNGEON_LEVEL5_PROLOGUE_DRESSING_READY` required by smoke. Screenshot review passed in `Saved/Automation/WindowedSmokeViewport.png`; remaining visual gap is replacing primitive carcasses/blood decals with final mesh or material assets later.
23. Make the first chamber read as a threshold, not an arena. Status: first pass completed on 3 June 2026 by adding warm surface trail slabs, cold descent trail slabs, broken low threshold edging, and explicit up/down choice runes, with `BROOD_DUNGEON_LEVEL5_TRAVERSAL_COMPOSITION_READY` required by smoke. Screenshot review passed in `Saved/Automation/WindowedSmokeViewport.png` after replacing tall edge-stone meshes with flat guide slabs so the larva remains readable.
24. Strengthen the lower-route lead-in to the Jailer Husk. Status: first pass completed on 3 June 2026 by adding `BuildDungeonLevel5JailerApproachDressing()` with a broken jail gate, fallen cell door, floor chains, warning skull/ribcage props, old blood, dead-stone landing, key-rune marker, and `BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY` smoke coverage. Remaining visual gap: review the completion screenshot and tune occlusion/scale if the gate reads too busy at 1280x720.
25. Replace debug-like completion feedback with physical dungeon feedback. Status: expanded on 3 June 2026 by removing completion HUD/text clutter from the art screenshot, hiding completion-only foreground occluders, switching to an in-level angled camera, replacing primitive cone/rune portal feedback with MedievalDungeon rim/open-lock/fallen-key/broken-chain assets, and requiring `BROOD_DUNGEON_LEVEL5_COMPLETION_REVIEW_CAMERA_READY`, `BROOD_DUNGEON_LEVEL5_COMPLETION_ANGLED_OCCLUDERS_HIDDEN_READY`, and `BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED_ASSET_FEEDBACK_READY` in visual smoke. Screenshot review passed in `Saved/Automation/WindowedLevel5CompleteViewport.png`; remaining visual gap is final lighting/composition polish so the lower exit feels less like an art-review crop.
26. Add real pack architecture to the Dungeon Level 4 descent. Status: first pass completed on 3 June 2026 by adding `BuildDungeonLevel5ExitArchitecture()` with `SM_DoorWay`, `SM_Half_Arch`, `SM_Pillar`, `SM_Stairs_5`, open lock, chains, torches, and depth lighting around the lower exit, with `BROOD_DUNGEON_LEVEL5_EXIT_ARCHITECTURE_READY` required by smoke. Screenshot review passed in `Saved/Automation/WindowedLevel5CompleteViewport.png`; remaining visual gap is that top-down review flattens the doorway/arch forms, so the next art-review pass should use a controlled angled camera or a separate no-occlusion showcase.
27. Add an angled lower-exit art-review showcase. Status: expanded on 3 June 2026 by adding `BroodDungeonLevel5ExitShowcaseSmokeTest`, `SmokeTestDungeonLevel5ExitShowcaseVisual.ps1`, and `WindowedLevel5ExitShowcaseViewport.png`. The showcase stages the lower descent with `SM_Floor`, `SM_Stairs_5`, `SM_DoorWay`, `SM_Half_Arch`, `SM_Pillar`, lock, chains, torches, extra landing/floor tiles, back/side walls, ceiling pieces and edge stones so the architecture can be reviewed without foreground occlusion, with `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_STAGED_READY`, `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_ASSET_FLOOR_READY`, `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_CLOSED_FRAME_READY`, and `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_DUNGEON_CLOSED_READY` required by smoke. Remaining visual gap: tune the isolated showcase so it reads more like the in-level descent rather than an art-review set.
28. Add fallback enemy and Jailer motion. Status: first pass completed on 3 June 2026 by replacing cumulative visual rotation with cached base-pose animation in `ABroodEnemyCharacter`, adding bone rattle, crawling lurch, slime pulse, bat/corpse accent flaps, and Jailer key/chain/manacle sway. `BROOD_DUNGEON_LEVEL5_ENEMY_MOTION_READY` is required by Level 5 and windowed smoke, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_CHAIN_MOTION_READY` is required by the Jailer visual smoke. Screenshot review passed for `Saved/Automation/WindowedSmokeViewport.png` and `Saved/Automation/WindowedJailerHuskViewport.png`; remaining visual gap is still final creature-specific authored animation/assets.
29. Replace the Jailer visual smoke technical stage with a dungeon chamber. Status: expanded on 3 June 2026 by moving the live Jailer Husk visual review into the authored in-level boss-room area, hiding foreground approach bars for the screenshot, reducing the oversized boss chain silhouette, and supporting the view with MedievalDungeon `SM_Floor`, `SM_Dungeon_Wall`, `SM_Ceiling_Flat`, `SM_Pillar`, cell, stocks, gibbet, chains, torches, coffin and bone props, with code fallback meshes if those assets are unavailable. `SmokeTestDungeonLevel5JailerVisual.ps1` now requires `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CHAMBER_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PACK_ASSETS_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CEILING_CLOSED_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PRISON_ROOM_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_IN_LEVEL_ROOM_READY`, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_FOREGROUND_BARS_HIDDEN_READY`. Screenshot review passed in `Saved/Automation/WindowedJailerHuskViewport.png`; remaining polish is moving the Jailer focal composition deeper into the boss room so it reads less like a stair/threshold crop and eventually replacing the code-spawned skeletal Jailer with final boss art.
30. Strengthen the Jailer Husk body read. Status: expanded on 3 June 2026 by adding hips, spine/neck, broken legs, fused hand keys, a central chain/robe mass, and reducing the oversized imported chain scale so the boss reads more like a dead jailer body instead of only a floating ribcage or a chain prop. `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VERTICAL_BODY_READY` is required by Level 5 and Jailer visual smoke. Screenshot review passed in `Saved/Automation/WindowedJailerHuskViewport.png`; remaining gap is final authored boss art and a cleaner fully composed boss-room backdrop.
31. Add a creepy spatial dressing pass across the playable route. Status: first pass completed on 3 June 2026 by adding edge-placed hanging chains, dry corpses, warning bones, drag scratches, floor shadows, and cold shadow pockets through `BuildDungeonLevel5CreepyDressing()`. `BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY` is required by Level 5 and windowed smoke. Screenshot review passed in `Saved/Automation/WindowedSmokeViewport.png`: the larva, enemies, and asset floor remain readable while the level edges feel more like a creepy dungeon. Remaining work: review and polish the full route from start to Jailer to exit, not only the first camera.
32. Harden the full-route visual review coverage. Status: completed on 3 June 2026 by replacing the Surface Trial checker review board with MedievalDungeon floor/wall/edge assets, adding more lower-exit floor/wall/ceiling/key coverage, sealing more Jailer review sightlines, and making `SmokeTestAll.bat` run Surface Trial visual, Jailer visual, and Exit showcase visual checks. `Saved/Automation/WindowedSurfaceTrialViewport.png`, `Saved/Automation/WindowedJailerHuskViewport.png`, and `Saved/Automation/WindowedLevel5ExitShowcaseViewport.png` were regenerated after the pass. Remaining work: next pass should make the Jailer chamber and lower exit feel less like isolated review staging and more like finished authored rooms.
33. Move the boss room and lower exit from review staging into the actual playable route. Status: first pass completed on 3 June 2026 by adding `BuildDungeonLevel5JailerBossRoomArchitecture()` and `BuildDungeonLevel5ExitDescentRoomArchitecture()`. The Jailer area now gets pack floor, wall, ceiling, stocks, gibbet, ball-chain, hanging chains, bones, torches, and key-light dressing; the lower descent now gets pack floor, side walls, ceiling, crypt doorway, lower stairs, broken lock, chains, bones, and a dark depth well. `BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY` and `BROOD_DUNGEON_LEVEL5_EXIT_DESCENT_ROOM_AUTHORED_READY` are required by smoke. Screenshot review: `WindowedSmokeViewport.png` remains readable, and `WindowedLevel5CompleteViewport.png` now shows the angled in-level lower exit without the previous giant wall/primitive foreground issue. Remaining work: final art polish for lighting, creature assets, and authored boss/exit composition.
34. Add separated visual review modes for environment and boss assets. Status: first pass completed on 3 June 2026 by adding `BroodDungeonLevel5EnvironmentVisualSmokeTest`, `SmokeTestDungeonLevel5EnvironmentVisual.ps1`, `BroodDungeonLevel5JailerAssetReviewSmokeTest`, and `SmokeTestDungeonLevel5JailerAssetReview.ps1`. The environment review uses the gameplay ARPG camera and saves `WindowedLevel5EnvironmentViewport.png`; the boss review isolates four Jailer orientations and saves `WindowedJailerHuskAssetReviewViewport.png`. Remaining work: use these reviews to drive the next art pass, especially stronger first-view enclosure and a final/non-fragmented Jailer Husk asset.
35. Strengthen the first playable view as a dungeon threshold. Status: first pass completed on 3 June 2026 by adding `BuildDungeonLevel5FirstViewArchitecture()` with low side buttresses, pillars, half-arch hints, broken cell doors, hanging chains and warm/cold guide lights. `SmokeTestDungeonLevel5EnvironmentVisual.ps1` requires `BROOD_DUNGEON_LEVEL5_FIRST_VIEW_ARCHITECTURE_READY` and regenerates `WindowedLevel5EnvironmentViewport.png`. Screenshot review: the first frame now has clearer side boundaries and less empty-floor dominance while keeping larva/enemies readable. Follow-up tuning rejected `SM_Decorative_Arches` for this first camera because it created oversized foreground clipping; smaller half-arch and low-trim assets are safer here. Remaining work: increase doorway/ceiling silhouette clarity from the ARPG camera without adding foreground occlusion.
36. Add a first-view threshold spine and retune the ARPG environment review camera. Status: first pass completed on 3 June 2026 by adding paired threshold pillars, small `SM_DoorWay` silhouettes, low `SM_Wall_Floor_Trim` ribs and extra warm/cold guide lights, with `BROOD_DUNGEON_LEVEL5_FIRST_VIEW_THRESHOLD_SPINE_READY` required by `SmokeTestDungeonLevel5EnvironmentVisual.ps1`. The review camera now looks further into the threshold and uses a slightly wider FOV so the screenshot evaluates more than the central floor field. `BuildGame.bat` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed, and `WindowedLevel5EnvironmentViewport.png` shows stronger right-side doorway/prison architecture. Remaining work: clean the left/top cut architectural silhouette and remaining technical background strips, then continue full-route review from spawn to Jailer to exit.
37. Clean first-view technical edges. Status: first pass completed on 3 June 2026 by expanding `BuildDungeonLevel5ModularFloor()` to a 9x9 `SM_Floor` grid, removing the first-view half-arch hints that were clipping in the ARPG camera, and reducing the main pier scale so the opening frame keeps enclosure without a giant foreground column. `BuildGame.bat` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed, and `WindowedLevel5EnvironmentViewport.png` shows fewer technical floor/background strips. Remaining work: the left edge still contains a cut vertical silhouette; next pass should solve that specific actor/framing issue before the opening shot is considered polished.
38. Hide first-view edge-only review occluders. Status: first pass completed on 3 June 2026 by making the environment art-review smoke hide known foreground/edge offenders without changing gameplay placement: first-view pier/buttress/chain pieces, the surface dust beam, the first creepy hanging-chain candidate, and the surface crack/root candidates. `SmokeTestDungeonLevel5EnvironmentVisual.ps1` now requires `BROOD_DUNGEON_LEVEL5_ENVIRONMENT_LEFT_EDGE_CLEANUP_READY`; `BuildGame.bat` and the environment visual smoke passed. Remaining work: the frame still contains primitive proxy discs/cubes used for early dressing and read markers, so the next art pass should replace or hide those for the polished environment screenshot.
39. Hide primitive environment proxy clutter in the ARPG review screenshot. Status: first pass completed on 3 June 2026 by hiding prologue/traversal/creepy primitive dressing proxies only in `BroodDungeonLevel5EnvironmentVisualSmokeTest`: dry blood discs, fall-scar cubes, carcass primitives, traversal guide slabs, choice runes, chain-shadow discs, dry-corpse cloth/blood proxies, noose shadow and scratch marks. `SmokeTestDungeonLevel5EnvironmentVisual.ps1` requires `BROOD_DUNGEON_LEVEL5_ENVIRONMENT_PROXY_CLUTTER_HIDDEN_READY`; `BuildGame.bat` and the environment visual smoke passed. Screenshot review: the environment frame is cleaner and now reads more from pack floor and prison architecture. Remaining work: remaining primitive shapes belong mostly to player/enemy fallback components and should be addressed through isolated creature/player asset-review passes.
40. Strengthen the Level 5 threshold as a real corridor instead of an open floor field. Status: first pass verified on 3 June 2026. An MCP prototype with prefix `DB_MCP_L5_*` was staged in the open Unreal Editor using MedievalDungeon floor, walls, doorway, stairs, pillars, half-arches, cell fronts, chains, coffin, skull, gibbet and torches. The layout was then translated into `BuildDungeonLevel5ThresholdCorridorArchitecture()` and required by `SmokeTestDungeonLevel5EnvironmentVisual.ps1` through `BROOD_DUNGEON_LEVEL5_THRESHOLD_CORRIDOR_READY` and `BROOD_DUNGEON_LEVEL5_THRESHOLD_CORRIDOR_CAMERA_CLEAR_READY`. `BuildGame.bat` and the environment visual smoke passed after closing the Live Coding editor lock. Remaining work: the first environment frame is cleaner and more enclosed, but still uses fallback player/enemy primitives and needs final creature/player art before it can be considered beautiful.
41. Clean Jailer and lower-exit art-review staging after MCP restart. Status: first pass verified on 3 June 2026. MCP was reconnected by launching `DungeonBrood.uproject` in UE 5.7 and confirming `PlayerStart` through `mcp__unreal_mcp.find_actors_by_name`. The Jailer live visual smoke now isolates the review chamber from non-review static meshes, pushes side walls outward, adds larger dark backplates/ceiling seals, and uses a clearer centered camera. `SmokeTestDungeonLevel5JailerVisual.ps1` passed and regenerated `WindowedJailerHuskViewport.png`. The lower-exit showcase now hides non-showcase actors/lights, adds dedicated warm/cold showcase lights and larger dark seals, and `SmokeTestDungeonLevel5ExitShowcaseVisual.ps1` passed with `WindowedLevel5ExitShowcaseViewport.png`. Remaining work: both screenshots are usable for review, but the Jailer body is still fallback skeletal kit art and the lower-exit camera remains a tight showcase crop, not final shipped composition.
42. Build visibly in the Unreal Editor through MCP. Status: corrected live editor blockout started on 3 June 2026. The first `DB_MCP_L5_Live_*` attempt failed technically: `spawn_physics_blueprint_actor` produced Blueprint actor icons in the viewport instead of visible layout meshes, so that batch was deleted. The corrected pass uses MCP construction tools that spawn real `StaticMeshActor` geometry with prefix `DB_MCP_L5_Blockout_*`. The current visible blockout contains a floor platform, left/right walls, rear wall segments with a central opening, an arch, corrected Y-axis descent stairs, and simplified side cell bars. A single `DB_MCP_L5_MeshTest_DungeonFloor_*` pack-asset sample was also spawned to check whether MedievalDungeon meshes render through the construction path. Design intent: first make a readable, visible corridor/cell threshold in the Editor, then progressively replace blockout cubes with pack assets and port approved transforms into runtime C++ helpers.
43. Convert Level 5 to a saved editor-authored map. Status: first complete foundation pass verified on 3 June 2026. `L_Dungeon_Level5_Editor` now uses the MedievalDungeon pack directly in the saved map instead of relying on runtime C++ layout spawning: main route, surface temptation landing, broken threshold, boss room, lower descent, walls, pillars, cells, ceilings, coffins, chains, bones, torches and hidden gameplay collision are authored under `DB_L5Pack_*`. Runtime now skips the old environment blockout on this map to avoid duplicate floors, proxy clutter and mismatched collision. Verification passed with direct UE build, visual smoke screenshot, `SmokeTestDungeonLevel5EditorFloor.ps1`, and `.agents/unreal_scripts/validate_level5_editor_map.py` reporting 375 pack actors, 14 hidden collision actors and no non-intentional floating pack actors. Remaining work: art review camera and creature fallback visuals still need polish; the environment foundation is now stable enough to iterate visually inside the Editor.
44. Rebuild the entrance as a logical combat corridor. Status: first repair pass verified on 3 June 2026 after screenshot review showed the previous composition still read like misplaced assets. The entrance now has a larger aligned floor footprint for first-wave combat, continuous side-wall collision, a visible surface-stair frame with explicit stair-run meshes, three readable phase gates for future run progression, edge-anchored cells, wall-anchored tombs/coffins, and props placed only by function: surface collapse, prison evidence, phase-gate debris, or Jailer clues. Generated lights are movable to avoid unbuilt-lighting warnings during this editor-first phase. `.agents/unreal_scripts/validate_level5_editor_map.py` passed with 423 pack actors, 18 hidden collision actors and no non-intentional floating pack actors; `SmokeTestDungeonLevel5EditorFloor.ps1` passed.
45. Redirect the first room toward a closed blue crypt-hall reference. Status: repair pass verified on 3 June 2026 after reference review. The entrance is no longer treated as a narrow corridor or loose prison asset set: it now reads as a wider crypt hall with a central combat floor, paired nave pillars, continuous perimeter walls, high ceiling panels, sealed side cells, blue dominant lighting, controlled surface opening, and props only where anchored to walls/cells/gates. Unsupported half-arches were removed, chains moved from floor clutter to a wall attachment, cells now have back/outer seal walls, and ceiling pieces were raised to create vertical volume closer to the supplied dungeon-hall reference. `.agents/unreal_scripts/validate_level5_editor_map.py` passed with 588 pack actors, 18 hidden collision actors and no non-intentional floating pack actors; `SmokeTestDungeonLevel5EditorFloor.ps1` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed.
