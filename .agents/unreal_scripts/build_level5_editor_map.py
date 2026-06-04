"""Deepborn - Dungeon Level 5 editor map builder (full design-map layout).

Method (proven on the 3-room slice, now scaled to the full map)
---------------------------------------------------------------
* ONE modular family (MedievalDungeon "Dungeon" set) on a single global 600 tile lattice,
  everything at scale 1.0.
* Each mesh's real bounds + pivot are measured at runtime; `place()` takes the desired
  bounding-box CENTRE and compensates pivot + yaw, so a piece lands exactly where asked
  regardless of where its pivot sits (kit pivots are at corners).
* Layout is a set of tile RECTS (rooms + 1-tile corridors) on the shared lattice. Walls are
  placed only on a tile face whose neighbour tile is EMPTY -> doorways and corridors line up
  by construction and there are never double walls or misaligned openings.

Layout = the canonical design map (docs/DUNGEON_LEVEL5_LAYOUT_REFERENCE.md):
  Spine (along +X):  SalaCaduta(start) -> SalaCentrale(hub) -> ArenaCarceriere(elite) -> ScalaLivello4
  North side rooms:  CelleCrollate, SalaAcqua, SalaRadici
  South side rooms:  CorridoioCatene, StanzaEvento, BacinoBiomassa
  Stairs:            SurfaceTemptation (up, off start), Level4Descent (down, off east room)

Gameplay contract (ABroodGameMode): map name contains L_Dungeon_Level5_Editor, a PlayerStart,
floor collision under it, and open space for the fixed wave spawns (X 540..1000, Y +-340) which
land inside SalaCentrale. (The Jailer boss spawn is relocated to ArenaCarceriere in C++.)
"""

import math

import unreal


MAP_PATH = "/Game/Deepborn/Maps/L_Dungeon_Level5_Editor"
PREFIX = "DB_L5Pack_"
CLEANUP_PREFIXES = ("DB_L5Pack_", "DB_MCP_L5_", "DB_MCP_")

TILE = 600.0
WALL_H = 600.0
WALL_THICK = 80.0

LEVEL_SUBSYSTEM = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
ACTOR_SUBSYSTEM = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


MESHES = {
    "cube": "/Engine/BasicShapes/Cube.Cube",
    "floor": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor",
    "wall": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall",
    "wall_decor": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall_Decorative_A.SM_Dungeon_Wall_Decorative_A",
    "pillar": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar",
    "ceiling": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat",
    "doorway": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay",
    "stairs_10": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_10.SM_Stairs_10",
    "torch": "/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch",
    "skeleton": "/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_01.SM_Skeleton_01",
    "coffin_open": "/Game/MedievalDungeon/Meshes/Props/SM_Coffin_No_Lid.SM_Coffin_No_Lid",
    "cell_front": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front",
    "cell_door": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door",
    "gibbet_open": "/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Open.SM_Gibbet_Cage_Open",
    "ball_chain": "/Game/MedievalDungeon/Meshes/Props/SM_Ball_And_Chain.SM_Ball_And_Chain",
    "pot_a": "/Game/MedievalDungeon/Meshes/Props/SM_Pot_A_Complete.SM_Pot_A_Complete",
    "pot_b": "/Game/MedievalDungeon/Meshes/Props/SM_Pot_B_Complete.SM_Pot_B_Complete",
    "fire_pit": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Fire_Pit.SM_Fire_Pit",
    "statue": "/Game/MedievalDungeon/Meshes/Props/SM_Statue_Hooded.SM_Statue_Hooded",
    "statue_stand": "/Game/MedievalDungeon/Meshes/Props/SM_Statue_Stand.SM_Statue_Stand",
    "stocks": "/Game/MedievalDungeon/Meshes/Props/SM_Stocks_Game_Ready.SM_Stocks_Game_Ready",
    "torture_chair": "/Game/MedievalDungeon/Meshes/Props/SM_Torture_Chair.SM_Torture_Chair",
    "table": "/Game/MedievalDungeon/Meshes/Props/SM_Table.SM_Table",
    "wood_a": "/Game/MedievalDungeon/Meshes/Props/SM_Wood_A.SM_Wood_A",
    "dirt": "/Game/MedievalDungeon/Meshes/Props/SM_Dirt_Mound_A.SM_Dirt_Mound_A",
}


# ----------------------------------------------------------------------------- helpers

def log(message):
    unreal.log("DB_LEVEL5_EDITOR_BUILD: " + message)


def ensure_directory(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def load_mesh(key):
    mesh = unreal.load_asset(MESHES[key])
    if mesh is None:
        raise RuntimeError("Missing mesh asset: {} -> {}".format(key, MESHES[key]))
    return mesh


def label(base):
    return "{}{}".format(PREFIX, base)


def set_folder(actor, folder):
    try:
        actor.set_folder_path(folder)
    except Exception:
        pass


def configure_collision(actor, enabled):
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if not component:
        return
    component.set_collision_enabled(
        unreal.CollisionEnabled.QUERY_AND_PHYSICS if enabled else unreal.CollisionEnabled.NO_COLLISION
    )


_MESH_DIMS = {}


def measure(key):
    if key in _MESH_DIMS:
        return _MESH_DIMS[key]
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_object(
        load_mesh(key), unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0)
    )
    actor.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))
    origin, extent = actor.get_actor_bounds(False)
    dims = (extent.x * 2.0, extent.y * 2.0, extent.z * 2.0, origin.x, origin.y, origin.z)
    ACTOR_SUBSYSTEM.destroy_actor(actor)
    _MESH_DIMS[key] = dims
    return dims


def _yaw_rotate(x, y, yaw_deg):
    rad = math.radians(yaw_deg)
    c, s = math.cos(rad), math.sin(rad)
    return (x * c - y * s, x * s + y * c)


def place(key, base, center_x, center_y, yaw=0.0, base_z=0.0, folder="DungeonLevel5",
          collision=False, ground=True):
    sx, sy, sz, px, py, pz = measure(key)
    center_z = base_z + (sz / 2.0) if ground else base_z
    off_x, off_y = _yaw_rotate(px, py, yaw)
    loc = unreal.Vector(center_x - off_x, center_y - off_y, center_z - pz)
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_object(load_mesh(key), loc, unreal.Rotator(0.0, yaw, 0.0))
    actor.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))
    actor.set_actor_label(label(base), mark_dirty=True)
    set_folder(actor, folder)
    configure_collision(actor, collision)
    return actor


def spawn_collision(base, center, size, folder="DungeonLevel5/GameplayCollision"):
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_object(
        load_mesh("cube"), unreal.Vector(center[0], center[1], center[2]), unreal.Rotator(0.0, 0.0, 0.0)
    )
    actor.set_actor_scale3d(unreal.Vector(size[0] / 100.0, size[1] / 100.0, size[2] / 100.0))
    actor.set_actor_label(label("COL_" + base), mark_dirty=True)
    set_folder(actor, folder)
    configure_collision(actor, True)
    actor.set_actor_hidden_in_game(True)
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if component:
        component.set_visibility(False)
        component.set_editor_property("hidden_in_game", True)
    return actor


def spawn_point_light(base, location, color, intensity=4000.0, radius=1400.0,
                      folder="DungeonLevel5/Lighting"):
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_class(
        unreal.PointLight, unreal.Vector(location[0], location[1], location[2]), unreal.Rotator(0, 0, 0)
    )
    actor.set_actor_label(label(base), mark_dirty=True)
    actor.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    actor.light_component.set_editor_property("intensity", intensity)
    actor.light_component.set_editor_property("attenuation_radius", radius)
    try:
        actor.light_component.set_light_color(
            unreal.LinearColor(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0, 1.0), True)
    except Exception:
        pass
    set_folder(actor, folder)
    return actor


def spawn_directional_light(base, rotation, color=(225, 232, 255), intensity=2.4):
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_class(
        unreal.DirectionalLight, unreal.Vector(0, 0, 2600), unreal.Rotator(rotation[0], rotation[1], rotation[2])
    )
    actor.set_actor_label(label(base), mark_dirty=True)
    actor.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    actor.light_component.set_editor_property("intensity", intensity)
    try:
        actor.light_component.set_light_color(
            unreal.LinearColor(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0, 1.0), True)
    except Exception:
        pass
    set_folder(actor, "DungeonLevel5/Lighting")
    return actor


def spawn_player_start(location, yaw=0.0):
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_class(
        unreal.PlayerStart, unreal.Vector(location[0], location[1], location[2]), unreal.Rotator(0.0, yaw, 0.0)
    )
    actor.set_actor_label(label("PlayerStart_Level5"), mark_dirty=True)
    set_folder(actor, "DungeonLevel5/Gameplay")
    return actor


def spawn_torch(base, center_x, center_y, yaw, color=(255, 150, 70), intensity=2600.0, radius=950.0,
                height=235.0):
    place("torch", base, center_x, center_y, yaw=yaw, base_z=height,
          folder="DungeonLevel5/Lighting/Torches", ground=False)
    spawn_point_light("{}Light".format(base), (center_x, center_y, height + 40.0),
                      color, intensity=intensity, radius=radius, folder="DungeonLevel5/Lighting/TorchLights")


# --------------------------------------------------------------------- layout (tiles)
# RECTS: name -> (i_min, i_max, j_min, j_max). Rooms are separated; 1-tile corridors bridge
# them. Spine runs along +X (west = surface side, east = descent). Side rooms branch +-Y.

ROOM_RECTS = {
    "SalaCaduta":      (-4, -3, -1,  0),   # start / threshold (west)
    "SalaCentrale":    (-1,  2, -2,  1),   # main hub + wave arena (holds fixed wave spawns)
    "ArenaCarceriere": ( 5,  7, -2,  1),   # elite Jailer arena
    "ScalaLivello4":   (10, 11, -1,  0),   # descent room (east)
    "CelleCrollate":   (-3, -2, -4, -3),   # north, near start
    "SalaAcqua":       (-1,  0, -5, -4),   # north, near hub
    "SalaRadici":      ( 5,  6, -5, -4),   # north, near elite
    "CorridoioCatene": (-3, -2,  3,  4),   # south, near start
    "StanzaEvento":    ( 0,  1,  4,  5),   # south, near hub
    "BacinoBiomassa":  ( 5,  6,  4,  5),   # south, near elite
}

CORRIDOR_RECTS = {
    "C_Caduta_Centrale":      (-2, -2,  0,  0),
    "C_Centrale_Carceriere":  ( 3,  4,  0,  0),
    "C_Carceriere_Livello4":  ( 8,  9,  0,  0),
    "C_Celle":                (-2, -2, -2, -1),
    "C_Acqua":                ( 0,  0, -3, -3),
    "C_Radici":               ( 6,  6, -3, -3),
    "C_Catene":               (-2, -2,  1,  2),
    "C_Evento":               ( 1,  1,  2,  3),
    "C_Bacino":               ( 6,  6,  2,  3),
}

ALL_RECTS = dict(ROOM_RECTS)
ALL_RECTS.update(CORRIDOR_RECTS)


def tile_center(i, j):
    return (i * TILE + TILE / 2.0, j * TILE + TILE / 2.0)


def rect_center(name):
    i0, i1, j0, j1 = ALL_RECTS[name]
    return ((i0 * TILE + (i1 + 1) * TILE) / 2.0, (j0 * TILE + (j1 + 1) * TILE) / 2.0)


def build_occupancy():
    occ = {}
    for name, (i0, i1, j0, j1) in ALL_RECTS.items():
        for i in range(i0, i1 + 1):
            for j in range(j0, j1 + 1):
                occ.setdefault((i, j), name)
    return occ


# ------------------------------------------------------------------------ architecture

def build_floors_and_ceilings(occ):
    for (i, j) in occ:
        cx, cy = tile_center(i, j)
        place("floor", "Floor_{}_{}".format(i, j), cx, cy, base_z=0.0,
              folder="DungeonLevel5/Architecture/Floors")
        place("ceiling", "Ceiling_{}_{}".format(i, j), cx, cy, base_z=WALL_H, ground=False,
              folder="DungeonLevel5/Architecture/Ceiling")


def build_perimeter_walls(occ):
    # A wall sits on any tile face whose neighbour tile is empty -> exterior of the whole
    # connected blob, leaving every room-to-corridor join open (the doorways).
    for (i, j) in occ:
        cx, cy = tile_center(i, j)
        x_left, x_right = i * TILE, (i + 1) * TILE
        y_top, y_bottom = j * TILE, (j + 1) * TILE
        if (i - 1, j) not in occ:
            place("wall", "WallW_{}_{}".format(i, j), x_left, cy, yaw=0.0,
                  folder="DungeonLevel5/Architecture/Walls")
            spawn_collision("WallW_{}_{}".format(i, j), (x_left, cy, WALL_H / 2.0), (WALL_THICK, TILE, WALL_H))
        if (i + 1, j) not in occ:
            place("wall", "WallE_{}_{}".format(i, j), x_right, cy, yaw=180.0,
                  folder="DungeonLevel5/Architecture/Walls")
            spawn_collision("WallE_{}_{}".format(i, j), (x_right, cy, WALL_H / 2.0), (WALL_THICK, TILE, WALL_H))
        if (i, j - 1) not in occ:
            place("wall", "WallN_{}_{}".format(i, j), cx, y_top, yaw=90.0,
                  folder="DungeonLevel5/Architecture/Walls")
            spawn_collision("WallN_{}_{}".format(i, j), (cx, y_top, WALL_H / 2.0), (TILE, WALL_THICK, WALL_H))
        if (i, j + 1) not in occ:
            place("wall", "WallS_{}_{}".format(i, j), cx, y_bottom, yaw=-90.0,
                  folder="DungeonLevel5/Architecture/Walls")
            spawn_collision("WallS_{}_{}".format(i, j), (cx, y_bottom, WALL_H / 2.0), (TILE, WALL_THICK, WALL_H))


def build_pillars():
    # Decorative pillars at room corners (mask wall seams, give vertical rhythm). Dedup exact
    # coincident positions between nearby rooms.
    seen = set()
    for name, (i0, i1, j0, j1) in ROOM_RECTS.items():
        corners = [(i0 * TILE, j0 * TILE), ((i1 + 1) * TILE, j0 * TILE),
                   (i0 * TILE, (j1 + 1) * TILE), ((i1 + 1) * TILE, (j1 + 1) * TILE)]
        for idx, (px, py) in enumerate(corners):
            key = (round(px), round(py))
            if key in seen:
                continue
            seen.add(key)
            place("pillar", "{}_Pillar_{}".format(name, idx), px, py,
                  folder="DungeonLevel5/Architecture/Pillars")


def build_floor_collision():
    for name in ALL_RECTS:
        i0, i1, j0, j1 = ALL_RECTS[name]
        cx, cy = rect_center(name)
        w = (i1 - i0 + 1) * TILE
        h = (j1 - j0 + 1) * TILE
        spawn_collision("{}_Floor".format(name), (cx, cy, -20.0), (w, h, 40.0))


# -------------------------------------------------------------------- stairs & dressing

def build_surface_temptation():
    # Off the start room's north edge: a doorway + short stair run rising toward daylight.
    cx, _ = rect_center("SalaCaduta")
    door_x, door_y = cx, (-1) * TILE  # SalaCaduta north boundary y = j_min*TILE = -600
    place("doorway", "SurfaceTemptation_Doorway", door_x, door_y, yaw=90.0,
          folder="DungeonLevel5/SurfaceStair")
    for step in range(5):
        place("stairs_10", "SurfaceTemptation_StairStep_{:02d}".format(step),
              door_x, door_y - 220.0 - step * 300.0, yaw=90.0, base_z=step * 90.0,
              folder="DungeonLevel5/SurfaceStair", ground=False)
    spawn_point_light("SurfaceTemptation_Daylight", (door_x, door_y - 1100.0, 380.0),
                      (255, 226, 170), intensity=6500.0, radius=2200.0)


def build_level4_descent():
    # Off the east room's east edge: doorway + cold blue depth glow (progression).
    i0, i1, j0, j1 = ROOM_RECTS["ScalaLivello4"]
    door_x = (i1 + 1) * TILE
    _, cy = rect_center("ScalaLivello4")
    place("doorway", "Level4_Doorway", door_x, cy, yaw=180.0, folder="DungeonLevel5/Level4Stair")
    spawn_point_light("Level4_DepthGlow", (door_x + 400.0, cy, 240.0), (55, 110, 210),
                      intensity=2600.0, radius=1500.0)


def place_dressing():
    # A small, themed prop set per room - grounded so nothing floats.
    cx, cy = rect_center("SalaCaduta")
    place("skeleton", "Caduta_Remains", cx - 200, cy + 180, yaw=24.0, folder="DungeonLevel5/SetDressing/Caduta")
    place("coffin_open", "Caduta_Coffin", cx + 220, cy - 160, yaw=-12.0, folder="DungeonLevel5/SetDressing/Caduta")

    cx, cy = rect_center("SalaCentrale")
    place("table", "Centrale_Table", cx, cy, yaw=12.0, folder="DungeonLevel5/SetDressing/Centrale")

    cx, cy = rect_center("CelleCrollate")
    place("cell_front", "Celle_Bars", cx - 250, cy, yaw=0.0, folder="DungeonLevel5/SetDressing/Celle")
    place("cell_door", "Celle_Door", cx + 80, cy + 180, yaw=70.0, folder="DungeonLevel5/SetDressing/Celle")
    place("skeleton", "Celle_Prisoner", cx + 200, cy - 150, yaw=40.0, folder="DungeonLevel5/SetDressing/Celle")

    cx, cy = rect_center("CorridoioCatene")
    place("gibbet_open", "Catene_Cage", cx - 220, cy, yaw=90.0, folder="DungeonLevel5/SetDressing/Catene")
    place("ball_chain", "Catene_Ball", cx + 180, cy + 120, yaw=20.0, folder="DungeonLevel5/SetDressing/Catene")

    cx, cy = rect_center("SalaAcqua")
    place("dirt", "Acqua_PoolProxy", cx, cy, yaw=0.0, folder="DungeonLevel5/SetDressing/Acqua")
    place("pot_a", "Acqua_PotA", cx - 240, cy - 160, yaw=18.0, folder="DungeonLevel5/SetDressing/Acqua")
    place("pot_b", "Acqua_PotB", cx + 220, cy + 140, yaw=-22.0, folder="DungeonLevel5/SetDressing/Acqua")

    cx, cy = rect_center("StanzaEvento")
    place("statue_stand", "Evento_Pedestal", cx, cy, yaw=0.0, folder="DungeonLevel5/SetDressing/Evento")
    place("statue", "Evento_Statue", cx, cy, yaw=180.0, base_z=70.0, ground=False,
          folder="DungeonLevel5/SetDressing/Evento")

    cx, cy = rect_center("SalaRadici")
    for k, (dx, dy, yaw) in enumerate([(-220, 60, 20), (180, -80, -18), (40, 200, 35)]):
        place("wood_a", "Radici_Beam_{}".format(k), cx + dx, cy + dy, yaw=yaw,
              folder="DungeonLevel5/SetDressing/Radici")

    cx, cy = rect_center("BacinoBiomassa")
    place("fire_pit", "Bacino_Basin", cx, cy, yaw=0.0, folder="DungeonLevel5/SetDressing/Bacino")

    cx, cy = rect_center("ArenaCarceriere")
    place("fire_pit", "Carceriere_Pit", cx, cy, yaw=0.0, folder="DungeonLevel5/SetDressing/Carceriere")
    place("stocks", "Carceriere_Stocks", cx - 520, cy - 260, yaw=22.0, folder="DungeonLevel5/SetDressing/Carceriere")
    place("torture_chair", "Carceriere_Chair", cx + 540, cy + 240, yaw=-28.0, folder="DungeonLevel5/SetDressing/Carceriere")
    place("ball_chain", "Carceriere_Ball", cx - 120, cy + 300, yaw=34.0, folder="DungeonLevel5/SetDressing/Carceriere")


def _soft(color, blend=0.62):
    # Blend a theme colour toward warm white so stone still reads as stone; colour is an
    # accent, not a flood (matches the design map where colour marks a room, not bathes it).
    warm = (255, 240, 215)
    return tuple(int(c * (1.0 - blend) + w * blend) for c, w in zip(color, warm))


def place_lighting():
    spawn_directional_light("KeyLight", rotation=(-52.0, 40.0, 0.0), intensity=1.9)

    # Each room gets a neutral warm base fill (readability) plus a smaller, low-intensity
    # themed accent (colour identity). Theme colours are softened toward warm white.
    rooms = [
        ("SalaCaduta",      (90, 220, 120), 1.0),   # green start
        ("SalaCentrale",    (255, 240, 215), 1.4),  # neutral warm hub (bigger)
        ("ArenaCarceriere", (255, 120, 80),  1.3),  # ominous warm (elite)
        ("ScalaLivello4",   (90, 140, 235),  1.0),  # cold descent
        ("CelleCrollate",   (235, 215, 180), 0.85),
        ("CorridoioCatene", (235, 215, 180), 0.85),
        ("SalaAcqua",       (70, 175, 210),  0.95), # teal water
        ("StanzaEvento",    (175, 90, 230),  0.95), # purple event
        ("SalaRadici",      (220, 160, 90),  0.9),  # amber roots
        ("BacinoBiomassa",  (90, 220, 90),   0.95), # green biomass
    ]
    for name, theme, scale in rooms:
        cx, cy = rect_center(name)
        # Neutral warm base - carries the frame so the stone is legible.
        spawn_point_light("Fill_{}".format(name), (cx, cy, 500.0), (255, 238, 212),
                          intensity=2300.0 * scale, radius=1800.0 * scale)
        # Soft themed accent - subtle colour identity.
        spawn_point_light("Accent_{}".format(name), (cx, cy, 300.0), _soft(theme),
                          intensity=1100.0 * scale, radius=1200.0 * scale)

    # Wall torches in the two arenas.
    for name in ("SalaCentrale", "ArenaCarceriere"):
        i0, i1, j0, j1 = ROOM_RECTS[name]
        x_left, x_right = i0 * TILE, (i1 + 1) * TILE
        cy_top, cy_bottom = (j0 + 0.5) * TILE, (j1 + 0.5) * TILE
        spawn_torch("{}_TorchNW".format(name), x_left + 30, cy_top, yaw=0.0)
        spawn_torch("{}_TorchSW".format(name), x_left + 30, cy_bottom, yaw=0.0)
        spawn_torch("{}_TorchNE".format(name), x_right - 30, cy_top, yaw=180.0)
        spawn_torch("{}_TorchSE".format(name), x_right - 30, cy_bottom, yaw=180.0)


# ------------------------------------------------------------------------- level build

def open_or_create_level():
    ensure_directory("/Game/Deepborn")
    ensure_directory("/Game/Deepborn/Maps")
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        log("Opening existing level {}".format(MAP_PATH))
        if not LEVEL_SUBSYSTEM.load_level(MAP_PATH):
            raise RuntimeError("Could not open level: {}".format(MAP_PATH))
        removed = 0
        for actor in ACTOR_SUBSYSTEM.get_all_level_actors():
            try:
                if actor.get_actor_label().startswith(CLEANUP_PREFIXES):
                    ACTOR_SUBSYSTEM.destroy_actor(actor)
                    removed += 1
            except Exception:
                pass
        log("Removed {} previous pack actors".format(removed))
        return
    log("Creating new level {}".format(MAP_PATH))
    if not LEVEL_SUBSYSTEM.new_level(MAP_PATH):
        raise RuntimeError("Could not create new level: {}".format(MAP_PATH))


def build_level():
    open_or_create_level()

    occ = build_occupancy()
    build_floors_and_ceilings(occ)
    build_perimeter_walls(occ)
    build_pillars()
    build_floor_collision()

    build_surface_temptation()
    build_level4_descent()
    place_dressing()
    place_lighting()

    # Start in the threshold room, facing east down the spine toward the hub arena.
    start_cx, start_cy = rect_center("SalaCaduta")
    spawn_player_start((start_cx + 200.0, start_cy, 150.0), yaw=0.0)

    log("Built {} floor tiles".format(len(occ)))
    log("Saving level")
    if not LEVEL_SUBSYSTEM.save_current_level():
        raise RuntimeError("Could not save current level")
    unreal.EditorAssetLibrary.save_directory("/Game/Deepborn", only_if_is_dirty=False, recursive=True)
    log("Done")


build_level()
