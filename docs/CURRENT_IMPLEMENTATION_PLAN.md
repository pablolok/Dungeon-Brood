# Deepborn - Current Implementation Plan

## Purpose

This document tracks how to migrate the current Dungeon Brood C++ prototype toward the new `Deepborn` design in `docs/GAME_DESIGN_SPEC.md`.

The complete vertical-slice production roadmap lives in `docs/DEEPBORN_VERTICAL_SLICES.md`.
The immediate Dungeon Level 5 art and asset plan lives in `docs/DUNGEON_LEVEL5_ART_AND_ASSET_PLAN.md`.
The detailed per-level source for Dungeon Level 5 is `docs/DEEPBORN_LEVEL_DESIGN.md`.

The previous plan proved a useful micro-run: larva fallback, village-crypt dressing, human-facing wave names, evolution choices, and boss smoke coverage. That work remains valuable, but it is now prototype infrastructure rather than the final run structure.

## Current Prototype State

Already implemented and verified:

- third-person player movement, mouse aim, melee attack, dodge, and pause/restart controls;
- code-spawned arena with dungeon dressing and local optional `MedievalDungeon` paths;
- code-spawned village dressing props;
- physical biomass pickups;
- evolution choices after waves;
- player starts with a `LARVA` fallback visual;
- player stage labels progress toward `JUVENILE` and `MATURE BROOD`;
- current enemy nameplates read as `SKELETON`, `SKELETON ARCHER`, `ANIMATED REMAINS`, and `JAILER HUSK`;
- console-driven smoke coverage for combat, pickups, pressure, mouse aim, and one complete Dungeon Level 5 boss run.

Fresh verification previously run:

- `BuildGame.bat`: passed on 2 June 2026.
- `SmokeTestDungeonLevel5.bat`: passed on 2 June 2026 after the Dungeon Level 5 retheme.
- `SmokeTestAll.bat`: passed on 2 June 2026 after the Dungeon Level 5 retheme.
- Dungeon Level 5 art readiness pass completed on 2 June 2026: landmarks, up/down stairs, larva fallback segments, and `BROOD_DUNGEON_LEVEL5_ART_READY` are covered by `BuildGame.bat`, `SmokeTestDungeonLevel5.bat`, and `SmokeTestAll.bat`.
- Dungeon Level 5 enemy dressing and larva readability pass completed on 2 June 2026: `BoneTorsoVisual`, `BoneHeadVisual`, `WeaponVisual`, `LarvaGlowLight`, `BROOD_DUNGEON_LEVEL5_ENEMY_DRESSING_READY`, and `BROOD_LARVA_GLOW_LIGHT_READY` are covered by `BuildGame.bat`, `SmokeTestDungeonLevel5.bat`, `SmokeTestWindowed.ps1`, and `SmokeTestAll.bat`.
- Graphical review artifact: `SmokeTestWindowed.ps1` saves the Unreal viewport screenshot to `Saved/Automation/WindowedSmokeViewport.png`.
- Dungeon Level 5 completion pass completed on 3 June 2026: Jailer Husk, sealed descent, unlocked Dungeon Level 4 exit, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_SEEN`, `BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED`, `BROOD_DUNGEON_LEVEL5_COMPLETE`, and `SmokeTestDungeonLevel5Visual.ps1` are verified.
- Broken Threshold dressing pass completed on 3 June 2026: abandoned cell pieces, rusty cell door, hanging chains, water pools, ceiling roots, jailer cage/ball-chain, darker lower stair composition, `BROOD_DUNGEON_LEVEL5_BROKEN_THRESHOLD_READY`, and `BROOD_DUNGEON_LEVEL5_EXIT_COMPOSITION_READY` are verified.
- Modular dungeon floor pass completed on 3 June 2026: `BuildDungeonLevel5ModularFloor()` spawns a full `SM_Floor` layer from the MedievalDungeon pack, `BROOD_DUNGEON_LEVEL5_MODULAR_FLOOR_READY` is verified, and screenshots show stone floor assets instead of debug-like tiles.
- Route layout pass completed on 3 June 2026: `BuildDungeonLevel5RouteLayout()` adds modular half walls, short walls, cell blocks and a Jailer Husk chamber read using the MedievalDungeon pack, with `BROOD_DUNGEON_LEVEL5_ROUTE_LAYOUT_READY` verified by smoke.
- Jailer Husk silhouette pass completed on 3 June 2026: `JailerKeyRingVisual`, `JailerKeyTeethVisual`, `JailerChainVisual`, `JailerMantleVisual`, bound arm pieces, manacle/ring props, and hidden Paragon placeholder body create a skeletal dead-jailer read for the boss. `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_DRESSING_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_BESPOKE_SILHOUETTE_READY`, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_PLACEHOLDER_BODY_HIDDEN_READY` are verified, and `SmokeTestDungeonLevel5JailerVisual.ps1` saves `Saved/Automation/WindowedJailerHuskViewport.png` for live boss review.
- Route collision pass completed on 3 June 2026: `BuildDungeonLevel5RouteCollision()` adds selective invisible blockers behind the visible route walls, cells, abandoned cells and Jailer chamber, with `BROOD_DUNGEON_LEVEL5_ROUTE_COLLISION_READY` verified by Level 5 smoke and windowed smoke.
- Camera readability pass completed on 3 June 2026: the gameplay camera now uses a steeper Level 5 angle, shorter spring arm, and disabled spring-arm collision against invisible route blockers. `BROOD_DUNGEON_LEVEL5_CAMERA_READABILITY_READY` is verified by windowed smoke.
- Clean first-look screenshot pass completed on 3 June 2026: `BroodCaptureWindowedSmokeScreenshot()` disables screen debug messages before the art review screenshot and logs `BROOD_DUNGEON_LEVEL5_CLEAN_FIRST_LOOK_READY`.
- Art-review nameplate cleanup completed on 3 June 2026: the windowed first-look screenshot hides player text render components before capture and logs `BROOD_DUNGEON_LEVEL5_ART_REVIEW_NAMEPLATES_HIDDEN_READY`.
- Dungeon Level 5 creature variety pass completed on 3 June 2026: melee, ranged, and brute enemy types now get code-spawned rat/slime/bat-corpse accent components through `CreatureAccentVisual` and `CreatureSecondaryAccentVisual`, with `BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY` covered by windowed and Level 5 smoke.
- Dungeon Level 5 enemy motion pass completed on 3 June 2026: enemy fallback dressing now caches base poses and applies tick-driven bone rattle, crawling lurch, slime pulse, bat/corpse accent flaps, and Jailer key/chain/manacle sway without cumulative transform drift. `BROOD_DUNGEON_LEVEL5_ENEMY_MOTION_READY` is covered by Level 5 and windowed smoke, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_CHAIN_MOTION_READY` is covered by Jailer visual smoke.
- Jailer Husk review chamber pass completed on 3 June 2026: the Jailer visual smoke now reviews the live boss in the authored in-level boss-room area instead of the older off-map stage, hides foreground approach bars for the screenshot, and uses MedievalDungeon floor, wall, pillar, chain, torch, coffin, cell, stocks, gibbet and bone assets with code fallbacks. `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CHAMBER_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PACK_ASSETS_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PRISON_ROOM_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_IN_LEVEL_ROOM_READY`, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_FOREGROUND_BARS_HIDDEN_READY` are covered by `SmokeTestDungeonLevel5JailerVisual.ps1`.
- Full-route visual hardening pass completed on 3 June 2026: the Surface Trial visual stage now uses MedievalDungeon floor/wall/edge assets instead of a checker review block, the lower-exit showcase adds side walls, ceiling pieces, extra floor tiles, dark sightline blockers and pack key props, and the Jailer review chamber adds ceiling/sightline seals around the boss. `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_ASSET_STAGE_READY`, `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_DUNGEON_CLOSED_READY`, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CEILING_CLOSED_READY` are covered by visual smoke scripts.
- In-level authored room pass completed on 3 June 2026: `BuildDungeonLevel5JailerBossRoomArchitecture()` and `BuildDungeonLevel5ExitDescentRoomArchitecture()` add pack floor/wall/ceiling modules, prison props, chains, bones, torches, lock/doorway/stair dressing and depth lighting to the actual gameplay route, not only the isolated visual review stages. `BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY` and `BROOD_DUNGEON_LEVEL5_EXIT_DESCENT_ROOM_AUTHORED_READY` are covered by Level 5, completion visual, Jailer visual, Exit showcase, and windowed smoke checks.
- Jailer Husk vertical body pass completed on 3 June 2026: the boss fallback now includes hips, a spine/neck connector, broken legs, fused hand keys, and a central chain mass so it reads closer to the design target of a dry dead jailer with keys fused into its hands. `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VERTICAL_BODY_READY` is covered by Level 5 and Jailer visual smoke.
- Jailer Husk isolated asset-review pass completed on 3 June 2026: `BroodDungeonLevel5JailerAssetReviewSmokeTest` and `SmokeTestDungeonLevel5JailerAssetReview.ps1` generate `Saved/Automation/WindowedJailerHuskAssetReviewViewport.png` with four Jailer Husk orientations and no dungeon clutter. Screenshot review is technically readable after switching to an orthographic turnaround camera, but the fallback body still needs final authored boss art and stronger grounded animation.
- Dungeon Level 5 environment ARPG camera review added on 3 June 2026: `BroodDungeonLevel5EnvironmentVisualSmokeTest` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` generate `Saved/Automation/WindowedLevel5EnvironmentViewport.png` using the gameplay high-angle camera for repeatable environment review. Screenshot review shows the asset floor, larva and enemies clearly, but the current first view still reads too much like a large floor field; next art pass should add stronger enclosure, vertical rhythm and landmarks visible from the gameplay camera.
- Dungeon Level 5 first-view architecture pass completed on 3 June 2026: `BuildDungeonLevel5FirstViewArchitecture()` adds low side buttresses, pillars, half-arch hints, broken cell doors, hanging chains and warm/cold guide lights around the spawn threshold without blocking the ARPG camera. `BROOD_DUNGEON_LEVEL5_FIRST_VIEW_ARCHITECTURE_READY` is covered by the environment visual smoke. Screenshot review shows better side boundaries and less pure floor dominance. Follow-up tuning replaced oversized `SM_Decorative_Arches` foreground pieces with smaller half-arch/low-trim assets after graphical review showed clipping/showroom scale. Remaining work is making the arch/ceiling landmarks more visually legible from the gameplay camera.
- Dungeon Level 5 first-view threshold spine pass completed on 3 June 2026: `BuildDungeonLevel5FirstViewArchitecture()` now adds paired threshold pillars, small doorway silhouettes, low trim ribs and extra warm/cold spine lights, and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` requires `BROOD_DUNGEON_LEVEL5_FIRST_VIEW_THRESHOLD_SPINE_READY`. The environment review camera was retuned to a lower Path-of-Exile-style view that includes more of the threshold architecture while keeping the larva and first enemies visible. `BuildGame.bat` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed after the change, and `Saved/Automation/WindowedLevel5EnvironmentViewport.png` shows stronger right-side prison/doorway read. Remaining work: fix the left/top cut architectural silhouette and hide any technical background strips before treating the opening shot as final.
- Dungeon Level 5 first-view cleanup pass completed on 3 June 2026: the `SM_Floor` modular layer expanded from a 7x7 to a 9x9 asset floor grid so the ARPG environment review sees floor assets instead of technical edge strips, and the opening half-arch hints were removed after screenshot review showed they created clipped foreground silhouettes. The main first-view piers were reduced in scale so they contribute enclosure without dominating the review camera. `BuildGame.bat` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed after the cleanup. Remaining work: the left edge still has a cut vertical silhouette in `WindowedLevel5EnvironmentViewport.png`; next pass should either move that actor fully into frame or hide it only for the dedicated art-review camera.
- Dungeon Level 5 environment left-edge review cleanup completed on 3 June 2026: `BroodDungeonLevel5EnvironmentVisualSmokeTest()` now hides known first-view edge-only offenders for the dedicated art-review camera, including first-view pier/buttress/chain pieces, the surface dust beam, first creepy hanging-chain candidate, and the surface crack/root candidates, and logs `BROOD_DUNGEON_LEVEL5_ENVIRONMENT_LEFT_EDGE_CLEANUP_READY`. `SmokeTestDungeonLevel5EnvironmentVisual.ps1` requires the marker, and `BuildGame.bat` plus the environment visual smoke passed. Screenshot review shows no new foreground regression and keeps larva, enemies, floor and right-side prison architecture readable. Remaining work: the opening shot still contains several primitive proxy shapes on the floor; next visual pass should replace or hide those in the art-review frame so the image reads less like mixed asset/prototype content.
- Dungeon Level 5 environment proxy cleanup completed on 3 June 2026: `BroodDungeonLevel5EnvironmentVisualSmokeTest()` now hides primitive prologue/traversal/creepy dressing proxies for the dedicated environment screenshot, including dry blood discs, fall-scar cubes, carcass primitives, traversal guide slabs, choice runes, chain shadows and scratch/noose proxy marks. The smoke script requires `BROOD_DUNGEON_LEVEL5_ENVIRONMENT_PROXY_CLUTTER_HIDDEN_READY`, and `BuildGame.bat` plus `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed. Screenshot review shows the frame is cleaner and more driven by `SM_Floor`, prison architecture, larva and enemies. Remaining work: the remaining primitive-looking shapes are mostly creature/player fallback components, so they should be handled in isolated asset-review passes rather than hidden as environment clutter.
- Dungeon Level 5 MCP threshold-corridor pass verified on 3 June 2026: Unreal MCP was used to stage a `DB_MCP_L5_*` corridor prototype directly in the open Editor with MedievalDungeon floor, side walls, doorway, descent stairs, pillars, half-arches, cell fronts, chains, coffin, skull, gibbet cage and torches. The same design was translated into `BuildDungeonLevel5ThresholdCorridorArchitecture()`, and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` now requires `BROOD_DUNGEON_LEVEL5_THRESHOLD_CORRIDOR_READY` and `BROOD_DUNGEON_LEVEL5_THRESHOLD_CORRIDOR_CAMERA_CLEAR_READY`. `BuildGame.bat` passed after closing the Live Coding editor lock, and the environment visual smoke regenerated `Saved/Automation/WindowedLevel5EnvironmentViewport.png`.
- Jailer and lower-exit review cleanup completed on 3 June 2026 after Codex/MCP restart: MCP was restored by launching `DungeonBrood.uproject` in UE 5.7 and confirming the level with `mcp__unreal_mcp.find_actors_by_name`. `SmokeTestDungeonLevel5JailerVisual.ps1` now uses an isolated closed Jailer review chamber with pushed-out side walls, larger dark backplates and a clearer camera. `SmokeTestDungeonLevel5ExitShowcaseVisual.ps1` now isolates showcase actors/lights, adds dedicated warm/cold review lighting and larger dark seals. `BuildGame.bat`, Jailer visual smoke, and Exit showcase visual smoke passed; remaining visual risk is art quality, not test stability.
- Live Unreal Editor construction pass corrected on 3 June 2026: the first `DB_MCP_L5_Live_*` MCP batch was removed because it appeared as Blueprint actor icons rather than visible mesh layout. The current editor construction uses `create_wall`, `create_arch`, and `create_staircase`, which spawn real `StaticMeshActor` geometry under `DB_MCP_L5_Blockout_*`. The visible blockout now has a floor platform, side walls, rear opening, arch, corrected Y-axis stairs, and simplified cell bars. A `DB_MCP_L5_MeshTest_DungeonFloor_*` pack-asset sample is present to verify whether MedievalDungeon meshes render correctly through this construction path. Next step: visually inspect the blockout in the Editor, replace cube blockout sections with pack assets once visibility is confirmed, then translate approved transforms into the runtime C++ helper.
- Editor-authored Level 5 map pass completed on 3 June 2026: `/Game/Deepborn/Maps/L_Dungeon_Level5_Editor` is now the project default map, generated with grounded MedievalDungeon pack architecture, 375 `DB_L5Pack_*` actors, 14 hidden gameplay collision actors, surface temptation, boss chamber, lower descent and set dressing. `ABroodGameMode` now skips the older runtime environment blockout on this map and logs `BROOD_DUNGEON_LEVEL5_EDITOR_AUTHORED_MAP_READY`, keeping gameplay/enemy spawning active without duplicating floors, walls and props. Verification passed with direct UE build, `SmokeTestDungeonLevel5EnvironmentVisual.ps1`, `SmokeTestDungeonLevel5EditorFloor.ps1`, and `.agents/unreal_scripts/validate_level5_editor_map.py`, which confirmed no non-intentional floating pack actors.
- Dungeon Level 5 entrance repair pass completed on 3 June 2026: the editor-authored map now treats the entrance as a wide combat corridor rather than an asset-dressed room. The rebuild adds a visible surface-stair frame and explicit stair-run meshes, continuous side walls/collision, three future phase gates, edge-only cell/tomb dressing, and props placed only by authored purpose. Generated lights are movable to avoid unbuilt-lighting noise. Verification passed through `.agents/unreal_scripts/validate_level5_editor_map.py` with 423 pack actors and 18 collision actors, plus `SmokeTestDungeonLevel5EditorFloor.ps1`.
- Dungeon Level 5 blue crypt-hall direction pass completed on 3 June 2026: after reference review, the first room was redirected toward a closed dungeon hall with blue dominant lighting, paired nave pillars, high ceiling volume, sealed side cells, no unsupported half-arches, wall-mounted chains, and props only where logically anchored. The latest validator reports 588 pack actors, 18 hidden collision actors, and no non-intentional floating pack actors. `SmokeTestDungeonLevel5EditorFloor.ps1` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed.
- Dungeon Level 5 pack-reference correction pass completed on 3 June 2026: after screenshot review against the MedievalDungeon pack examples, the editor-authored Level 5 map was corrected away from blue-dominant/showroom placement. The first entrance gate was removed to keep the opening combat corridor wide, oversized duplicated doorway pieces were replaced with a single crypt doorway phase gate, unsupported ceiling arch/chandelier sightline clutter was removed, warm point lights are now tied to torch/candle sources, and blue lights are limited to low-intensity depth/fill. `.agents/unreal_scripts/validate_level5_editor_map.py` passed with 491 pack actors and 18 hidden collision actors; `SmokeTestDungeonLevel5EditorFloor.ps1` and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed. Remaining art gap: this is a cleaner grounded layout, not final shipped dungeon art; next pass should solve vaulted ceiling/supports using measured pack bounds and rebalance the first-frame lighting toward warmer torch/candle composition.
- Dungeon Level 5 reference-layout restyle started on 3 June 2026: `docs/DUNGEON_LEVEL5_LAYOUT_REFERENCE.md` now records the target top-down layout provided by the user. `.agents/unreal_scripts/build_level5_editor_map.py` was rewritten around the reference graph: surface temptation stair, `Sala della Caduta`, main spine, `Sala Centrale`, `Arena del Carceriere`, level 4 descent, and the six side rooms. The map now regenerates `/Game/Deepborn/Maps/L_Dungeon_Level5_Editor` with 661 `DB_L5Pack_*` actors, 85 hidden collision actors, required layout markers, straight centered stairs, and no non-intentional floating actors. `BuildGame.bat`, `.agents/unreal_scripts/validate_level5_editor_map.py`, `SmokeTestDungeonLevel5EditorFloor.ps1`, and `SmokeTestDungeonLevel5EnvironmentVisual.ps1` passed. The environment visual smoke now uses a top-down orthographic review camera with a runtime-only review light so the layout can be judged against the reference before final dressing. Remaining work: update gameplay spawn/unlock coordinates to the new rooms and replace the rectangular blockout with richer vaulted/cell/biomass dressing while preserving the verified topology.
- Dungeon Level 5 reference-layout wall hardening continued on 4 June 2026: the editor generator now uses a tighter reference-like top-down arrangement, narrower main spine, separated side rooms, octagonal `Sala Centrale`/`Arena del Carceriere` silhouettes, continuous stone-material wall backers behind the visible pack wall modules, design-review lighting, and temporary top-down readability markers for the main flow. Verification passed with `BuildGame.bat`, `.agents/unreal_scripts/validate_level5_editor_map.py` reporting 759 `DB_L5Pack_*` actors and 85 hidden collision actors, `SmokeTestDungeonLevel5EditorFloor.ps1`, and `SmokeTestDungeonLevel5EnvironmentVisual.ps1`. Screenshot `Saved/Automation/WindowedLevel5EnvironmentViewport.png` now matches the reference structure more closely, but final art still needs stronger material polish and better central marker contrast before calling the visual comparison complete.
- Dungeon Level 5 creepy spatial dressing pass completed on 3 June 2026: `BuildDungeonLevel5CreepyDressing()` adds edge-placed hanging chains, dry corpses, warning bones, drag scratches, floor shadows, and cold shadow pockets without blocking the main play route. `BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY` is covered by Level 5 and windowed smoke.
- Advanced larva silhouette pass completed on 3 June 2026: the starting player now adds code-spawned mandibles, side spines, a tail stinger, and subtle organic movement on top of the segmented larva fallback. `BROOD_LARVA_ALIEN_SILHOUETTE_READY` and `BROOD_LARVA_ORGANIC_MOTION_READY` are covered by windowed and Level 5 smoke.
- Prologue chamber dressing pass completed on 3 June 2026: the spawn area now has a fall scar, dry blood, small carcasses, loose bones, ancient biomass glow, and a surface dust/light leak so the first room communicates the larva's fall and immediate hunger. `BROOD_DUNGEON_LEVEL5_PROLOGUE_DRESSING_READY` is covered by Level 5 and windowed smoke.
- Traversal composition pass completed on 3 June 2026: the spawn chamber now has a warm visual trail toward the surface, a cold green trail toward the descent, broken low threshold edging, and up/down choice runes. `BROOD_DUNGEON_LEVEL5_TRAVERSAL_COMPOSITION_READY` is covered by Level 5 and windowed smoke.
- Jailer approach dressing pass completed on 3 June 2026: the lower route now has a broken jail gate, floor chains, warning bones, old blood, a dead-stone landing, and a key-rune lead-in to the Jailer Husk. `BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY` is covered by Level 5 and windowed smoke.
- Completion presentation pass completed on 3 June 2026: the post-Jailer exit now removes debug HUD/text clutter from the art screenshot, hides completion-only foreground occluders, uses an in-level angled camera, and replaces the previous primitive cone/rune portal feedback with MedievalDungeon rim, open lock, fallen key and broken chain assets. `BROOD_DUNGEON_LEVEL5_COMPLETION_REVIEW_CAMERA_READY`, `BROOD_DUNGEON_LEVEL5_COMPLETION_ANGLED_OCCLUDERS_HIDDEN_READY`, and `BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED_ASSET_FEEDBACK_READY` are covered by the completion visual smoke.
- Lower exit architecture pass completed on 3 June 2026: the descent now uses MedievalDungeon doorway, half-arch, pillar, visible stair, open lock, chain, torch and depth-light assets instead of relying only on primitive portal feedback. `BROOD_DUNGEON_LEVEL5_EXIT_ARCHITECTURE_READY` is covered by Level 5, windowed, and completion visual smoke.
- Lower exit angled review pass completed on 3 June 2026: `BroodDungeonLevel5ExitShowcaseSmokeTest` and `SmokeTestDungeonLevel5ExitShowcaseVisual.ps1` now generate `Saved/Automation/WindowedLevel5ExitShowcaseViewport.png`, an isolated angled showcase of the lower descent using MedievalDungeon floor, stair, doorway, half-arch, pillar, lock, chain and torch assets. `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_STAGED_READY` and `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_ASSET_FLOOR_READY` verify that this review is staged with pack assets rather than debug platform geometry.
- Surface death branch pass completed on 3 June 2026: the upper stair now has a surface temptation trigger/prompt, entering it spawns guard/archer/knight pressure silhouettes, applies lethal damage, and logs `THIS_BODY_WAS_NOT_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_HUMANOID_SILHOUETTES_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_STAGED_SILHOUETTES_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_DEATH_READY`, and `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_PASSED`.
- Visual readability pass completed on 3 June 2026 after screenshot review: `BuildDungeonLevel5MoodLighting()` darkens the overexposed dungeon view, central route walls now use low-profile `SM_Wall_Floor_Trim` / `SM_Edge_Stones` assets, and visual smoke tests verify `BROOD_DUNGEON_LEVEL5_MOOD_LIGHTING_READY` plus `BROOD_DUNGEON_LEVEL5_LOW_PROFILE_ROUTE_READY`.
- Readable darkness and enemy silhouette pass completed on 3 June 2026: the mood post-process was rebalanced after screenshot review, center/enemy read lights were added, first-wave enemies were moved slightly closer, and high-contrast accent markers now emit `BROOD_DUNGEON_LEVEL5_READABLE_DARKNESS_READY` plus `BROOD_DUNGEON_LEVEL5_ENEMY_SILHOUETTE_READY`.

## New Direction

The new game is no longer a single first-level village crypt. It is a 10-level roguelite run:

1. The larva starts near the surface.
2. The player may try the surface and die quickly.
3. The correct route is downward through dungeon levels 5 to 1.
4. The larva mutates through body-card choices.
5. The Dungeon Level 1 boss grants the Ascending Core.
6. The creature returns upward through surface levels 6 to 10.
7. The final boss is Sir Aldric, the Radiant Lance.

## Migration Strategy

Do not throw away the current C++ prototype. Reuse it as a combat and smoke-test harness while changing the progression model.

Recommended order:

1. Update documentation source of truth to Deepborn.
2. Rename the plan around descent/ascent progression.
3. Convert the current four-wave micro-run into Dungeon Level 5 prototype content.
4. Replace current human wave labels with dungeon enemy labels for the descent.
5. Add a first playable surface-death branch.
6. Add a lightweight level progression state machine.
7. Replace simple evolution count with body-slot/tag mutation tracking.
8. Expand cards into body, organ, instinct, and unstable mutation types.
9. Add archetype detection.
10. Add surface phase after the Buried Warden.

## Task 1: Documentation Pivot

Status: completed.

Goal:

Replace the old single-level village-crypt design with the Deepborn descent/ascent design.

Files:

- Modify: `docs/GAME_DESIGN_SPEC.md`
- Modify: `docs/CURRENT_IMPLEMENTATION_PLAN.md`
- Keep: `docs/ASSET_INTEGRATION.md`
- Remove old mixed design/implementation documents.

Verification:

- `docs/GAME_DESIGN_SPEC.md` contains `Deepborn`, `Dungeon Level 5`, `Dungeon Level 1`, `Surface Level 10`, `The Buried Warden`, and `Sir Aldric`.
- `docs/CURRENT_IMPLEMENTATION_PLAN.md` treats the old village-crypt run as prototype infrastructure.

## Task 2: Preserve Prototype Stability

Status: completed.

Goal:

Keep current runtime playable while design migrates.

Files:

- Existing C++ gameplay files.
- Existing smoke scripts.

Acceptance:

- `BuildGame.bat` passes.
- `SmokeTestAll.bat` passes.
- Existing larva fallback and boss-run smoke remain useful.

Current evidence:

- The last full code verification before this documentation pivot passed on 2 June 2026.
- Dungeon Level 5 art marker test passed for `BuildDungeonLevel5Landmarks`, `BROOD_DUNGEON_LEVEL5_ART_READY`, `UpperSurfaceStair`, `LowerDungeonStair`, `LarvaSegment`, and `LarvaGlow`.
- Dungeon Level 5 visual marker tests passed for enemy bone dressing, fallback marker coverage, larva glow light, and viewport screenshot capture.
- Visual screenshot review passed on 2 June 2026: the larva reads clearly with a green glow, skeleton enemies show bone dressing, and HUD text does not overlap the play area.
- Completion screenshot review passed on 3 June 2026: the lower stair unlock is visible through `Saved/Automation/WindowedLevel5CompleteViewport.png`; remaining polish is to hide arena edges and make the final exit scene more attractive.
- Broken Threshold screenshot review passed on 3 June 2026: the lower stair area now reads more like stone dungeon space with darker floor composition; remaining polish is to eliminate the last bright outer bounds and add a stronger Jailer Husk-specific silhouette.
- Modular floor screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` and `Saved/Automation/WindowedLevel5CompleteViewport.png` show MedievalDungeon stone floor assets across the playable space.
- Route layout screenshot review passed on 3 June 2026: the first-view screenshot keeps combat readable while adding dungeon walls/cell structure at the edges; the next gap is making route walls selectively physical once encounter flow is stable.
- Jailer Husk screenshot review passed on 3 June 2026: `Saved/Automation/WindowedJailerHuskViewport.png` now shows the boss review isolated on the dungeon floor with the Paragon placeholder body hidden, leaving a skeletal jailer silhouette built from ribcage/skull/bone/key/chain/manacle props. Remaining art gap: replace this code-spawned skeletal composition with a final custom undead jailer asset once asset production is ready.
- Jailer Husk review chamber screenshot review passed on 3 June 2026: `Saved/Automation/WindowedJailerHuskViewport.png` now uses an asset-based dungeon chamber rather than the previous exposed technical review area. Remaining polish: close the last small bright side gap and replace the code-spawned skeletal composition with final boss art.
- Jailer Husk vertical body screenshot review passed on 3 June 2026: `Saved/Automation/WindowedJailerHuskViewport.png` now shows a more connected skeletal jailer body with added hips/legs/spine and fused hand keys. Remaining polish: final authored boss mesh/animation and a fully closed boss-room backdrop.
- Creepy dressing screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` keeps the larva, enemies, and MedievalDungeon floor readable while adding darker edge dressing and horror props around the route. Remaining work: continue reviewing the full route from start to Jailer to exit, not just the first camera.
- Route collision screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` still shows readable player/enemies/floor after invisible blockers were added. `Saved/Automation/WindowedLevel5CompleteViewport.png` shows improved lower-stair composition after replacing large bright floor-mask pieces with darker code-spawned slabs. Remaining visual gap: reduce camera-facing wall occlusion and replace temporary boss/player placeholders with stronger custom or imported art.
- Camera readability screenshot review passed on 3 June 2026: the updated `Saved/Automation/WindowedSmokeViewport.png` reduces foreground wall occlusion in the first playable view while keeping the modular stone floor, larva glow, and first enemies readable. Remaining visual gap: reduce debug/HUD noise and improve bespoke creature/boss art.
- Clean first-look screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` is now captured after screen debug messages are hidden, giving a cleaner art-review image for the opening Level 5 view.
- Art-review nameplate screenshot review passed on 3 June 2026: the updated `Saved/Automation/WindowedSmokeViewport.png` no longer shows the mirrored/partial player nameplate over the larva. Remaining visual gap: enemies are still prototype-readable rather than final-art readable at distance.
- Creature variety smoke review passed on 3 June 2026: `BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY` verifies that the current Level 5 enemy set has rat/slime/bat-corpse accent dressing without requiring new binary assets. Follow-up screenshot review led to stronger first-wave enemy silhouettes and readable-darkness lighting, covered by `BROOD_DUNGEON_LEVEL5_ENEMY_SILHOUETTE_READY` and `BROOD_DUNGEON_LEVEL5_READABLE_DARKNESS_READY`.
- Enemy motion screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` keeps the larva and first enemies readable after the fallback motion pass, and `Saved/Automation/WindowedJailerHuskViewport.png` shows the skeletal Jailer composition with visible chain/key props. Remaining art gap: authored final animations and creature-specific assets still need production later.
- Advanced larva screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` shows the larva with visible mandibles, side spines, tail stinger, green core glow, and low alien silhouette against the MedievalDungeon floor.
- Prologue dressing screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` shows the central room with dry blood/fall scar/carcass/bone/biomass dressing around the larva while keeping combat readability.
- Traversal composition screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSmokeViewport.png` shows clearer first-room directionality between the warm surface path and the green descent path without obscuring the larva or first combat read.
- Jailer approach smoke pass completed on 3 June 2026: `BROOD_DUNGEON_LEVEL5_JAILER_APPROACH_READY` verifies a stronger lower-route lead-in toward the Jailer Husk, using a broken jail gate, floor chains, warning bones, old blood, dead stone, and a key-rune marker.
- Completion presentation screenshot review passed on 3 June 2026: `Saved/Automation/WindowedLevel5CompleteViewport.png` no longer contains green HUD/debug completion text and now frames the opened descent with physical portal/rune/light pieces. Remaining polish: tune the large lower-stair frame so it feels architectural instead of heavy in the cropped review.
- Lower exit architecture screenshot review passed on 3 June 2026: `Saved/Automation/WindowedLevel5CompleteViewport.png` shows the opened descent with additional MedievalDungeon doorway/arch/pillar/stair/lock/chain/torch dressing and no gameplay HUD text. Remaining polish: the top-down review crop still flattens some architecture; future pass should validate this area from an in-game angled camera once foreground occlusion is controlled.
- Lower exit showcase screenshot review passed on 3 June 2026: `Saved/Automation/WindowedLevel5ExitShowcaseViewport.png` shows the Dungeon Level 4 descent architecture from an angled camera with pack floor tiles, staged stair landing, walls, edge stones, and no foreground dungeon occluders. Remaining polish: a small far-background strip of technical review space is still visible outside the main architecture.
- Lower exit showcase screenshot review passed again on 3 June 2026 after hardening: `Saved/Automation/WindowedLevel5ExitShowcaseViewport.png` now hides more of the technical background with extra floor, side wall, ceiling and dark backplate coverage. Remaining polish: tune the angled staging so it feels less like an isolated review set and more like an in-level dungeon descent.
- Surface trial screenshot review passed on 3 June 2026: `Saved/Automation/WindowedSurfaceTrialViewport.png` is generated by `SmokeTestDungeonLevel5SurfaceTrialVisual.ps1`. Current visual read proves the upper stair/light/death-message branch and now uses pack floor/wall/edge assets under the staged guard, archer, and knight silhouettes. Remaining visual gap: replace the temporary surface attackers with final readable humanoid assets once asset production catches up.
- Jailer Husk screenshot review passed again on 3 June 2026 after hardening: `Saved/Automation/WindowedJailerHuskViewport.png` now captures the live boss in the in-level authored room area, hides foreground bars that blocked the shot, and reduces the oversized fused chain silhouette on the boss. Remaining visual gap: the shot is improved but still reads too close to a stair/threshold composition rather than a fully composed boss-room portrait; next pass should position the boss encounter focal point deeper inside the Jailer room and rebalance warm/cold lighting.
- Completion screenshot review passed again on 3 June 2026 after adding the in-level exit room, switching the completion camera to an angled view, hiding non-subject foreground occluders, removing text render clutter, and replacing primitive completion feedback with pack lock/key/chain/rim assets. `Saved/Automation/WindowedLevel5CompleteViewport.png` now shows the authored descent room with asset floor, pillars, stairs, chains and green depth lighting instead of a wall/primitive-dominated closeup. Remaining visual gap: tune final lighting and composition so the lower exit feels more like shipped dungeon art and less like an art-review crop.
- Jailer asset-turnaround screenshot review passed technically on 3 June 2026: `Saved/Automation/WindowedJailerHuskAssetReviewViewport.png` now shows front/back/side orientations in one isolated shot. Remaining visual gap: the code-spawned skeletal fallback is readable but still not final; it needs a bespoke boss mesh or a much tighter animated corpse assembly before Level 5 can be considered art-complete.
- Environment ARPG camera screenshot review passed technically on 3 June 2026: `Saved/Automation/WindowedLevel5EnvironmentViewport.png` is generated by the new environment smoke. Remaining visual gap: the camera is stable, but the scene needs more enclosure and dungeon landmarks in the first playable view so it does not read primarily as a flat floor with props.
- First-view architecture screenshot review passed on 3 June 2026: `Saved/Automation/WindowedLevel5EnvironmentViewport.png` now shows low side boundaries around the larva/enemy view while preserving playable readability. A large foreground arch regression was caught visually and removed in favor of half-arch/low-trim assets. Remaining visual gap: the new half-arch and ceiling-hint pieces are present but still subtle from the high camera, so the next pass should create a stronger doorway/corridor silhouette without foreground clipping.

## Task 3: Convert Current Micro-Run Into Dungeon Level 5

Status: completed.

Goal:

Retheme the current four encounters away from surface humans and toward the opening dungeon level.

Design target:

- Location: Dungeon Level 5, Upper Threshold.
- Enemies: weak skeletons, mutated rats, small slimes, bats, animated remains.
- Boss or miniboss: `Jailer Husk`, the dead jailer guarding the stair to Dungeon Level 4.

Files:

- Modify: `Source/DungeonBrood/Private/Game/BroodGameMode.cpp`
- Modify: `Source/DungeonBrood/Private/Enemies/BroodEnemyCharacter.cpp`
- Modify: `docs/ASSET_INTEGRATION.md` if new optional paths are linked.

Planned changes:

- Replace current human-facing wave text with Dungeon Level 5 text.
- Map existing mechanics temporarily:
  - `MeleeGrub` mechanics -> skeleton or rat;
  - `Spitter` mechanics -> skeleton archer or slime;
  - `Brute` mechanics -> animated remains;
  - `BossBrute` mechanics -> Jailer Husk.
- Keep surface human labels available for later Surface Level 6+.
- Update smoke marker names only if necessary; keep tests stable.

Verification:

- content marker test passed for `BroodDungeonLevel5SmokeTest`, `SKELETON`, `SKELETON ARCHER`, `ANIMATED REMAINS`, `JAILER HUSK`, `La Soglia Spezzata`, and `Dungeon Level 5`;
- `BuildGame.bat`: passed on 2 June 2026;
- `SmokeTestDungeonLevel5.bat`: passed on 3 June 2026 with Jailer Husk and completion markers;
- `SmokeTestDungeonLevel5.bat`: now also requires `BROOD_DUNGEON_LEVEL5_CREATURE_VARIETY_READY`;
- `SmokeTestDungeonLevel5.bat`: now also requires `BROOD_DUNGEON_LEVEL5_ENEMY_MOTION_READY` and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_CHAIN_MOTION_READY`;
- `SmokeTestDungeonLevel5.bat`: now also requires `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VERTICAL_BODY_READY`;
- `SmokeTestDungeonLevel5.bat`: now also requires `BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY`;
- `SmokeTestDungeonLevel5.bat`: now also requires `BROOD_DUNGEON_LEVEL5_JAILER_BOSS_ROOM_AUTHORED_READY` and `BROOD_DUNGEON_LEVEL5_EXIT_DESCENT_ROOM_AUTHORED_READY`;
- `SmokeTestDungeonLevel5.bat`: now also requires `BROOD_DUNGEON_LEVEL5_EXIT_UNLOCKED_ASSET_FEEDBACK_READY`;
- `SmokeTestDungeonLevel5Visual.ps1`: passed on 3 June 2026 and saved `Saved/Automation/WindowedLevel5CompleteViewport.png`;
- `SmokeTestDungeonLevel5JailerVisual.ps1`: passed on 3 June 2026 and saved `Saved/Automation/WindowedJailerHuskViewport.png`;
- `SmokeTestDungeonLevel5JailerVisual.ps1`: now also requires `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CHAMBER_READY` and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PACK_ASSETS_READY`;
- `SmokeTestDungeonLevel5JailerVisual.ps1`: now also requires `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_CEILING_CLOSED_READY`;
- `SmokeTestDungeonLevel5JailerVisual.ps1`: now also requires `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_PRISON_ROOM_READY`, `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_IN_LEVEL_ROOM_READY`, and `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_REVIEW_FOREGROUND_BARS_HIDDEN_READY`;
- `SmokeTestDungeonLevel5JailerVisual.ps1`: now also requires `BROOD_DUNGEON_LEVEL5_JAILER_HUSK_VERTICAL_BODY_READY`;
- `SmokeTestDungeonLevel5JailerAssetReview.ps1`: passed on 3 June 2026 and saves `Saved/Automation/WindowedJailerHuskAssetReviewViewport.png`;
- `SmokeTestDungeonLevel5EnvironmentVisual.ps1`: passed on 3 June 2026 and saves `Saved/Automation/WindowedLevel5EnvironmentViewport.png`;
- `SmokeTestDungeonLevel5EnvironmentVisual.ps1`: now also requires `BROOD_DUNGEON_LEVEL5_FIRST_VIEW_ARCHITECTURE_READY`;
- `SmokeTestDungeonLevel5SurfaceTrialVisual.ps1`: now requires `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_ASSET_STAGE_READY`;
- `SmokeTestDungeonLevel5ExitShowcaseVisual.ps1`: now requires `BROOD_DUNGEON_LEVEL5_EXIT_SHOWCASE_DUNGEON_CLOSED_READY`;
- `SmokeTestAll.bat`: now runs Surface Trial visual, Jailer visual, and Exit showcase visual smoke checks so the Level 5 full smoke suite includes graphical checkpoints across the route.
- `SmokeTestWindowed.ps1`: now requires `BROOD_DUNGEON_LEVEL5_CREEPY_DRESSING_READY` and sends both mouse and SpaceBar attack inputs so the real attack check is less sensitive to window focus.
- `SmokeTestAll.bat`: passed on 3 June 2026.

## Task 4: Surface Death Branch

Status: first pass completed on 3 June 2026.

Goal:

Let the player choose the lit stairway upward at the start and learn that the surface is death.

Files:

- Modify: `Source/DungeonBrood/Private/Game/BroodGameMode.cpp`
- Modify: `Source/DungeonBrood/Public/Game/BroodGameMode.h`
- Possibly modify player controller or input routing for an early interaction.

Prototype acceptance:

- Starting chamber has two readable directions: up and down.
- Entering the up path triggers a short playable surface ambush.
- The ambush includes overwhelming enemies or direct scripted damage.
- Death message: `This body was not ready.`
- Restart returns to the starting chamber.

Verification:

- `SmokeTestDungeonLevel5SurfaceTrial.bat` proves `BROOD_DUNGEON_LEVEL5_SURFACE_TEMPTATION_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_ENTERED`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_GUARDS_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_HUMANOID_SILHOUETTES_READY`, `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_DEATH_READY`, `THIS_BODY_WAS_NOT_READY`, and `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_SMOKE_PASSED`.
- `SmokeTestDungeonLevel5SurfaceTrialVisual.ps1` proves `BROOD_DUNGEON_LEVEL5_SURFACE_TRIAL_REVIEW_STAGED_SILHOUETTES_READY` and saves `Saved/Automation/WindowedSurfaceTrialViewport.png`.
- Existing smoke suite must still pass.

## Task 5: Level Progression State

Status: not started.

Goal:

Replace the single encounter index with a lightweight run progression model.

Initial enum:

- `SurfaceTrial`
- `DungeonLevel5`
- `DungeonLevel4`
- `DungeonLevel3`
- `DungeonLevel2`
- `DungeonLevel1`
- `SurfaceLevel6`
- `SurfaceLevel7`
- `SurfaceLevel8`
- `SurfaceLevel9`
- `SurfaceLevel10`

Files:

- Modify: `Source/DungeonBrood/Public/Game/BroodGameMode.h`
- Modify: `Source/DungeonBrood/Private/Game/BroodGameMode.cpp`

Acceptance:

- runtime HUD/debug text says which level is active;
- encounter composition can branch by run level;
- boss completion can move to the next run level instead of ending immediately.

## Task 6: Body Slot Mutation Model

Status: not started.

Goal:

Move from simple evolution IDs to body-slot-aware mutation cards.

Core data:

- mutation id;
- display name;
- card type: body, organ, instinct, unstable;
- body slot;
- tags;
- effect summary;
- exclusivity group;
- minimum form;
- whether it is larval-exclusive.

Files:

- Modify or extend: `Source/DungeonBrood/Public/Evolution/*`
- Modify: `Source/DungeonBrood/Private/Characters/BroodCharacter.cpp`
- Modify: `Source/DungeonBrood/Private/Game/BroodGameMode.cpp`

First cards to model:

- Primary Claws;
- Predatory Legs;
- Segmented Tail;
- Membrane Wings;
- Toxic Glands;
- Carapace;
- Neural Tentacles;
- Threadlike Body.

Acceptance:

- each chosen card records body slot and tags;
- mutually exclusive slots reject or downgrade conflicting cards;
- at least one card visibly changes fallback body state.

## Task 7: Archetype Detection

Status: not started.

Goal:

Detect emergent build classes from mutation tags.

First archetypes:

- Alien Dragon;
- Reaper;
- Plague Horror;
- Armored Behemoth;
- Mind Parasite;
- Chimera.

Files:

- Modify or create evolution/archetype helper files.
- Update player status/debug output.

Acceptance:

- tag thresholds are counted;
- primary archetype is selected when conditions are met;
- mixed builds can become Chimera;
- smoke or unit-style content marker verifies one archetype unlock.

## Task 8: Buried Warden Prototype

Status: not started.

Goal:

Create the first true dungeon boss milestone.

Design:

- Boss name: The Buried Warden.
- Phases: heavy strikes, summons/shockwaves, exposed core.
- Reward: Ascending Core.

Prototype acceptance:

- can reuse current BossBrute mechanics initially;
- nameplate and marker read `THE BURIED WARDEN`;
- death grants `ASCENDING CORE`;
- run progression unlocks surface ascent.

## Task 9: Surface Phase Prototype

Status: not started.

Goal:

After the Buried Warden dies, start surface levels with human escalation.

First surface content:

- Surface Level 6: farmers, dogs, hunters, light guards, priests.
- Surface Level 7: village, bells, militia, archers, traps.

Acceptance:

- surface phase starts only after dungeon boss reward;
- humans become the enemy roster here, not in the opening descent;
- at least one human counter mechanic is prototyped.

## Task 10: Final Boss Direction

Status: not started.

Goal:

Prototype Sir Aldric as the final endpoint of the run.

Design:

- Phase 1: mounted lance combat.
- Phase 2: sacred magic and burning biomass zones.
- Phase 3: radiant fused knight/horse mutation.

Prototype acceptance:

- nameplate and smoke marker identify `SIR ALDRIC`;
- final boss death completes the full run;
- smoke command can drive a shortened full-run path.

## Current Completion Criteria

The project is on track when:

- the design spec reflects Deepborn, not the old micro-run;
- the current prototype remains buildable and smoke-testable;
- Dungeon Level 5 replaces the old human wave opening;
- the player can choose the surface early and learn that the body is not ready;
- mutation cards track body slots and tags;
- at least one archetype unlocks from choices;
- The Buried Warden opens the surface ascent;
- humans are reintroduced as surface enemies;
- Sir Aldric becomes the final long-term boss target.

## Explicitly Open

- Final alien/larva asset line.
- Modular body visual implementation.
- Full descent/ascent level content.
- Final boss implementation.
- Real UI for cards and mutation body slots.
