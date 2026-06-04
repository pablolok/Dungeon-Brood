# Dungeon Level 5 Layout Reference

## Source

Reference images provided by the user (latest polished concept map confirmed 4 June 2026).

- **Canonical design map**: save the polished infographic as
  `docs/reference/level5_design_map.png` (the dark fantasy map titled
  "DUNGEON LEVEL 5 - LA SOGLIA SPEZZATA"). This file is the single visual source of
  truth for the Level 5 layout. See "Extracted Map Data" below for its full content.
- The image is a conceptual roguelike map, **not to scale, modular and variable**, but
  the structural relationships (spine + symmetric side rooms) must be respected.

This is the target layout for `Dungeon Level 5 - La Soglia Spezzata`.

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

## Extracted Map Data

Full transcription of the canonical design map (`docs/reference/level5_design_map.png`).
Italian flavor text is kept verbatim because it is the intended in-game tone.

### Header

- Title: **DUNGEON LEVEL 5 - LA SOGLIA SPEZZATA**
- Intro: "Detriti, catene e radici marciscono queste prigioni dimenticate. In superficie,
  la liberta' chiama... ma il corpo e' debole, la mutazione e' appena iniziata. Scendi piu'
  in profondita', o tenta la tentazione?"
- Footer: "LAYOUT CONCETTUALE ROGUELIKE. Non in scala. Modulare e variabile." (compass: N up)

### Legend (node icons)

| Icon | Meaning |
| --- | --- |
| Green larva/snake | Punto di partenza (player start) |
| Orange up arrow | Scala verso superficie (temptation) |
| Blue down arrow | Scala verso livello 4 (progression) |
| White skull | Area nemici normali |
| Red skull | Area elite (dangerous) |
| Dashed outline | Stanza opzionale (optional room) |
| Purple "?" | Stanza evento / mutazione |
| Green leaf | Bacino di biomassa |
| Dashed line | Corridoio principale (main corridor) |

### Nodes (spatial position, type, enemies, flavor)

Spine runs top -> bottom. Side rooms are symmetric: three on the left, three on the right,
each linked to the spine by a short corridor.

| Node | Position | Type | Enemies / content | Flavor (verbatim) |
| --- | --- | --- | --- | --- |
| Scala verso la Superficie | Top, on spine | Temptation stair (up) | none (lethal if entered early) | "Vedi la luce. Ma sei troppo debole per uscirne vivo." |
| Sala della Caduta | Upper spine | **Player start** | Resti, tutorial, primi nemici | "Il larva nasce qui, ai piedi della via d'uscita tentatrice." |
| Celle Crollate | Upper left (optional) | Normal | Scheletri rotti, cadaveri striscianti | "Resti di prigionieri abbandonati." |
| Corridoio delle Catene | Upper right (optional) | Normal | Topi mutati, scheletri rotti | "Catene che tintinnano e intralciano il passo." |
| Sala dell'Acqua | Mid left (optional) | Normal | Melme piccole, pipistrelli ciechi | "Acqua stagnante e correnti infette." |
| Sala Centrale | Center, on spine | Normal hub / first arena | Mix di tutti i nemici del livello | "Prima arena. Punto di snodo principale." (octagonal) |
| Stanza Evento / Mutazione | Mid right (optional) | Event / mutation | none (choice/card/rare event) | "La mutazione offre potere... o un prezzo da pagare." |
| Sala delle Radici | Lower left (optional) | Normal | Pipistrelli, topi mutati | "Radici mobili e trappole naturali." |
| Bacino di Biomassa | Lower right (optional) | Biomass reward | none (heal/risk) | "La biomassa muta chi vi sosta troppo a lungo." |
| Arena del Carceriere | Lower spine | **Elite** | Jailer Husk + biomass/traps | "Biomassa, trappole, attacchi devastanti. Carta garantita alla vittoria." (octagonal) |
| Scala verso Livello 4 | Bottom, on spine | Progression stair (down) | opens after Jailer Husk | "La discesa continua. Nuove prigioni piu' in basso." |

### Design notes (procedural intent)

From the "NOTE DI PROGETTAZIONE" panel:

- Layout concettuale roguelike: the **general structure stays**, but secondary rooms and some
  connections may vary per run.
- Optional rooms can change position or be absent.
- Connections, enemies and rewards vary to keep each run unique.
- The surface temptation is always present, but not always reachable.

Implication for implementation: build the **spine + the full set of side rooms as authored
content** for the vertical slice now; the per-run shuffling of optional rooms is a later
system, not required for the first playable Level 5.

## Mapping To The Current Builder

`.agents/unreal_scripts/build_level5_editor_map.py` now builds the **full design-map layout**
on a global 600 tile lattice using a tile-occupancy model with automatic openings (a wall is
placed only where a tile face borders an empty tile, so doorways and corridors line up by
construction). All eleven design nodes are present:

- Spine (along +X): `SalaCaduta` (start) -> `SalaCentrale` (hub) -> `ArenaCarceriere` (elite)
  -> `ScalaLivello4` (descent room), with `SurfaceTemptation` stair off the start and
  `Level4_Doorway` off the east room.
- North side rooms: `CelleCrollate`, `SalaAcqua`, `SalaRadici`.
- South side rooms: `CorridoioCatene`, `StanzaEvento`, `BacinoBiomassa`.
- Side rooms attach to the spine via one-tile corridors (`C_*` rects).

Validated clean by `validate_level5_editor_map.py` (75 floor tiles, no floor/wall overlaps,
no gaps, no floating art, PlayerStart + collision present). Run headless via
`UnrealEditor.exe <uproject> -ExecutePythonScript=...` (engine at `E:\EpicGames\UE_5.7`).

Gameplay wiring (`Source/.../BroodGameMode.cpp`): wave encounters 0-2 spawn inside
`SalaCentrale`; the Jailer Husk boss spawn is relocated to `ArenaCarceriere` on the editor
map (`(3900, 0, 120)`), keeping the runtime-blockout spawn unchanged.

Lighting follows the colour rules above as **subtle accents over a neutral warm base** (stone
must read as stone): green=start/biomass, blue=descent, teal=water, purple=event, amber=roots,
warm=torches, ominous warm=elite arena.

### Not yet done (future polish)
- `SalaCentrale` and `ArenaCarceriere` are rectangular; the map draws them octagonal. A
  chamfered-corner octagon (45-degree wall pieces + corner pillars to mask seams) is a later
  pass once the kit chamfer is verified clean.
- Per-run shuffling of optional rooms (design note) is a later system; the slice authors all
  rooms as fixed content.
