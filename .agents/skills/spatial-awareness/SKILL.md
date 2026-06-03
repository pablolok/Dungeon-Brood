---
name: spatial-awareness
description: Use when building, composing, or correcting 3D scenes, dungeon rooms, arenas, corridors, landmarks, set dressing, modular kits, or asset placement in Dungeon-Brood. Applies before placing visual assets or assembling scene objects in Unreal; does not apply to pure gameplay code with no scene layout or transform work.
---

# Spatial Awareness

Use this skill to make 3D scene work deliberate instead of trial-and-error. The goal is a playable, readable scene with correct scale, navigation, collision, silhouette, and asset intent. Be direct when the current result is not working; do not rationalize a broken scene as acceptable progress.

## Project Constraints

- Target project: `Dungeon-Brood`, Unreal Engine 5.7, C++ first.
- Prefer code-spawned test content and editor scripting while the vertical slice is unstable.
- Keep Blueprint extension points minimal.
- Avoid large procedural systems, asset-heavy content, marketplace dependencies, complex animation requirements, inventory, saves, and multiplayer.
- Work in small batches that can be visually verified.

## Core Rule

Do not place or assemble assets blindly. If a current scene, screenshot, viewport capture, or level already exists, diagnose it before adding anything else.

Before adding scene objects, establish:

1. The playable purpose of the space.
2. The player scale reference.
3. The asset role.
4. The placement grid or transform convention.
5. The validation view or screenshot that will prove the placement works.

If those five facts are not known, stop generating scene placement and gather them first.

## Current Scene Triage Gate

When given an existing scene or screenshot, start with a blunt triage:

1. Is this recognizably the intended space type from the first readable screenshot?
2. Does the architecture define a coherent room, corridor, dungeon hall, arena, or threshold?
3. Are walls, floors, columns, arches, doors, and props grounded, aligned, and scaled plausibly?
4. Is there a navigable path with deliberate boundaries?
5. Are props supporting the space, or are they scattered like unrelated kit pieces?
6. Does lighting or contrast guide attention?
7. Are there obvious floating, clipping, upside-down, oversized, undersized, or randomly rotated objects?

Classify the result as:

- `PASS`: recognizable space, minor cleanup only.
- `WEAK`: recognizable space but poor composition, needs targeted repair.
- `FAIL_ASSET_DUMP`: not a coherent dungeon or gameplay space; stop placement and repair the foundation.
- `FAIL_TECHNICAL`: transform, scale, collision, pivot, or asset-loading problems make evaluation unreliable.

If the result is `FAIL_ASSET_DUMP` or `FAIL_TECHNICAL`, do not continue dressing the scene. Explain the failure in concrete spatial terms, then switch to the repair protocol.

## Dungeon Recognition Rubric

A dungeon scene must read as built space, not just stone materials on a plane.

Required signals:

- Enclosure: walls, arches, columns, ceiling hints, gates, stairs, pits, or height changes define bounded space.
- Structure: architectural pieces align to a grid or believable construction logic.
- Depth: foreground, midground, and background layers create a clear spatial read.
- Path: the player can infer where they came from and where they can go.
- Scale: doors, walls, props, and openings make sense relative to the player.
- Anchoring: props sit on floors, lean on walls, hang from supports, or cluster around functional areas.
- Focal point: a door, altar, gate, bridge, enemy space, light shaft, or landmark pulls the eye.
- Atmosphere: lighting, shadow, fog, candles, grime, water, or debris reinforces the dungeon identity.

Failure signals:

- Huge flat tiled floor with disconnected props.
- Wall slabs floating, clipping, or placed like showroom panels.
- Decorative bones, chains, bars, planks, or debris scattered without architectural purpose.
- No ceiling, columns, doorways, vertical rhythm, or enclosed volume.
- Props blocking readability instead of clarifying space.
- Repetition that looks stamped instead of constructed.
- A reference image clearly reads as a dungeon, while the current scene reads as an asset test map.

When failure signals dominate, say so and repair the layout. Do not describe the scene as a dungeon just because it uses stone assets.

## Reference Comparison

When a reference image is provided, extract reusable spatial principles rather than copying the image exactly.

Compare the current scene against the reference for:

- Silhouette: columns, arches, openings, ceiling mass, and major vertical shapes.
- Composition: strong foreground/midground/background separation.
- Light: warm/cool contrast, pools of light, readable focal path.
- Density: set dressing clustered around believable support points instead of evenly scattered.
- Construction logic: pieces look supported by gravity and masonry.
- Negative space: playable floor is intentionally clear, not empty by accident.

Write a short delta:

- `Reference has...`
- `Current scene lacks...`
- `Repair target...`

Then implement only the repair target for the next batch.

## Required Workflow

### 1. Define The Spatial Intent

Write a short intent before placing assets:

- Space type: room, corridor, arena, threshold, landmark, vista, cover cluster, set dressing, prop assembly.
- Gameplay job: navigation, combat, telegraph, obstruction, reveal, pressure, reward, orientation.
- Player relationship: where the player enters, where they look first, where they can move, where they should not move.
- Camera expectation: third-person readability, no dense foreground clutter, clear silhouette from common approach angles.

### 2. Audit Available Assets

Inspect assets before use. Prefer actual asset metadata over names alone.

For each candidate asset, determine:

- Role: floor, wall, ceiling, arch, column, door, stair, rail, prop, body part, landmark, dressing, blocker, VFX anchor.
- Approximate dimensions and bounding box.
- Pivot location and orientation.
- Collision behavior.
- Material/readability role.
- Whether it is modular, decorative, or a one-off hero piece.

Reject assets that cannot be understood well enough for controlled placement. Use simple proxy geometry until the asset can be inspected.

### 3. Block Out First

For new spaces, create a functional layout before art dressing:

- Floor footprint.
- Walls or boundaries.
- Entry and exit.
- Combat or interaction area.
- Player start and approach path.
- Navigation clearance.
- Major landmarks.

Use simple shapes or known modular pieces first. Do not solve composition by adding more decorative assets.

For failed existing scenes, rebuild the blockout in place:

- Preserve only the floor footprint, player start, essential gameplay actors, and any clearly useful architectural anchors.
- Temporarily remove or quarantine scattered props that do not support the room identity.
- Establish walls, columns, gates, arches, thresholds, and vertical scale before reintroducing debris.
- Add one focal landmark and one readable route before adding atmosphere.

### 4. Place By Constraints

Use explicit transform rules:

- Snap modular architecture to a consistent grid.
- Keep rotations in predictable increments unless the object is intentionally organic.
- Align floors, walls, doors, stairs, and large props by bounding box and pivot, not by visual guesswork.
- Keep walkable paths clear for the character capsule.
- Use repeated modules consistently before introducing variation.
- Avoid intersecting visible surfaces unless the overlap is hidden or intentional.

When composing organic or irregular assets, define anchor points first:

- Base contact point.
- Facing direction.
- Primary silhouette direction.
- Clearance volume.
- Attachment or overlap region.

### 5. Batch Size Limit

Place only a small batch before validation:

- Architecture: one room, corridor segment, or threshold at a time.
- Set dressing: 3-8 related props at a time.
- Complex assemblies: one structural layer at a time, such as base, torso, head, arms, spikes, or dressing.

After each batch, validate before continuing.

### 6. Validate Spatially

A batch is not complete until checked against these criteria:

- Scale: objects make sense next to the player.
- Walkability: player path and combat space remain usable.
- Collision: no accidental blockers in main routes.
- Silhouette: primary forms read from approach angles.
- Camera: third-person view is not blocked by noisy foreground clutter.
- Orientation: exits, threats, and goals are visually legible.
- Repetition: modular pieces look intentional, not randomly stamped.
- Performance: no excessive dense asset use for an early vertical slice.

Prefer screenshots or viewport captures from fixed angles:

- Player entry view.
- Reverse view.
- Top-down or orthographic layout view.
- Combat/readability view.
- Close inspection view for seams and intersections.

### 7. Fix Before Expanding

If validation fails, do not add more assets. Fix the current batch first.

Common fixes:

- Replace with simpler proxy geometry.
- Move to grid.
- Correct pivot or rotation.
- Increase clearance.
- Remove decorative clutter.
- Split one overloaded space into smaller readable zones.
- Turn a failed hero object into background dressing.

## Repair Protocol For Failed Dungeon Scenes

When triage returns `FAIL_ASSET_DUMP`, repair in this order:

1. Remove noise: hide or delete loose props, random bones, planks, chains, bars, decals, and floating pieces that do not define structure.
2. Rebuild enclosure: create a coherent room or corridor with grounded walls, corner logic, and thresholds.
3. Add vertical rhythm: columns, arches, buttresses, ceiling hints, raised platforms, or broken height changes.
4. Define path: entry, exit, blocked edges, combat space, and sightline to the next goal.
5. Add focal point: gate, altar, light shaft, cage, bridge, boss space, treasure, or traversal objective.
6. Reintroduce dressing in clusters: debris near walls, bones near a cage or altar, chains attached to supports, candles near landmarks.
7. Validate with screenshots before adding more.

The expected repair output is not "more detailed"; it is "more recognizably a dungeon."

### 8. Stop Conditions

Stop and report the issue when:

- Asset names are too ambiguous to infer role.
- Bounding boxes, pivots, or collision are unknown and cannot be inspected.
- The scene requires a visual judgment but no screenshot or viewport validation is available.
- Three consecutive placement attempts fail the same criterion.
- The requested result depends on complex animation, rigging, or asset authoring outside the current vertical-slice scope.

## Unreal Notes

- Use Unreal actor transforms intentionally: location, rotation, scale, mobility, collision, and labels.
- Prefer Static Mesh Actors or simple C++ spawned actors for early layout validation.
- Use editor scripting or deterministic C++ helpers when many transforms must be generated.
- Keep actor labels meaningful enough to inspect in the World Outliner.
- Use debug logs/messages for generated layout summaries: count, asset role, bounds, and validation result.
- Run the project build or relevant smoke test when scene placement changes gameplay, collision, spawning, or level flow.

## Output Expectations

When using this skill, include a short spatial summary:

- Current scene classification, when evaluating existing work.
- Intent.
- Assets or proxies used.
- Placement rules.
- Validation performed.
- Known visual compromises.
- Next correction, if any.

Do not claim a scene is good, readable, or fixed without validation evidence.
