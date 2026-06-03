import unreal


MAP_PATH = "/Game/Deepborn/Maps/L_Dungeon_Level5_Editor"
PREFIX = "DB_L5Pack_"

LEVEL_SUBSYSTEM = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
ACTOR_SUBSYSTEM = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


INTENTIONAL_OFF_GROUND = (
    "Ceiling",
    "Chandelier",
    "Torch",
    "RootBeam",
    "Light",
    "StairStep",
    "StairRun",
    "WallChains",
)

REJECTED_LAYOUT_LABELS = (
    PREFIX + "EntranceSurfaceDoorway",
    PREFIX + "EntranceVisibleSurfaceStairs",
    PREFIX + "EntranceSurfaceStairLowerRun",
    PREFIX + "EntranceSurfaceStairs10Flat",
    PREFIX + "EntranceSurfaceStairs5Flat",
    PREFIX + "SurfaceReadableStairRun_00",
    PREFIX + "SurfaceReadableStairRun_01",
    PREFIX + "SurfaceReadableStairRun_02",
    PREFIX + "SurfaceReadableStairRun_03",
    PREFIX + "SurfaceReadableStairRun_04",
    PREFIX + "PhaseGate_FirstCombatToJailerApproach",
    PREFIX + "Exit_DarkStairsToLevel4",
    PREFIX + "Exit_Level4Doorway",
)


def actor_bottom_z(actor):
    origin, extent = actor.get_actor_bounds(False)
    return origin.z - extent.z


def main():
    if not LEVEL_SUBSYSTEM.load_level(MAP_PATH):
        raise RuntimeError("Could not load {}".format(MAP_PATH))

    pack_actors = []
    collision_actors = []
    floating = []
    unsupported_warm_lights = []
    rejected_layout_actors = []
    vertical_stairs = []
    off_axis_entrance_steps = []

    for actor in ACTOR_SUBSYSTEM.get_all_level_actors():
        try:
            label = actor.get_actor_label()
        except Exception:
            continue

        if not label.startswith(PREFIX):
            continue

        pack_actors.append(actor)
        if label in REJECTED_LAYOUT_LABELS:
            rejected_layout_actors.append(label)

        if label.startswith(PREFIX + "COL_"):
            collision_actors.append(actor)
            continue

        if isinstance(actor, unreal.PointLight):
            label_lower = label.lower()
            if "light" in label_lower and not any(token in label_lower for token in ("torch", "candle", "chandelier", "blue", "fill", "window", "depth", "shadow")):
                unsupported_warm_lights.append(label)
            continue

        origin, extent = actor.get_actor_bounds(False)
        if "Stairs" in label and extent.z > max(extent.x, extent.y) * 0.65:
            vertical_stairs.append((label, extent.x, extent.y, extent.z))
            continue

        if "EntranceSurfaceStairStep" in label and abs(actor.get_actor_location().x) > 1.0:
            off_axis_entrance_steps.append((label, actor.get_actor_location().x))

        if any(token in label for token in INTENTIONAL_OFF_GROUND):
            continue

        bottom = origin.z - extent.z
        if bottom > 8.0:
            floating.append((label, bottom))

    unreal.log("DB_LEVEL5_EDITOR_VALIDATE: pack actors={}".format(len(pack_actors)))
    unreal.log("DB_LEVEL5_EDITOR_VALIDATE: collision actors={}".format(len(collision_actors)))

    if len(collision_actors) < 10:
        raise RuntimeError("Expected at least 10 hidden collision actors, found {}".format(len(collision_actors)))

    if unsupported_warm_lights:
        for label in unsupported_warm_lights[:25]:
            unreal.log_error("DB_LEVEL5_EDITOR_VALIDATE_UNSUPPORTED_LIGHT: {}".format(label))
        raise RuntimeError("Found {} light actors without a physical source or approved blue-fill role".format(len(unsupported_warm_lights)))

    if rejected_layout_actors:
        for label in rejected_layout_actors[:25]:
            unreal.log_error("DB_LEVEL5_EDITOR_VALIDATE_REJECTED_LAYOUT_ACTOR: {}".format(label))
        raise RuntimeError("Found {} rejected entrance/doorway/stair layout actors".format(len(rejected_layout_actors)))

    if vertical_stairs:
        for label, ex, ey, ez in vertical_stairs[:25]:
            unreal.log_error("DB_LEVEL5_EDITOR_VALIDATE_VERTICAL_STAIRS: {} extent=({:.1f},{:.1f},{:.1f})".format(label, ex, ey, ez))
        raise RuntimeError("Found {} stair actors with vertical orientation".format(len(vertical_stairs)))

    if off_axis_entrance_steps:
        for label, x in off_axis_entrance_steps[:25]:
            unreal.log_error("DB_LEVEL5_EDITOR_VALIDATE_OFF_AXIS_ENTRANCE_STEP: {} x={:.1f}".format(label, x))
        raise RuntimeError("Found {} entrance stair steps off the doorway centerline".format(len(off_axis_entrance_steps)))

    if floating:
        for label, bottom in floating[:25]:
            unreal.log_error("DB_LEVEL5_EDITOR_VALIDATE_FLOATING: {} bottom_z={:.1f}".format(label, bottom))
        raise RuntimeError("Found {} non-intentional floating DB_L5Pack actors".format(len(floating)))

    unreal.log("DB_LEVEL5_EDITOR_VALIDATE_PASSED: no non-intentional floating pack actors and hidden collision is present.")


main()
