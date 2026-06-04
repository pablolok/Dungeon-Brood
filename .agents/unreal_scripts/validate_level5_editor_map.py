"""Validate the editor-authored Dungeon Level 5 map for the failure modes that made the
old build look chaotic: overlapping tiles, gaps, and floating geometry.

Checks:
  * floor tile count equals the unique tiles of the layout (no gaps, no extras);
  * no floor-floor and no wall-wall XY overlaps;
  * art meshes sit on the ground (bottom Z ~ 0), excluding intentionally raised pieces;
  * a PlayerStart and hidden gameplay collision are present.
"""

import unreal


MAP_PATH = "/Game/Deepborn/Maps/L_Dungeon_Level5_Editor"
PREFIX = "DB_L5Pack_"

LEVEL_SUBSYSTEM = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
ACTOR_SUBSYSTEM = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# Must mirror ALL_RECTS in build_level5_editor_map.py (i_min, i_max, j_min, j_max).
RECTS = {
    "SalaCaduta": (-4, -3, -1, 0), "SalaCentrale": (-1, 2, -2, 1),
    "ArenaCarceriere": (5, 7, -2, 1), "ScalaLivello4": (10, 11, -1, 0),
    "CelleCrollate": (-3, -2, -4, -3), "SalaAcqua": (-1, 0, -5, -4),
    "SalaRadici": (5, 6, -5, -4), "CorridoioCatene": (-3, -2, 3, 4),
    "StanzaEvento": (0, 1, 4, 5), "BacinoBiomassa": (5, 6, 4, 5),
    "C_Caduta_Centrale": (-2, -2, 0, 0), "C_Centrale_Carceriere": (3, 4, 0, 0),
    "C_Carceriere_Livello4": (8, 9, 0, 0), "C_Celle": (-2, -2, -2, -1),
    "C_Acqua": (0, 0, -3, -3), "C_Radici": (6, 6, -3, -3),
    "C_Catene": (-2, -2, 1, 2), "C_Evento": (1, 1, 2, 3), "C_Bacino": (6, 6, 2, 3),
}

INTENTIONAL_OFF_GROUND = ("Ceiling", "Torch", "Light", "StairStep", "Daylight",
                          "DepthGlow", "Glow", "COL_", "Statue")
OVERLAP_TOL = 40.0
GROUND_TOL = 12.0


def log(msg):
    unreal.log("DB_LEVEL5_EDITOR_VALIDATE: " + msg)


def err(msg):
    unreal.log_error("DB_LEVEL5_EDITOR_VALIDATE_" + msg)


def aabb(actor):
    o, e = actor.get_actor_bounds(False)
    return (o.x - e.x, o.x + e.x, o.y - e.y, o.y + e.y, o.z - e.z, o.z + e.z)


def xy_overlap(a, b):
    return min(min(a[1], b[1]) - max(a[0], b[0]), min(a[3], b[3]) - max(a[2], b[2]))


def expected_tiles():
    tiles = set()
    for (i0, i1, j0, j1) in RECTS.values():
        for i in range(i0, i1 + 1):
            for j in range(j0, j1 + 1):
                tiles.add((i, j))
    return len(tiles)


def main():
    if not LEVEL_SUBSYSTEM.load_level(MAP_PATH):
        raise RuntimeError("Could not load {}".format(MAP_PATH))

    floors, walls = [], []
    pack = collision = 0
    has_player_start = False
    floating = []

    for actor in ACTOR_SUBSYSTEM.get_all_level_actors():
        try:
            lbl = actor.get_actor_label()
        except Exception:
            continue
        if isinstance(actor, unreal.PlayerStart) and lbl.startswith(PREFIX):
            has_player_start = True
        if not lbl.startswith(PREFIX):
            continue
        pack += 1
        if "COL_" in lbl:
            collision += 1
            continue
        if not isinstance(actor, unreal.StaticMeshActor):
            continue
        box = aabb(actor)
        if "Floor_" in lbl and "COL_" not in lbl:
            floors.append((lbl, box))
        elif "Wall" in lbl:
            walls.append((lbl, box))
        if not any(tok in lbl for tok in INTENTIONAL_OFF_GROUND) and box[4] > GROUND_TOL:
            floating.append((lbl, box[4]))

    log("pack={} floors={} walls={} collision={}".format(pack, len(floors), len(walls), collision))
    problems = []

    if floating:
        for lbl, b in floating[:25]:
            err("FLOATING: {} bottom_z={:.1f}".format(lbl, b))
        problems.append("{} floating art".format(len(floating)))

    exp = expected_tiles()
    if len(floors) != exp:
        err("FLOOR_COUNT: expected {} unique tiles, found {}".format(exp, len(floors)))
        problems.append("floor count mismatch (gap/extra)")

    fo = 0
    for a in range(len(floors)):
        for b in range(a + 1, len(floors)):
            if xy_overlap(floors[a][1], floors[b][1]) > OVERLAP_TOL:
                if fo < 25:
                    err("FLOOR_OVERLAP: {} <-> {}".format(floors[a][0], floors[b][0]))
                fo += 1
    if fo:
        problems.append("{} floor overlaps".format(fo))

    wo = 0
    for a in range(len(walls)):
        for b in range(a + 1, len(walls)):
            if xy_overlap(walls[a][1], walls[b][1]) > OVERLAP_TOL:
                if wo < 25:
                    err("WALL_OVERLAP: {} <-> {}".format(walls[a][0], walls[b][0]))
                wo += 1
    if wo:
        problems.append("{} wall overlaps".format(wo))

    if not has_player_start:
        err("NO_PLAYER_START")
        problems.append("missing PlayerStart")
    if collision < 8:
        err("LOW_COLLISION: {}".format(collision))
        problems.append("insufficient collision")

    if problems:
        raise RuntimeError("Validation failed: " + "; ".join(problems))
    log("PASSED: {} floor tiles tile cleanly, no floor/wall overlaps, no floating art, "
        "PlayerStart + collision present.".format(len(floors)))


main()
