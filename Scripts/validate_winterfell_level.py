"""Run inside Unreal Editor after opening /Game/Maps/WinterfellApproach.

Checks that the editor assembly created the expected first-slice actors and
that the 24-tree HISM cluster contains every instance. This is not a lighting,
navigation, C++ compile, Android package, or device-performance test.
"""
import unreal

LEVEL = "/Game/Maps/WinterfellApproach"
SOURCE_DIR = "/Game/Environment/North/SourceMeshes"
MESH_NAMES = (
    "Winterfell_Gatehouse", "Northern_Pine_Tree", "Northern_Longhouse",
    "Northern_Cobblestone_Road", "Northern_Curtain_Wall",
    "Northern_Watchtower", "Northern_Rolling_Terrain",
    "Northern_Supply_Crate", "Northern_Palisade_Fence",
    "Northern_Rock_Cluster", "Northern_Stone_Firepit",
    "Northern_Woodpile", "Northern_Handcart", "Northern_Snowdrift",
    "Northern_Wolf_Banner",
)

checks = []
errors = []

def require(condition, message):
    (checks if condition else errors).append(message)

require(
    unreal.EditorAssetLibrary.does_asset_exist(LEVEL),
    "WinterfellApproach map asset exists",
)
for name in MESH_NAMES:
    require(
        unreal.EditorAssetLibrary.does_asset_exist(SOURCE_DIR + "/" + name + "." + name),
        "Imported mesh exists: " + name,
    )

editor_actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
if not editor_actors:
    raise RuntimeError("EditorActorSubsystem is unavailable")
actors = editor_actors.get_all_level_actors()
labels = [actor.get_actor_label() for actor in actors]

def count_exact(label):
    return sum(1 for value in labels if value == label)

def count_prefix(prefix):
    return sum(1 for value in labels if value.startswith(prefix))

expected = {
    "Winterfell_Gatehouse": 1,
    "Northern_Curtain_Wall": 2,
    "Northern_Watchtower": 2,
    "Northern_Longhouse": 3,
    "Northern_Cobblestone_Road": 3,
    "Northern_Palisade_Fence": 4,
    "Northern_Rock_Cluster": 8,
    "Northern_Stone_Firepit": 1,
    "PROP_Western_Stacked_Firewood": 1,
    "PROP_Eastern_Stacked_Firewood": 1,
    "PROP_Eastern_Supply_Handcart": 1,
    "PROP_West_Gatehouse_Wolf_Standard": 1,
    "PROP_East_Gatehouse_Wolf_Standard": 1,
    "Northern_Rolling_Terrain": 1,
    "PlayerStart_Landscape_Approach": 1,
    "NPC_Northern_Guard_QuestGiver": 1,
    "NPC_Winterfell_Scout": 1,
    "OBJ_Winterfell_Supply_Marker": 1,
    "NAV_WinterfellApproach": 1,
    "TEMP_Northern_Directional_Light": 1,
    "TEMP_Northern_Atmospheric_Fog": 1,
    "ENV_Northern_Forest_HISM": 1,
}
for label, expected_count in expected.items():
    require(
        count_exact(label) == expected_count,
        "{} actor count is {} (expected {})".format(label, count_exact(label), expected_count),
    )

require(
    count_prefix("ENV_Roadside_Snowdrift_") == 6,
    "six roadside snow berm actors are present",
)

forest_actor = next(
    (actor for actor in actors if actor.get_actor_label() == "ENV_Northern_Forest_HISM"),
    None,
)
if forest_actor:
    hism_class = unreal.HierarchicalInstancedStaticMeshComponent
    forest_component = forest_actor.get_component_by_class(hism_class)
    require(forest_component is not None, "forest actor owns a HISM component")
    if forest_component:
        require(
            forest_component.get_instance_count() == 24,
            "forest HISM contains exactly 24 pine instances",
        )
        require(
            forest_component.get_editor_property("collision_enabled") == unreal.CollisionEnabled.NO_COLLISION,
            "forest HISM collision is disabled",
        )

for message in checks:
    unreal.log("LOTN LEVEL PASS: " + message)
for message in errors:
    unreal.log_error("LOTN LEVEL FAIL: " + message)

unreal.log(
    "LOTN level validation: {} passed, {} failed. "
    "Visual inspection, lighting build, navigation validation and Android profiling remain separate gates.".format(
        len(checks), len(errors)
    )
)
if errors:
    raise RuntimeError("WinterfellApproach level validation failed")
