# Dungeon Level 5 Layout Reference

## Source

Reference image provided by the user on 3 June 2026.

This is the target layout for `Dungeon Level 5 - La Soglia Spezzata`.
The image is a conceptual roguelike map, not exact scale, but the structural relationships must be respected.

## Core Layout

The level is organized around one vertical main corridor:

1. Top: `Scala verso la superficie` as temptation route.
2. Upper room: `Sala della Caduta`, the player start.
3. Center: narrow main corridor leading down.
4. Middle: large circular/octagonal `Sala Centrale`, the first arena and main hub.
5. Lower room: `Arena del Carceriere`, elite Jailer Husk encounter.
6. Bottom: `Scala verso livello 4`, progression route.

The main path must read clearly from top to bottom.
Secondary rooms attach to the main spine through side corridors.

## Required Rooms

### Sala della Caduta

- Player start.
- Close to the surface staircase.
- Tutorial/rest area near the tempting exit.
- Green/biomass start marker.
- Should feel like the larva just fell into the dungeon.

### Scala verso la Superficie

- Top of the map.
- Warm upward light.
- Temptation route, not main progression.
- Player can see it, but it is dangerous/too early.

### Sala Centrale

- Main combat arena.
- Circular or octagonal footprint.
- Central combat marker.
- Connects to optional rooms and to the Jailer route.
- Must be wide enough for combat and readable movement.

### Arena del Carceriere

- Large lower room.
- Elite/boss encounter.
- Heavier prison dressing: chains, cages, blood, biomass, traps.
- Guaranteed victory reward/card after Jailer Husk.

### Scala verso Livello 4

- Bottom of the map.
- Blue/cold progression marker.
- Opens after Jailer Husk.
- Must be a clear descent, not a decorative side prop.

## Optional/Side Rooms

These may vary per run later, but the current vertical slice should block them in as authored rooms:

- `Celle Crollate`: upper-left room with broken cells, skeletons, collapsed prisoners.
- `Sala dell'Acqua`: left-middle room with stagnant water, small enemies, rot mood.
- `Sala delle Radici`: lower-left room with roots, natural traps, mutated rats.
- `Corridoio delle Catene`: upper-right side corridor/room with hanging chains and small enemies.
- `Stanza Evento / Mutazione`: right-middle optional event room, purple mood.
- `Bacino di Biomassa`: lower-right reward/heal/risk room, green mood.

## Flow

Required main flow:

`Sala della Caduta` -> `Sala Centrale` -> `Arena del Carceriere` -> `Scala verso livello 4`.

Optional routes must branch from the main path, then return or dead-end cleanly.
They must not break the readability of the main corridor.

## Lighting Rules

- Warm light comes from torches, candles, braziers, or visible lamps.
- Blue/cold light is reserved for descent/progression and unlit depth.
- Green light is reserved for larva start and biomass.
- Purple light is reserved for event/mutation room.
- No unsupported floating point lights.

## Scene-Building Rules

- Build layout first: floors, walls, doorways, collision, main path.
- Do not place props until the room footprint is correct.
- Every room must be enclosed enough to read as dungeon, not a grid floor.
- Doorways and stairs must be verified by bounds and visual screenshot before being accepted.
- No floating or inverted architectural assets.
- No overlapping stair systems.
- The player must always stand on floor, not ceiling or hidden proxy surfaces.

## Current Restyling Goal

Replace the current corridor-only Level 5 with this readable top-down structure:

- start room near top;
- central arena;
- lower Jailer arena;
- bottom descent;
- side optional rooms arranged around the main spine.

The first implementation can use simple rectangular/octagonal rooms. Final polish can replace blockout walls with richer MedievalDungeon kit dressing after the layout is correct.
