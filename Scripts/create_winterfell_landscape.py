"""Assemble the WinterfellApproach landscape greybox in Unreal Editor.

Run from the Unreal Editor Python console with Python Editor Script Plugin enabled.
Imports source OBJ meshes, creates a wide approach composition and saves
/Game/Maps/WinterfellApproach. This is an editor assembly helper, not a final art pass.
"""
import os
import unreal

PROJECT_DIR = unreal.Paths.project_dir()
SOURCE_DIR = os.path.join(PROJECT_DIR, "Content", "SourceMeshes")
DEST = "/Game/Environment/North/SourceMeshes"
LEVEL = "/Game/Maps/WinterfellApproach"

MESH_NAMES = (
    "Winterfell_Gatehouse",
    "Northern_Pine_Tree",
    "Northern_Longhouse",
    "Northern_Cobblestone_Road",
    "Northern_Curtain_Wall",
    "Northern_Watchtower",
    "Northern_Rolling_Terrain",
)

for folder in ("/Game/Maps", "/Game/Environment",
               "/Game/Environment/North", DEST):
    unreal.EditorAssetLibrary.make_directory(folder)

if unreal.EditorLevelLibrary.new_level(LEVEL) is False:
    raise RuntimeError("Could not create level: " + LEVEL)

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
for name in MESH_NAMES:
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

def mesh_actor(name, location, yaw=0.0, scale=1.0):
    asset = unreal.load_asset(DEST + "/" + name + "." + name)
    if not asset:
        raise RuntimeError("Imported mesh not found: " + name)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_object(
        asset,
        unreal.Vector(float(location[0]), float(location[1]), float(location[2])),
        unreal.Rotator(0.0, float(yaw), 0.0),
    )
    if not actor:
        raise RuntimeError("Could not place mesh actor: " + name)
    actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
    actor.set_actor_label(name)
    return actor

def actor_of(actor_class, label, location, rotation, scale=None):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        unreal.Vector(float(location[0]), float(location[1]), float(location[2])),
        rotation,
    )
    if not actor:
        raise RuntimeError("Could not spawn actor: " + label)
    actor.set_actor_label(label)
    if scale:
        actor.set_actor_scale3d(unreal.Vector(scale[0], scale[1], scale[2]))
    return actor

# Rolling terrain source mesh replaces the featureless temporary plane.
# It is a first-pass terrain surface; verify/import simple collision in Editor.
mesh_actor("Northern_Rolling_Terrain", (0, 0, -8))

# Landmark and flanking defensive wall: wall segments meet the gatehouse edges.
mesh_actor("Winterfell_Gatehouse", (0, 0, 0))
mesh_actor("Northern_Curtain_Wall", (-1775, 0, 0))
mesh_actor("Northern_Curtain_Wall", (1775, 0, 0), yaw=180.0)
mesh_actor("Northern_Watchtower", (-3100, 0, 0))
mesh_actor("Northern_Watchtower", (3100, 0, 0))

# Three adjoining road tiles lead from the player start to the gate.
for index, y in enumerate((-1400, -750, -100)):
    mesh_actor("Northern_Cobblestone_Road", (0, y, 0),
               scale=1.0 if index else 1.03)

# Settlement buildings sit off the central camera and movement corridor.
mesh_actor("Northern_Longhouse", (-1250, -800, 0), yaw=8.0, scale=1.0)
mesh_actor("Northern_Longhouse", (1250, -650, 0), yaw=172.0, scale=0.82)
mesh_actor("Northern_Longhouse", (1850, -1150, 0), yaw=195.0, scale=0.68)

# Side groves frame the approach; keep the central lane and NPC sightlines clear.
tree_positions = (
    (-1900, -1050), (-2200, -450), (-1850, 250), (-2350, 850),
    (-1700, 1450), (-2600, 1650), (1900, -1000), (2200, -350),
    (1850, 350), (2350, 950), (1750, 1500), (2650, 1750),
)
for index, (x, y) in enumerate(tree_positions):
    size = 1.05 + (index % 4) * 0.13
    mesh_actor("Northern_Pine_Tree", (x, y, 0),
               yaw=float((index * 37) % 360), scale=size)

# Player start and navigation coverage for the first walkable slice.
actor_of(unreal.PlayerStart, "PlayerStart_Landscape_Approach",
         (0, -1700, 100), unreal.Rotator(0.0, 90.0, 0.0))
actor_of(unreal.NavMeshBoundsVolume, "NAV_WinterfellApproach",
         (0, -350, 250), unreal.Rotator(0.0, 0.0, 0.0),
         (38.0, 24.0, 5.0))

# Cold daylight baseline. Tune/bake the final lighting after materials and
# foliage are imported; keep expensive dynamic effects out of the mobile target.
sun = actor_of(unreal.DirectionalLight, "TEMP_Northern_Directional_Light",
               (0, 0, 1800), unreal.Rotator(-35.0, -35.0, 0.0))
fog = actor_of(unreal.ExponentialHeightFog, "TEMP_Northern_Atmospheric_Fog",
               (0, 0, 0), unreal.Rotator(0.0, 0.0, 0.0))

if not unreal.EditorLevelLibrary.save_current_level():
    raise RuntimeError("Level was created but could not be saved")

unreal.log(
    "LOTN: WinterfellApproach landscape greybox saved. "
    "Inspect rolling terrain collision/materials, add quest NPC Blueprints, "
    "and profile mobile lighting before sign-off."
)
