# Deepborn - Vertical Slices Roadmap

## Purpose

This document defines the vertical slices needed to finish Deepborn. It is the production roadmap: what to build, in what order, what assets are needed, and what proves each slice is complete.

Game design source of truth: `docs/GAME_DESIGN_SPEC.md`  
Current engineering plan: `docs/CURRENT_IMPLEMENTATION_PLAN.md`  
Asset sources and license notes: `docs/ASSET_INTEGRATION.md`

## Production Principle

Deepborn should become visually compelling early. The first real level must not feel like a gray prototype with future promises. The evolution system is the long-term differentiator, but it should not block making Dungeon Level 5 feel good first.

Order of focus:

1. Define the game clearly.
2. Build a stable playable base.
3. Make the first level beautiful and readable with real or curated placeholder assets.
4. Finish Dungeon Level 5 as a complete vertical slice.
5. Expand level progression.
6. Add deeper mutation and modular evolution.
7. Build the full descent/ascent run.
8. Polish, balance, optimize, and ship.

## Current Prototype Baseline

The current project already proves:

- third-person movement;
- mouse aim;
- basic melee;
- dodge;
- enemy chase and attack behavior;
- biomass pickup drops;
- wave progression;
- basic evolution choices;
- larva fallback visual;
- boss-run smoke coverage;
- code-spawned dungeon/village dressing;
- optional local `MedievalDungeon` and `ParagonMinions` imports.

This is useful infrastructure, not final game structure. The next production slices should reshape it toward Deepborn.

## Slice 0: Game Definition Lock

### Goal

Lock the creative direction enough that asset selection, level art, UI, and code architecture stop drifting.

### Content

- Final working title: `Deepborn`.
- Core pitch: alien larva descends through dungeon to become powerful, then ascends to invade the surface.
- Run structure: Dungeon Levels 5 to 1, then Surface Levels 6 to 10.
- First production target: Dungeon Level 5, Upper Threshold.
- Evolution system scoped as a later major slice, not the first production blocker.

### Assets

No new asset import required.

### Deliverables

- `docs/GAME_DESIGN_SPEC.md` updated.
- `docs/DEEPBORN_VERTICAL_SLICES.md` created.
- `docs/CURRENT_IMPLEMENTATION_PLAN.md` references this roadmap.
- Old mixed design/implementation documents have been removed.

### Acceptance Criteria

- A developer can read the docs and understand the game loop, first level target, long-term mutation goal, and production order.
- No active document presents the old village-crypt micro-run as the final design.

### Verification

- `rg "Deepborn|Dungeon Level 5|The Buried Warden|DEEPBORN_VERTICAL_SLICES" docs`

## Slice 1: Playable Foundation

### Goal

Keep the current C++ prototype stable while preparing it for production content.

### Content

- Player movement.
- Camera and mouse aim.
- Basic melee attack.
- Dodge and stamina.
- Health, damage, death, restart.
- Enemy spawn, chase, attack, death.
- Biomass pickups.
- Basic wave progression.
- Smoke tests.

### Assets

Use code-spawned fallback meshes and existing local optional assets only.

Required fallback visuals:

- larva player fallback;
- enemy primitive silhouettes;
- dungeon floor/walls;
- readable pickups;
- readable attack feedback.

### Acceptance Criteria

- The game boots without optional imported assets.
- The player can move, aim, attack, dodge, kill enemies, collect biomass, and die.
- Smoke tests can validate the loop.

### Verification

- `BuildGame.bat`
- `SmokeTestAll.bat`

## Slice 2: First Level Art Direction and Asset Base

### Goal

Make Dungeon Level 5 visually credible before expanding systems. This is where the game starts becoming attractive.

### Target Level

Dungeon Level 5: Upper Threshold.

This level is the closest dungeon layer to the surface. It should feel like a medieval crypt under a living village: stone, old bones, water, roots, broken human objects, torches above, rot below.

### Content

- Art direction board or written visual target.
- Curated asset list.
- Import plan for minimal required assets.
- First pass lighting.
- First pass materials.
- First pass environment composition.
- First pass player and enemy silhouettes.

### Required Asset Categories

#### Environment

- modular crypt/dungeon floor;
- modular walls and arches;
- stairs up and stairs down;
- broken doors or gate pieces;
- pillars;
- torches or candles;
- roots, bones, rubble, wet floor detail;
- village intrusion props such as crates, barricades, tools, cloth, banners.

#### Player

- larva or alien crawler mesh, or a stronger custom fallback if no free asset works;
- material that reads organic, alien, wet or chitinous;
- simple idle/move posture;
- attack-facing readability.

#### Enemies

For Dungeon Level 5:

- weak skeleton;
- mutated rat or small beast;
- slime or crawling hazard;
- bat or flying nuisance if feasible;
- animated remains or Jailer Husk dressing.

#### UI and Feedback

- readable health/stamina/biomass;
- wave/level label;
- damage and death feedback;
- pickup feedback.

### Asset Rules

- Prefer free Epic/Fab assets with license recorded.
- Import only what the slice uses.
- Keep fallback code path.
- Do not commit giant full packs without an asset storage decision.
- Any wired path must be documented in `docs/ASSET_INTEGRATION.md`.

### Acceptance Criteria

- Opening camera shows a visually coherent dungeon/crypt, not an abstract arena.
- The player reads as a larva or clearly alien creature.
- Dungeon Level 5 enemies read as dungeon creatures or undead, not surface villagers.
- Lighting supports dark fantasy but gameplay remains readable.
- Build and smoke tests still pass.

### Verification

- `BuildGame.bat`
- `SmokeTestAll.bat`
- Windowed visual check at 1280x720.
- Screenshot review for readability of player, enemies, pickups, and stairs.

## Slice 3: Dungeon Level 5 Complete Vertical Slice

### Goal

Finish the first playable level before building the full game. This is the first level that should be good enough to show.

### Content

- Starting chamber with two paths:
  - lit stair up to surface trial;
  - dark stair down into Dungeon Level 5.
- Optional first surface attempt:
  - short playable ambush;
  - overwhelming humans;
  - death message: `This body was not ready.`
- Dungeon Level 5 combat route.
- 3 to 5 encounter rooms or arena phases.
- First mutation reward flow, still simple.
- Level-end miniboss: Jailer Husk.
- Transition to Dungeon Level 4 placeholder.

### Gameplay Scope

Keep evolution simple in this slice:

- 6 to 8 simple mutation cards;
- cards can still be mostly stat/ability based;
- at least 2 cards should visibly affect fallback body or color;
- no full modular body system yet.

### Enemy Roster

- weak skeleton;
- mutated rat;
- slime;
- bat or fast nuisance;
- animated remains elite;
- Jailer Husk.

### Assets

This slice should use real or curated visual assets for:

- level environment;
- player larva;
- at least 3 enemy types;
- pickup;
- level transition/stairs;
- basic UI frame or HUD elements.

### Acceptance Criteria

- A new player understands where they are and why they should descend.
- The first level looks intentional.
- Combat, pickups, and first rewards are clear without reading logs.
- The level has a beginning, middle, and end.
- The level can be completed in a few minutes.
- The player can die and restart.

### Verification

- Dedicated smoke test for Dungeon Level 5 completion.
- `BuildGame.bat`
- `SmokeTestAll.bat`
- Manual windowed playthrough.

## Slice 4: First-Level Polish Pass

### Goal

Make Dungeon Level 5 feel good before scaling content.

### Content

- Improved combat feedback.
- Better attack readability.
- Better enemy hit/death feedback.
- Better biomass pickup visuals.
- Basic HUD replacing most debug text.
- Sound pass for:
  - attack;
  - hit;
  - pickup;
  - enemy death;
  - mutation choice;
  - level complete.
- VFX pass for:
  - hit flash;
  - biomass collect;
  - death puff/burst;
  - mutation pulse.
- Lighting and composition polish.

### Assets

- basic sound set;
- simple particle or Niagara effects;
- UI textures/icons if needed;
- improved materials.

### Acceptance Criteria

- The level can be understood without logs.
- The game feels responsive.
- Hits and pickups are satisfying.
- The first level is visually good enough for early video capture.

### Verification

- Windowed playthrough.
- Screenshot pass.
- `SmokeTestAll.bat`

## Slice 5: Level Progression Framework

### Goal

Replace the single hardcoded run with a simple progression model for all 10 levels.

### Content

- Run level enum/state:
  - SurfaceTrial;
  - DungeonLevel5;
  - DungeonLevel4;
  - DungeonLevel3;
  - DungeonLevel2;
  - DungeonLevel1;
  - SurfaceLevel6;
  - SurfaceLevel7;
  - SurfaceLevel8;
  - SurfaceLevel9;
  - SurfaceLevel10.
- Level definition data:
  - display name;
  - environment theme;
  - enemy roster;
  - wave count;
  - boss/miniboss;
  - reward type;
  - next level.
- Runtime transition between levels.
- Debug/smoke path for moving through levels quickly.

### Assets

No full asset requirement for every level yet. Use placeholders outside Dungeon Level 5.

### Acceptance Criteria

- Dungeon Level 5 can transition to Dungeon Level 4.
- The game tracks active level separately from encounter index.
- Smoke tests can validate at least two level transitions.

### Verification

- New progression smoke test.
- `BuildGame.bat`
- `SmokeTestAll.bat`

## Slice 6: Dungeon Descent Content

### Goal

Build the five-level dungeon descent.

### Levels

- Dungeon Level 5: Upper Threshold.
- Dungeon Level 4: Rotten Crypt.
- Dungeon Level 3: Ancient Nest.
- Dungeon Level 2: Cult Halls.
- Dungeon Level 1: Buried Heart.

### Content

Each dungeon level needs:

- distinct visual theme;
- distinct enemy roster;
- 3 to 5 encounter spaces or phases;
- level reward;
- transition to next level.

### Boss

Dungeon Level 1 ends with **The Buried Warden**.

Prototype boss phases:

- heavy strikes and summons;
- ground cracks or danger zones;
- exposed core;
- player devours core.

Reward:

- `Ascending Core`;
- unlocks surface route;
- grants major mutation choice.

### Assets

Required by the end of this slice:

- undead/skeleton pack or curated equivalent;
- rat/beast or crawler enemy;
- slime or hazard enemy;
- spider/worm/nest creatures;
- cultist or mage placeholder;
- flesh golem/construct placeholder;
- Buried Warden boss mesh or strong custom placeholder;
- environment variations for crypt, nest, cult halls, buried heart.

### Acceptance Criteria

- The player can complete the full descent.
- The dungeon gets harder and stranger as it goes deeper.
- The Buried Warden marks a real midpoint climax.
- The Ascending Core opens the surface phase.

### Verification

- Descent smoke test.
- Boss smoke test.
- Manual full descent playthrough.

## Slice 7: Surface Phase Content

### Goal

Build the power reversal: the monster returns to the surface and humans adapt.

### Levels

- Surface Level 6: Outer Fields.
- Surface Level 7: Village.
- Surface Level 8: Barracks and Walls.
- Surface Level 9: Fortress.
- Surface Level 10: Final Boss.

### Content

Surface levels should introduce human counter-design:

- farmers and villagers as weak groups;
- dogs to chase fast builds;
- archers to punish large/slow builds;
- spearmen to stop charges;
- crossbowmen to punish flight;
- alchemists to counter regeneration;
- priests to counter plague/psychic;
- knights and paladins as elite threats.

### Special Mechanics

- Village bells call reinforcements.
- Barracks introduce nets and anti-build tools.
- Fortress introduces artillery or sacred zones.

### Boss

Surface Level 10 ends with **Sir Aldric, the Radiant Lance**.

Prototype phases:

- mounted lance combat;
- sacred magic zones;
- radiant fused knight/horse final form.

### Assets

Required by the end of this slice:

- villagers/farmers;
- dogs or hounds;
- archers/crossbowmen;
- spearmen/soldiers;
- priests or holy casters;
- knights/paladins;
- Sir Aldric mounted or equivalent staged boss asset;
- village, barracks, fortress props.

### Acceptance Criteria

- The surface feels different from the dungeon.
- The player feels powerful at first, then challenged by human adaptation.
- Sir Aldric is a memorable final boss target.
- A full run can be completed from Dungeon Level 5 to Surface Level 10.

### Verification

- Surface smoke test.
- Full run smoke test.
- Manual full run playthrough.

## Slice 8: Modular Evolution Foundation

### Goal

Build the real mutation framework after the first level and progression base are stable.

### Content

- Mutation card data model.
- Body slots:
  - head;
  - mouth;
  - eyes;
  - arms;
  - legs;
  - back;
  - tail;
  - torso;
  - organs;
  - blood;
  - aura.
- Card types:
  - body;
  - organ;
  - instinct;
  - unstable.
- Mutation tags.
- Slot conflicts.
- Larva/Predator/Abomination form restrictions.
- Evolve vs delay choice.

### First Card Set

- Primary Claws.
- Predatory Legs.
- Segmented Tail.
- Membrane Wings.
- Toxic Glands.
- Carapace.
- Neural Tentacles.
- Threadlike Body.

### Assets

Use simple modular visual attachments first:

- claw mesh or fallback;
- tail mesh or fallback;
- wing mesh or fallback;
- carapace plates;
- tentacle pieces;
- eye/head markers;
- aura/VFX.

### Acceptance Criteria

- Cards record tags and body slots.
- Slot conflicts are enforced.
- At least 4 cards visibly change the player.
- The player can choose to evolve or delay at a threshold.

### Verification

- Mutation smoke test.
- Visual check for body changes.
- `SmokeTestAll.bat`

## Slice 9: Archetypes and Build Identity

### Goal

Make classes emerge from mutation choices.

### First Archetypes

- Alien Dragon.
- Manticore.
- Broodmother.
- Armored Behemoth.
- Reaper.
- Plague Horror.
- Mind Parasite.
- Abyssal Horror.
- Wyvern.
- Chimera.

### Content

- Tag threshold counting.
- Primary archetype detection.
- Secondary archetype detection if feasible.
- Archetype-specific card offers.
- Archetype-specific buffs and weaknesses.
- Build label in HUD/debug UI.

### Assets

Initial archetype visuals can be readable combinations of modular parts, not unique full-body meshes.

### Acceptance Criteria

- At least 3 archetypes can unlock.
- Archetypes change gameplay, not only text.
- Mixed builds can become Chimera.
- The player can understand their build direction from UI and body silhouette.

### Verification

- Archetype unlock smoke tests.
- Manual build path tests.

## Slice 10: UI, Card Presentation, and Run Readability

### Goal

Replace debug messages with a playable interface.

### Content

- HUD:
  - health;
  - stamina/energy;
  - biomass;
  - current level;
  - current form;
  - current archetype;
  - enemy count or wave state.
- Card selection screen:
  - 3 card choices;
  - body slot;
  - tags;
  - visible effect summary;
  - conflict warning;
  - form requirement.
- Death screen.
- Level complete screen.
- Run complete screen.

### Assets

- UI frame;
- card backgrounds by card type;
- tag icons;
- body slot icons;
- mutation rarity icons.

### Acceptance Criteria

- The player can understand cards without reading logs.
- Slot conflicts are visible before selection.
- The UI supports controller/mouse future extension without redesign.

### Verification

- UI screenshot pass.
- Card selection smoke test.
- Manual playthrough.

## Slice 11: Audio, VFX, and Game Feel

### Goal

Make the game satisfying and readable moment to moment.

### Content

- Player movement sound.
- Bite/claw/impact sounds.
- Enemy hit/death sounds.
- Biomass pickup sound.
- Mutation sound.
- Boss phase sounds.
- Ambience per level type.
- VFX for:
  - hit;
  - bleed;
  - poison;
  - fire;
  - psychic;
  - biomass;
  - mutation;
  - boss phase transitions.

### Acceptance Criteria

- Hits feel clear.
- Pickups feel rewarding.
- Mutation choices feel important.
- Dungeon and surface have different moods.

### Verification

- Manual feel pass.
- No severe audio spam in horde scenarios.
- Performance check.

## Slice 12: Balance and Counter-Design

### Goal

Make builds powerful but not automatic wins.

### Content

- Enemy counters by build type.
- Boss resistance profiles.
- Mutation rarity and risk/reward tuning.
- Biomass economy.
- Evolution timing.
- Difficulty curve from Dungeon Level 5 to Surface Level 10.

### Acceptance Criteria

- At least 3 build paths can complete the run.
- Bad builds can fail, but failure feels explainable.
- Surface enemies adapt to common overpowered patterns.
- Bosses test different build needs.

### Verification

- Multiple manual run paths.
- Smoke tests for representative archetypes.
- Balance notes after play sessions.

## Slice 13: Optimization and Packaging

### Goal

Make the game stable enough to distribute.

### Content

- Asset size audit.
- Git LFS/Perforce or curated asset commit decision.
- Cook/package pipeline.
- Load-time check.
- Runtime performance check.
- Crash pass.
- Input/settings basics.

### Acceptance Criteria

- Packaged build launches.
- First level loads reliably.
- Full run can complete in packaged build.
- Asset repository policy is clear.

### Verification

- Packaged build test.
- Full-run smoke or manual packaged run.
- Asset size report.

## Slice 14: Content Completion

### Goal

Finish all planned level, enemy, boss, card, and asset content.

### Required Completion

- 10 run levels.
- Dungeon midpoint boss: The Buried Warden.
- Final boss: Sir Aldric, the Radiant Lance.
- Core enemy roster for dungeon and surface.
- Core mutation card pool.
- Core archetype pool.
- Finalized first-level art.
- Acceptable art coverage for all later levels.
- Full audio/VFX pass.
- Full UI pass.

### Acceptance Criteria

- The full game can be played from start to final boss.
- The first level looks good enough to sell the fantasy immediately.
- The evolution system delivers visible body changes.
- The player can make meaningfully different builds.
- The game remains C++ first with clean Blueprint extension points.

### Verification

- Full manual playthrough.
- Full smoke suite.
- Packaged build playthrough.
- Asset/license audit.

## Slice 15: Release Candidate

### Goal

Prepare the project for an external demo, playtest, or release candidate.

### Content

- Bug fixing.
- Balance pass.
- Crash fixes.
- UI polish.
- Options menu basics.
- Credits/license documentation.
- Trailer/screenshot capture level.
- Steam/demo checklist if needed.

### Acceptance Criteria

- No known blocker bugs.
- First 10 minutes are compelling.
- First level is visually strong.
- Full run is completable.
- Asset licenses are documented.
- Build can be shared.

### Verification

- Release checklist.
- Fresh packaged build.
- External playtest notes.

## Immediate Production Priority

The next real production milestone should be **Slice 2 + Slice 3**:

1. Curate and import enough assets to make Dungeon Level 5 visually strong.
2. Retheme the current runtime away from surface humans and into dungeon enemies.
3. Build the starting chamber with up/down choice.
4. Finish Dungeon Level 5 as a complete, attractive, playable level.

Do not start the full modular evolution system before Dungeon Level 5 is attractive and shippable as a first impression.
