import unreal


MAP_PATH = "/Game/Deepborn/Maps/L_Dungeon_Level5_Editor"
PREFIX = "DB_L5Pack"
CLEANUP_PREFIXES = ("DB_L5Pack_", "DB_MCP_L5_", "DB_MCP_")

LEVEL_SUBSYSTEM = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
ACTOR_SUBSYSTEM = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


MESHES = {
    "cube": "/Engine/BasicShapes/Cube.Cube",
    "floor": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Floor.SM_Floor",
    "crypt_floor": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Floor.SM_Crypt_Floor",
    "crypt_floor_concave": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Floor_Concave.SM_Crypt_Floor_Concave",
    "wall": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall.SM_Dungeon_Wall",
    "wall_half": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall_Half.SM_Dungeon_Wall_Half",
    "wall_short": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/Wall_Short.Wall_Short",
    "wall_decor": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Dungeon_Wall_Decorative_A.SM_Dungeon_Wall_Decorative_A",
    "crypt_wall": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Wall.SM_Crypt_Wall",
    "crypt_wall_decor_a": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Wall_Decorative_A.SM_Crypt_Wall_Decorative_A",
    "crypt_wall_decor_b": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Wall_Decorative_B.SM_Crypt_Wall_Decorative_B",
    "ceiling": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Flat.SM_Ceiling_Flat",
    "ceiling_arch": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Arch_A.SM_Ceiling_Arch_A",
    "ceiling_arch_b": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Arch_B.SM_Ceiling_Arch_B",
    "ceiling_piece": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Ceiling_Piece.SM_Ceiling_Piece",
    "decorative_arches": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Decorative_Arches.SM_Decorative_Arches",
    "pillar": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Pillar.SM_Pillar",
    "crypt_pillar": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Pillar.SM_Crypt_Pillar",
    "doorway": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay.SM_DoorWay",
    "crypt_doorway": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Crypt_Doorway.SM_Crypt_Doorway",
    "large_door_left": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay_Large_Door_Left.SM_DoorWay_Large_Door_Left",
    "large_door_right": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_DoorWay_Large_Door_Right.SM_DoorWay_Large_Door_Right",
    "half_arch": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Half_Arch.SM_Half_Arch",
    "cell_front": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Front.SM_Cell_Front",
    "cell_side": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Side.SM_Cell_Side",
    "cell_door": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Cell_Door.SM_Cell_Door",
    "stairs_10": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_10.SM_Stairs_10",
    "stairs_5": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Stairs_5.SM_Stairs_5",
    "edge": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Edge_Stones.SM_Edge_Stones",
    "fire_pit": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Fire_Pit.SM_Fire_Pit",
    "large_window": "/Game/MedievalDungeon/Meshes/Architecture/Dungeon/SM_Large_Window_Half.SM_Large_Window_Half",
    "torch": "/Game/MedievalDungeon/Meshes/Props/SM_Torch.SM_Torch",
    "candle": "/Game/MedievalDungeon/Meshes/Props/SM_Candle_Used_A.SM_Candle_Used_A",
    "candle_b": "/Game/MedievalDungeon/Meshes/Props/SM_Candle_Used_B.SM_Candle_Used_B",
    "candle_holder": "/Game/MedievalDungeon/Meshes/Props/SM_CandleHolder_Iron.SM_CandleHolder_Iron",
    "chandelier": "/Game/MedievalDungeon/Meshes/Props/SM_Chandelier.SM_Chandelier",
    "table": "/Game/MedievalDungeon/Meshes/Props/SM_Table.SM_Table",
    "coffin": "/Game/MedievalDungeon/Meshes/Props/SM_Coffin.SM_Coffin",
    "coffin_open": "/Game/MedievalDungeon/Meshes/Props/SM_Coffin_No_Lid.SM_Coffin_No_Lid",
    "stone_coffin": "/Game/MedievalDungeon/Meshes/Props/SM_Stone_Coffin.SM_Stone_Coffin",
    "statue": "/Game/MedievalDungeon/Meshes/Props/SM_Statue_Hooded.SM_Statue_Hooded",
    "statue_stand": "/Game/MedievalDungeon/Meshes/Props/SM_Statue_Stand.SM_Statue_Stand",
    "gargoyle": "/Game/MedievalDungeon/Meshes/Props/SM_Gargoyle_Statue_On_Stand.SM_Gargoyle_Statue_On_Stand",
    "pot_a": "/Game/MedievalDungeon/Meshes/Props/SM_Pot_A_Complete.SM_Pot_A_Complete",
    "pot_b": "/Game/MedievalDungeon/Meshes/Props/SM_Pot_B_Complete.SM_Pot_B_Complete",
    "pot_c": "/Game/MedievalDungeon/Meshes/Props/SM_Pot_C_Complete.SM_Pot_C_Complete",
    "mausoleum": "/Game/MedievalDungeon/Meshes/Architecture/Crypt/SM_Mausoleum.SM_Mausoleum",
    "stocks": "/Game/MedievalDungeon/Meshes/Props/SM_Stocks_Game_Ready.SM_Stocks_Game_Ready",
    "torture_chair": "/Game/MedievalDungeon/Meshes/Props/SM_Torture_Chair.SM_Torture_Chair",
    "gibbet": "/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Wall_Mount.SM_Gibbet_Cage_Wall_Mount",
    "gibbet_open": "/Game/MedievalDungeon/Meshes/Props/SM_Gibbet_Cage_Open.SM_Gibbet_Cage_Open",
    "chains": "/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_A.SM_Decor_Chains_A",
    "chains_b": "/Game/MedievalDungeon/Meshes/Props/SM_Decor_Chains_B.SM_Decor_Chains_B",
    "ball_chain": "/Game/MedievalDungeon/Meshes/Props/SM_Ball_And_Chain.SM_Ball_And_Chain",
    "handcuff": "/Game/MedievalDungeon/Meshes/Props/SM_Handcuff.SM_Handcuff",
    "lock_key": "/Game/MedievalDungeon/Meshes/Props/SM_Lock_Key.SM_Lock_Key",
    "lock_open": "/Game/MedievalDungeon/Meshes/Props/SM_Lock_Open.SM_Lock_Open",
    "wood_a": "/Game/MedievalDungeon/Meshes/Props/SM_Wood_A.SM_Wood_A",
    "wood_b": "/Game/MedievalDungeon/Meshes/Props/SM_Wood_B.SM_Wood_B",
    "dirt": "/Game/MedievalDungeon/Meshes/Props/SM_Dirt_Mound_A.SM_Dirt_Mound_A",
    "skull": "/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Skull.SM_Bones_Skull",
    "rib": "/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_RibCage.SM_Bones_RibCage",
    "arm": "/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Arm.SM_Bones_Arm",
    "leg": "/Game/MedievalDungeon/Meshes/Skeletons/SM_Bones_Leg.SM_Bones_Leg",
    "skeleton": "/Game/MedievalDungeon/Meshes/Skeletons/SM_Skeleton_01.SM_Skeleton_01",
}


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
    return "{}_{}".format(PREFIX, base)


def set_folder(actor, folder):
    try:
        actor.set_folder_path(folder)
    except Exception:
        pass


def configure_collision(actor, enabled):
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if not component:
        return
    if enabled:
        component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    else:
        component.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)


def ground_actor(actor, bottom_z=0.0):
    origin, extent = actor.get_actor_bounds(False)
    current_bottom = origin.z - extent.z
    delta = bottom_z - current_bottom
    loc = actor.get_actor_location()
    actor.set_actor_location(unreal.Vector(loc.x, loc.y, loc.z + delta), False, False)


def spawn_mesh(key, base, location, rotation=(0, 0, 0), scale=(1, 1, 1), folder="DungeonLevel5", bottom_z=0.0, collision=False):
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_object(
        load_mesh(key),
        unreal.Vector(location[0], location[1], location[2]),
        unreal.Rotator(rotation[0], rotation[1], rotation[2]),
    )
    actor.set_actor_label(label(base), mark_dirty=True)
    actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
    set_folder(actor, folder)
    configure_collision(actor, collision)
    if bottom_z is not None:
        ground_actor(actor, bottom_z)
    return actor


def spawn_collision(base, location, size, folder="DungeonLevel5/GameplayCollision"):
    actor = spawn_mesh(
        "cube",
        "COL_{}".format(base),
        location,
        scale=(size[0] / 100.0, size[1] / 100.0, size[2] / 100.0),
        folder=folder,
        bottom_z=None,
        collision=True,
    )
    actor.set_actor_hidden_in_game(True)
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    if component:
        component.set_visibility(False)
        component.set_editor_property("hidden_in_game", True)
    return actor


def spawn_light(base, location, color, intensity=2200.0, radius=900.0, folder="DungeonLevel5/Lighting"):
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_class(
        unreal.PointLight,
        unreal.Vector(location[0], location[1], location[2]),
        unreal.Rotator(0, 0, 0),
    )
    actor.set_actor_label(label(base), mark_dirty=True)
    actor.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
    actor.light_component.set_editor_property("intensity", intensity)
    actor.light_component.set_editor_property("attenuation_radius", radius)
    actor.light_component.set_editor_property("light_color", unreal.Color(color[0], color[1], color[2], 255))
    try:
        actor.light_component.set_light_color(unreal.LinearColor(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0, 1.0), True)
    except Exception:
        pass
    set_folder(actor, folder)
    return actor


def spawn_torch(base, location, yaw, intensity=900.0, radius=520.0):
    spawn_mesh("torch", base, location, rotation=(0, yaw, 0), scale=(0.64, 0.64, 0.64), folder="DungeonLevel5/Lighting/Torches", bottom_z=None)
    spawn_light("{}Light".format(base), (location[0], location[1], location[2] + 28), (255, 137, 45), intensity=intensity, radius=radius, folder="DungeonLevel5/Lighting/TorchLights")


def spawn_candle_cluster(base, location, scale=0.68, intensity=420.0, radius=310.0):
    spawn_mesh("candle", "{}A".format(base), location, scale=(scale, scale, scale), folder="DungeonLevel5/Lighting/Candles", bottom_z=0)
    spawn_mesh("candle_b", "{}B".format(base), (location[0] + 22, location[1] - 8, location[2]), scale=(scale * 0.82, scale * 0.82, scale * 0.82), folder="DungeonLevel5/Lighting/Candles", bottom_z=0)
    spawn_light("{}Light".format(base), (location[0] + 10, location[1], location[2] + 58), (255, 176, 82), intensity=intensity, radius=radius, folder="DungeonLevel5/Lighting/CandleLights")


def spawn_chandelier(base, location):
    spawn_mesh("chandelier", base, location, scale=(0.82, 0.82, 0.82), folder="DungeonLevel5/Lighting/Chandeliers", bottom_z=None)
    spawn_light("{}Light".format(base), (location[0], location[1], location[2] - 20), (255, 158, 66), intensity=620.0, radius=560.0, folder="DungeonLevel5/Lighting/ChandelierLights")


def spawn_player_start(location):
    actor = ACTOR_SUBSYSTEM.spawn_actor_from_class(
        unreal.PlayerStart,
        unreal.Vector(location[0], location[1], location[2]),
        unreal.Rotator(0, 0, 0),
    )
    actor.set_actor_label(label("PlayerStart_Level5"), mark_dirty=True)
    set_folder(actor, "DungeonLevel5/Gameplay")
    return actor


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
        log("Removed {} previous {} actors".format(removed, PREFIX))
        return

    log("Creating new level {}".format(MAP_PATH))
    if not LEVEL_SUBSYSTEM.new_level(MAP_PATH):
        raise RuntimeError("Could not create new level: {}".format(MAP_PATH))


def floor_grid(name, xs, ys, mesh_key="floor", scale=0.72):
    for xi, x in enumerate(xs):
        for yi, y in enumerate(ys):
            key = mesh_key if (xi + yi) % 4 else "crypt_floor"
            spawn_mesh(key, "{}_Floor_{}_{}".format(name, xi, yi), (x, y, 0), scale=(scale, scale, scale), folder="DungeonLevel5/Architecture/Floors", bottom_z=0)


def build_entrance_corridor():
    # Spatial intent: a clean vaulted dungeon hall. Architecture first, props second.
    floor_grid(
        "EntranceCorridor",
        [-1040, -780, -520, -260, 0, 260, 520, 780, 1040],
        list(range(-1840, 1440, 260)),
        scale=0.82,
    )

    side_walls("EntranceCorridor", list(range(-1840, 1440, 260)), 1320, scale=0.80)

    # Rear wall frames the surface stair with one controlled opening; every other edge is closed.
    for i, x in enumerate([-1040, -780, -520, 520, 780, 1040]):
        spawn_mesh("crypt_wall", "EntranceSurfaceRearWall_{}".format(i), (x, -2020, 0), rotation=(0, 180, 0), scale=(0.80, 0.80, 0.80), folder="DungeonLevel5/Entrance/SurfaceFrame", bottom_z=0)

    spawn_mesh("doorway", "EntranceSurfaceDoorwayUpright", (0, -2020, 0), rotation=(0, 0, 90), scale=(0.92, 0.92, 0.92), folder="DungeonLevel5/Entrance/SurfaceFrame", bottom_z=0)
    for i in range(10):
        spawn_mesh(
            "crypt_floor",
            "EntranceSurfaceStairStep_{:02d}".format(i),
            (0, -2040 - i * 86, 0),
            scale=(1.28, 0.32, 0.14),
            folder="DungeonLevel5/Entrance/SurfaceFrame",
            bottom_z=i * 18,
        )

    # The entrance remains open and wide enough for combat. Progression starts after the first arena.
    cross_wall("PhaseGateJailerSideWall", 1360, [-1040, -780, -520, -260, 0, 260, 520, 780, 1040], opening_min=-420, opening_max=420)
    spawn_mesh("doorway", "PhaseGate_FirstCombatDoorwayUpright", (0, 1360, 0), rotation=(0, 0, 90), scale=(0.88, 0.88, 0.88), folder="DungeonLevel5/PhaseGates", bottom_z=0)

    for x in [-760, 760]:
        for y in [-1420, -620, 180, 980]:
            spawn_mesh("crypt_pillar", "EntranceSidePillar_{}_{}".format(x, y), (x, y, 0), scale=(0.74, 0.74, 1.02), folder="DungeonLevel5/Architecture/Pillars", bottom_z=0)

    for x in [-1180, 1180]:
        for y in [-1560, -780, 40, 860, 1380]:
            spawn_mesh("crypt_pillar", "EntranceWallButtress_{}_{}".format(x, y), (x, y, 0), scale=(0.56, 0.56, 0.82), folder="DungeonLevel5/Architecture/Pillars", bottom_z=0)

    # First pass keeps the entrance ceiling clean; unsupported arch modules stay out of the playable sightline.

    # Source lights for the hall. Warm light is always attached to torches/candles/chandelier.
    spawn_torch("EntranceTorchLeftRear", (-1290, -1500, 172), 90, intensity=900, radius=540)
    spawn_torch("EntranceTorchRightRear", (1290, -1500, 172), -90, intensity=900, radius=540)
    spawn_torch("EntranceTorchLeftMid", (-1290, -420, 172), 90, intensity=860, radius=520)
    spawn_torch("EntranceTorchRightMid", (1290, -420, 172), -90, intensity=860, radius=520)
    spawn_torch("EntranceTorchLeftGate", (-1290, 820, 172), 90, intensity=820, radius=500)
    spawn_torch("EntranceTorchRightGate", (1290, 820, 172), -90, intensity=820, radius=500)


def side_walls(name, ys, x_abs, scale=0.78, wall_key="crypt_wall"):
    for i, y in enumerate(ys):
        key = wall_key if i % 2 else "wall"
        spawn_mesh(key, "{}_LeftWall_{}".format(name, i), (-x_abs, y, 0), rotation=(0, 90, 0), scale=(scale, scale, scale), folder="DungeonLevel5/Architecture/Walls", bottom_z=0)
        spawn_mesh(key, "{}_RightWall_{}".format(name, i), (x_abs, y, 0), rotation=(0, -90, 0), scale=(scale, scale, scale), folder="DungeonLevel5/Architecture/Walls", bottom_z=0)
        if i % 3 == 1:
            spawn_mesh("crypt_wall_decor_a", "{}_LeftWallDecor_{}".format(name, i), (-x_abs + 18, y, 0), rotation=(0, 90, 0), scale=(scale, scale, scale), folder="DungeonLevel5/Architecture/Walls", bottom_z=0)
            spawn_mesh("crypt_wall_decor_b", "{}_RightWallDecor_{}".format(name, i), (x_abs - 18, y, 0), rotation=(0, -90, 0), scale=(scale, scale, scale), folder="DungeonLevel5/Architecture/Walls", bottom_z=0)


def cross_wall(name, y, xs, opening_min=-260, opening_max=260, scale=0.78):
    for i, x in enumerate(xs):
        if opening_min <= x <= opening_max:
            continue
        spawn_mesh("crypt_wall", "{}_CrossWall_{}".format(name, i), (x, y, 0), rotation=(0, 180, 0), scale=(scale, scale, scale), folder="DungeonLevel5/Architecture/Walls", bottom_z=0)


def ceiling_grid(name, xs, ys, z=430, scale=0.72, every=2):
    for xi, x in enumerate(xs):
        for yi, y in enumerate(ys):
            if (xi + yi) % every == 0:
                spawn_mesh("ceiling", "{}_Ceiling_{}_{}".format(name, xi, yi), (x, y, z), scale=(scale, scale, scale), folder="DungeonLevel5/Architecture/Ceiling", bottom_z=None, collision=False)


def low_route_edges():
    for y in range(-1500, 3320, 330):
        spawn_mesh("edge", "RouteEdgeLeft_{}".format(y), (-690, y, 0), rotation=(0, 90, 0), scale=(0.48, 0.48, 0.48), folder="DungeonLevel5/Architecture/LowEdges", bottom_z=0)
        spawn_mesh("edge", "RouteEdgeRight_{}".format(y), (690, y, 0), rotation=(0, -90, 0), scale=(0.48, 0.48, 0.48), folder="DungeonLevel5/Architecture/LowEdges", bottom_z=0)


def cells():
    # Jail starts after the first clean hall, not inside the entrance composition.
    for side, x, yaw, inside_x in [("Left", -1060, 90, -840), ("Right", 1060, -90, 840)]:
        for i, y in enumerate([1540, 2140]):
            spawn_mesh("cell_front", "{}CellFront_{}".format(side, i), (x, y, 0), rotation=(0, yaw, 0), scale=(0.78, 0.78, 0.78), folder="DungeonLevel5/Architecture/Cells", bottom_z=0)
            spawn_mesh("cell_side", "{}CellSide_{}".format(side, i), (inside_x, y - 145, 0), rotation=(0, 0 if side == "Left" else 180, 0), scale=(0.62, 0.62, 0.62), folder="DungeonLevel5/Architecture/Cells", bottom_z=0)
            spawn_mesh("crypt_wall", "{}CellBackWall_{}".format(side, i), (inside_x, y + 170, 0), rotation=(0, 180, 0), scale=(0.62, 0.62, 0.62), folder="DungeonLevel5/Architecture/Cells", bottom_z=0)
            spawn_mesh("crypt_wall", "{}CellOuterSeal_{}".format(side, i), (x + (-130 if side == "Left" else 130), y, 0), rotation=(0, yaw, 0), scale=(0.62, 0.62, 0.62), folder="DungeonLevel5/Architecture/Cells", bottom_z=0)
            if i != 1:
                spawn_mesh("cell_door", "{}CellDoorBroken_{}".format(side, i), (inside_x, y + 75, 0), rotation=(0, yaw + (16 if side == "Left" else -16), 0), scale=(0.58, 0.58, 0.58), folder="DungeonLevel5/Architecture/Cells", bottom_z=0)
            if i == 0:
                spawn_mesh("skeleton", "{}CellSkeleton_{}".format(side, i), (inside_x, y - 55, 0), rotation=(0, yaw, 0), scale=(0.38, 0.38, 0.38), folder="DungeonLevel5/SetDressing/Bones", bottom_z=0)


def landmarks():
    spawn_mesh("lock_open", "PhaseGateBrokenLock", (-270, 1300, 0), rotation=(0, -35, 0), scale=(0.54, 0.54, 0.54), folder="DungeonLevel5/PhaseGates", bottom_z=0)

    spawn_mesh("cell_front", "BossRoom_RearCellBars", (0, 2520, 0), rotation=(0, 180, 0), scale=(1.28, 1.28, 1.15), folder="DungeonLevel5/BossRoom", bottom_z=0)
    spawn_mesh("gibbet", "BossRoom_WallGibbet", (470, 2360, 0), rotation=(0, 180, 0), scale=(0.78, 0.78, 0.78), folder="DungeonLevel5/BossRoom", bottom_z=0)
    spawn_mesh("stocks", "BossRoom_Stocks", (-430, 1910, 0), rotation=(0, 24, 0), scale=(0.72, 0.72, 0.72), folder="DungeonLevel5/BossRoom", bottom_z=0)
    spawn_mesh("torture_chair", "BossRoom_TortureChair", (440, 1780, 0), rotation=(0, -24, 0), scale=(0.65, 0.65, 0.65), folder="DungeonLevel5/BossRoom", bottom_z=0)
    spawn_mesh("fire_pit", "BossRoom_CentralFirePit", (0, 1980, 0), scale=(0.85, 0.85, 0.85), folder="DungeonLevel5/BossRoom", bottom_z=0)
    spawn_mesh("ball_chain", "BossRoom_JailerBallChain", (-80, 2110, 0), rotation=(0, 35, 0), scale=(0.7, 0.7, 0.7), folder="DungeonLevel5/BossRoom", bottom_z=0)
    spawn_mesh("lock_key", "BossRoom_FusedKeyClue", (110, 2100, 0), rotation=(0, -20, 0), scale=(0.7, 0.7, 0.7), folder="DungeonLevel5/BossRoom", bottom_z=0)

    for i in range(8):
        spawn_mesh(
            "crypt_floor",
            "Exit_DarkStairStep_{:02d}".format(i),
            (0, 3120 + i * 82, 0),
            scale=(1.18 - i * 0.035, 0.30, 0.14),
            folder="DungeonLevel5/ExitToLevel4",
            bottom_z=i * 16,
        )
    spawn_mesh("doorway", "Exit_Level4DoorwayUpright", (0, 3660, 0), rotation=(0, 0, 90), scale=(0.88, 0.88, 0.88), folder="DungeonLevel5/ExitToLevel4", bottom_z=0)


def pillars_and_mausoleums():
    for x in [-980, 980]:
        for y in [1880, 2460, 3180]:
            spawn_mesh("pillar", "LowerWallButtress_{}_{}".format(x, y), (x, y, 0), scale=(0.58, 0.58, 0.86), folder="DungeonLevel5/Architecture/Pillars", bottom_z=0)
    for i, (x, y, yaw) in enumerate([(-1120, -1140, 0), (1120, -1140, 180), (-1120, 240, 0), (1120, 240, 180)]):
        spawn_mesh("mausoleum", "WallAnchoredMausoleum_{}".format(i), (x, y, 0), rotation=(0, yaw, 0), scale=(0.44, 0.44, 0.44), folder="DungeonLevel5/SetDressing/Tombs", bottom_z=0)


def dressing():
    # Props are placed after the architecture, on edges or functional surfaces.
    props = [
        ("dirt", "SurfaceStairRubbleBase", (-330, -2050, 0), (0, 18, 0), 0.48),
        ("wood_a", "SurfaceStairBrokenBeamLeft", (-520, -1960, 210), (7, 6, 82), 0.48),
        ("wood_b", "SurfaceStairBrokenBeamRight", (520, -1960, 230), (-6, -8, 84), 0.46),
        ("table", "EntranceCenterTable", (0, -700, 0), (0, 8, 0), 0.72),
        ("pot_a", "EntranceLeftPotA", (-1120, -840, 0), (0, 18, 0), 0.58),
        ("pot_b", "EntranceLeftPotB", (-1160, -740, 0), (0, -22, 0), 0.44),
        ("pot_c", "EntranceRightPotA", (1120, -980, 0), (0, -10, 0), 0.56),
        ("coffin", "EntranceLeftWallCoffin", (-1110, 120, 0), (0, 0, 0), 0.48),
        ("stone_coffin", "EntranceRightWallCoffin", (1110, 260, 0), (0, 180, 0), 0.48),
        ("statue_stand", "EntranceRearStatueStand", (0, 1120, 0), (0, 180, 0), 0.62),
        ("statue", "EntranceRearHoodedStatue", (0, 1120, 90), (0, 180, 0), 0.48),
        ("chains", "JailWallChains", (1260, 1780, 150), (0, -90, 0), 0.55),
        ("coffin_open", "BossRoom_OpenCoffin", (-650, 2260, 0), (0, -18, 0), 0.62),
        ("coffin", "BossRoom_CoffinRight", (650, 2140, 0), (0, 14, 0), 0.58),
        ("skull", "BossRoom_SkullNearStocks", (-360, 1840, 0), (0, -44, 0), 0.48),
        ("handcuff", "BossRoom_HandcuffNearJailer", (-170, 2060, 0), (0, -15, 0), 0.58),
        ("leg", "Exit_BoneLegAtWall", (-620, 3080, 0), (0, 40, 0), 0.46),
    ]
    for key, name, loc, rot, scale in props:
        bottom = None if "RootBeam" in name or "WallChains" in name else 0
        spawn_mesh(key, name, loc, rotation=rot, scale=(scale, scale, scale), folder="DungeonLevel5/SetDressing", bottom_z=bottom)

    spawn_candle_cluster("EntranceTableCandles", (-70, -700, 0), scale=0.62, intensity=280, radius=260)
    spawn_candle_cluster("EntranceLeftMausoleumCandles", (-1100, 240, 0), scale=0.58, intensity=220, radius=230)
    spawn_candle_cluster("EntranceRightMausoleumCandles", (1100, 240, 0), scale=0.58, intensity=220, radius=230)

    for i, (x, y, yaw) in enumerate([
        (-1015, 1560, 90), (1015, 1560, -90),
        (-1015, 2240, 90), (1015, 2240, -90), (-905, 3060, 90), (905, 3060, -90),
    ]):
        spawn_torch("LowerRouteTorch_{}".format(i), (x, y, 150), yaw, intensity=820, radius=500)


def lighting():
    # Blue is ambient fill for unlit depth, never the main source in torch-lit areas.
    spawn_light("SurfaceStairColdWindowFill", (0, -2180, 390), (75, 135, 255), intensity=160, radius=560)
    spawn_light("EntranceDeepShadowBlueFill", (0, 780, 320), (38, 78, 160), intensity=80, radius=760)
    spawn_light("JailDeepBlueFill", (0, 2100, 300), (42, 82, 170), intensity=320, radius=980)
    spawn_light("ExitDepthBlueFill", (0, 3330, 260), (55, 120, 205), intensity=360, radius=780)


def gameplay_collision():
    # Invisible collision surfaces keep gameplay reliable even if individual pack meshes have weak collision.
    spawn_collision("EntranceCorridorFloor", (0, -690, -12), (2700, 3320, 42))
    spawn_collision("SurfaceStairFloor", (0, -2240, -12), (980, 720, 42))
    spawn_collision("MainRouteFloor", (0, 520, -12), (2180, 1440, 42))
    spawn_collision("BossRoomFloor", (0, 2040, -12), (2200, 1420, 42))
    spawn_collision("ExitRouteFloor", (0, 3140, -12), (1680, 1180, 42))

    for name, x, y, sx, sy in [
        ("EntranceLeftWall", -1400, -690, 80, 3320), ("EntranceRightWall", 1400, -690, 80, 3320),
        ("SurfaceLeftWall", -560, -2240, 80, 720), ("SurfaceRightWall", 560, -2240, 80, 720),
        ("SurfaceBackWall", 0, -2640, 1180, 80),
        ("MainLeftWall", -1160, 520, 80, 1440), ("MainRightWall", 1160, 520, 80, 1440),
        ("BossLeftWall", -1130, 2040, 80, 1420), ("BossRightWall", 1130, 2040, 80, 1420),
        ("ExitLeftWall", -980, 3140, 80, 1180), ("ExitRightWall", 980, 3140, 80, 1180),
        ("BossRearWall", 0, 2600, 2200, 80), ("ExitRearWall", 0, 3780, 1680, 80),
    ]:
        spawn_collision(name, (x, y, 160), (sx, sy, 320))


def build_level():
    open_or_create_level()

    main_xs = [-780, -520, -260, 0, 260, 520, 780]
    main_ys = list(range(900, 1440, 260))
    boss_xs = [-780, -520, -260, 0, 260, 520, 780]
    boss_ys = list(range(1400, 2640, 260))
    exit_ys = list(range(2660, 3700, 260))

    build_entrance_corridor()
    floor_grid("MainRoute", main_xs, main_ys, scale=0.74)
    floor_grid("BossRoom", boss_xs, boss_ys, scale=0.74)
    floor_grid("ExitRoute", main_xs, exit_ys, scale=0.74)

    side_walls("MainRoute", list(range(900, 1440, 260)), 1080, scale=0.78)
    side_walls("BossRoom", list(range(1400, 2640, 260)), 1080, scale=0.78)
    side_walls("ExitRoute", list(range(2660, 3700, 260)), 940, scale=0.78)
    cross_wall("BossRear", 2640, [-780, -520, -260, 0, 260, 520, 780], opening_min=-220, opening_max=220)

    low_route_edges()
    cells()
    pillars_and_mausoleums()
    landmarks()
    dressing()
    lighting()
    gameplay_collision()
    spawn_player_start((0, -1320, 120))

    log("Saving level")
    if not LEVEL_SUBSYSTEM.save_current_level():
        raise RuntimeError("Could not save current level")
    unreal.EditorAssetLibrary.save_directory("/Game/Deepborn", only_if_is_dirty=False, recursive=True)
    log("Done")


build_level()
