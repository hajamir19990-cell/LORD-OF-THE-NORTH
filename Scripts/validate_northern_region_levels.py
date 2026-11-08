"""Validate the eight regional greybox maps inside Unreal Editor.

Run after create_northern_region_levels.py. This checks asset existence and
basic actor composition only; it does not replace visual QA, navigation build,
lighting build, C++ compilation or Android device profiling.
"""
import unreal

REGIONS = (
    "LOTN_TheDreadfort", "LOTN_WhiteHarbor", "LOTN_BearIsland",
    "LOTN_Karhold", "LOTN_LastHearth", "LOTN_DeepwoodMotte",
    "LOTN_GreywaterWatch", "LOTN_TheWall",
)
errors = []
passed = []

def require(ok, message):
    (passed if ok else errors).append(message)

for level in REGIONS:
    path = "/Game/Maps/" + level
    exists = unreal.EditorAssetLibrary.does_asset_exist(path)
    require(exists, "map asset exists: " + level)
    if not exists:
        continue
    loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(path)
    if not loaded_world:
        require(False, "could load map: " + level)
        continue
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = subsystem.get_all_level_actors() if subsystem else []
    labels = [actor.get_actor_label() for actor in actors]
    require(any(label.startswith("ENV_" + level.replace("LOTN_", "") + "_Terrain") for label in labels),
            level + " has its regional terrain")
    require(any(label == "PlayerStart_" + level.replace("LOTN_", "") for label in labels),
            level + " has a PlayerStart")
    require(any(label == "NAV_" + level.replace("LOTN_", "") for label in labels),
            level + " has navigation bounds")
    require(any(label == "ENV_" + level.replace("LOTN_", "") + "_Forest_HISM" for label in labels),
            level + " has a batched forest actor")
    require(any(label.startswith("LANDMARK_") for label in labels),
            level + " has at least one regional landmark")
    require(any(label.startswith("TEMP_") and label.endswith("_Sun") for label in labels),
            level + " has a static-light baseline")
    require(any(label.startswith("TEMP_") and label.endswith("_Fog") for label in labels),
            level + " has restrained non-volumetric fog")

    forest_label = "ENV_" + level.replace("LOTN_", "") + "_Forest_HISM"
    forest_actor = next((a for a in actors if a.get_actor_label() == forest_label), None)
    if forest_actor:
        component = forest_actor.get_component_by_class(unreal.HierarchicalInstancedStaticMeshComponent)
        require(component is not None, level + " forest owns a HISM component")
        if component:
            require(component.get_instance_count() > 0, level + " forest contains tree instances")
            require(component.get_editor_property("collision_enabled") == unreal.CollisionEnabled.NO_COLLISION,
                    level + " forest collision is disabled")

for item in passed:
    unreal.log("LOTN REGIONAL PASS: " + item)
for item in errors:
    unreal.log_error("LOTN REGIONAL FAIL: " + item)
unreal.log("LOTN regional validation: {} passed, {} failed".format(len(passed), len(errors)))
if errors:
    raise RuntimeError("Regional map validation failed")
