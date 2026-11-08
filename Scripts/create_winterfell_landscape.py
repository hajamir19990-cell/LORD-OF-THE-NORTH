"""Create a landscape greybox level from the committed OBJ source meshes.

Run inside Unreal Editor's Python console after enabling Python Editor Script Plugin.
This creates a new level and saves it at /Game/Maps/WinterfellApproach.
It is a greybox assembly helper, not a substitute for final art, lighting, collision or QA.
"""
import os
import unreal

PROJECT_DIR = unreal.Paths.project_dir()
SOURCE_DIR = os.path.join(PROJECT_DIR, "Content", "SourceMeshes")
DEST = "/Game/Environment/North/SourceMeshes"
LEVEL = "/Game/Maps/WinterfellApproach"

unreal.EditorAssetLibrary.make_directory("/Game/Maps")
unreal.EditorAssetLibrary.make_directory("/Game/Environment")
unreal.EditorAssetLibrary.make_directory("/Game/Environment/North")
unreal.EditorAssetLibrary.make_directory(DEST)

# Start with a clean level so repeated runs do not duplicate every actor.
if unreal.EditorLevelLibrary.new_level(LEVEL) is False:
    raise RuntimeError("Could not create level: " + LEVEL)

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
mesh_names = (
    "Winterfell_Gatehouse",
    "Northern_Pine_Tree",
    "Northern_Longhouse",
    "Northern_Cobblestone_Road",
)
for name in mesh_names:
    source_file = os.path.join(SOURCE_DIR, name + ".obj")
    if not os.path.isfile(source_file):
        raise RuntimeError("Missing source OBJ: " + source_file)
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source_file)
    task.set_editor_property("destination_path", DEST)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    asset_tools.import_asset_tasks([task])

def mesh_actor(name, location, rotation=None, scale=1.0):
    asset = unreal.load_asset(DEST + "/" + name + "." + name)
    if not asset:
        raise RuntimeError("Imported mesh not found: " + name)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_object(
        asset,
        unreal.Vector(location[0], location[1], location[2]),
        rotation or unreal.Rotator(0.0, 0.0, 0.0),
    )
    actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
    actor.set_actor_label(name)
    return actor

# Temporary ground plane: replace with sculpted landscape/terrain before production.
plane = unreal.load_asset("/Engine/BasicShapes/Plane.Plane")
if not plane:
    raise RuntimeError("Unreal basic Plane mesh is unavailable")
ground = unreal.EditorLevelLibrary.spawn_actor_from_object(
    plane, unreal.Vector(0.0, 0.0, -15.0), unreal.Rotator(0.0, 0.0, 0.0)
)
ground.set_actor_scale3d(unreal.Vector(65.0, 65.0, 1.0))
ground.set_actor_label("TEMP_Greybox_Ground_Replace_With_Landscape")

# Main landmark and approach road.
mesh_actor("Winterfell_Gatehouse", (0.0, 0.0, 0.0))
mesh_actor("Northern_Cobblestone_Road", (0.0, -1050.0, 0.0), scale=1.25)
mesh_actor("Northern_Cobblestone_Road", (0.0, -350.0, 0.0), scale=1.0)

# Settlement edge: buildings frame the road but leave the center open.
mesh_actor("Northern_Longhouse", (-1250.0, -250.0, 0.0), scale=1.0)
mesh_actor("Northern_Longhouse", (1250.0, -150.0, 0.0),
           unreal.Rotator(0.0, 180.0, 0.0), scale=0.82)

# Pine groves use repeated instances of the imported source mesh.
tree_positions = (
    (-1850, -150), (-2050, 450), (-1700, 1000), (-2250, 1250),
    (1850, -50), (2050, 550), (1750, 1050), (2250, 1350),
    (-1100, 1250), (1100, 1450),
)
for index, (x, y) in enumerate(tree_positions):
    size = 0.72 + (index % 3) * 0.12
    mesh_actor("Northern_Pine_Tree", (float(x), float(y), 0.0),
               unreal.Rotator(0.0, float((index * 37) % 360), 0.0), size)

# Player begins on the south/north-road approach, facing +Y toward the gate.
start = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.PlayerStart,
    unreal.Vector(0.0, -1700.0, 100.0),
    unreal.Rotator(0.0, 90.0, 0.0),
)
start.set_actor_label("PlayerStart_Landscape_Approach")

# Basic cold daylight; final lighting should be baked and profiled on target Android hardware.
sun = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.DirectionalLight,
    unreal.Vector(0.0, 0.0, 1800.0),
    unreal.Rotator(-35.0, -35.0, 0.0),
)
sun.set_actor_label("TEMP_Northern_Directional_Light")

if not unreal.EditorLevelLibrary.save_current_level():
    raise RuntimeError("Level was created but could not be saved")

unreal.log("LOTN: WinterfellApproach greybox created and saved. Add NPC/quest Blueprints, sculpt terrain, collisions, materials, lighting and mobile LODs before gameplay sign-off.")
