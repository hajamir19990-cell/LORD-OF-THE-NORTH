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
    "Northern_Supply_Crate",
    "Northern_Palisade_Fence",
    "Northern_Rock_Cluster",
    "Northern_Stone_Firepit",
)

for folder in ("/Game/Maps", "/Game/Environment",
               "/Game/Environment/North", DEST):
    unreal.EditorAssetLibrary.make_directory(folder)

level_editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
editor_actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
if not level_editor or not editor_actors:
    raise RuntimeError("Unreal level/actor editor subsystems are unavailable")
if not level_editor.new_level(LEVEL):
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
    imported_paths = task.get_editor_property("imported_object_paths")
    if not imported_paths:
        raise RuntimeError("Unreal did not import source mesh: " + source_file)

# Keep complex collision only on surfaces and structures that must block movement.
# Decorative trees, rocks and the firepit are non-blocking to avoid snagging and
# unnecessary mobile collision work. Replace structural complex collision with
# simplified UCX meshes before release and profile on target devices.
COLLISION_MESHES = {
    "Northern_Rolling_Terrain",
    "Northern_Cobblestone_Road",
    "Winterfell_Gatehouse",
    "Northern_Curtain_Wall",
    "Northern_Watchtower",
    "Northern_Longhouse",
    "Northern_Palisade_Fence",
}
NON_BLOCKING_MESHES = {
    "Northern_Pine_Tree",
    "Northern_Rock_Cluster",
    "Northern_Stone_Firepit",
    "Northern_Supply_Crate",
}

for name in COLLISION_MESHES:
    asset = unreal.load_asset(DEST + "/" + name + "." + name)
    if not asset:
        raise RuntimeError("Imported collision mesh missing during setup: " + name)
    body = asset.get_editor_property("body_setup")
    if not body:
        raise RuntimeError("No BodySetup available for collision mesh: " + name)
    body.set_editor_property(
        "collision_trace_flag",
        unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE,
    )
    asset.post_edit_change()
    unreal.EditorAssetLibrary.save_loaded_asset(asset)

def mesh_actor(name, location, yaw=0.0, scale=1.0):
    asset = unreal.load_asset(DEST + "/" + name + "." + name)
    if not asset:
        raise RuntimeError("Imported mesh not found: " + name)
    actor = editor_actors.spawn_actor_from_object(
        asset,
        unreal.Vector(float(location[0]), float(location[1]), float(location[2])),
        unreal.Rotator(0.0, float(yaw), 0.0),
    )
    if not actor:
        raise RuntimeError("Could not place mesh actor: " + name)
    actor.set_actor_scale3d(unreal.Vector(scale, scale, scale))
    actor.set_actor_label(name)
    if name in NON_BLOCKING_MESHES:
        actor.set_actor_enable_collision(False)
    return actor

def actor_of(actor_class, label, location, rotation, scale=None):
    actor = editor_actors.spawn_actor_from_class(
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
# Use complex-as-simple collision for this static, low-poly ground surface so
# the first playable slice has a continuous walkable surface. Profile on device.
terrain_asset = unreal.load_asset(DEST + "/Northern_Rolling_Terrain.Northern_Rolling_Terrain")
if not terrain_asset:
    raise RuntimeError("Imported rolling terrain mesh is missing")
terrain_body = terrain_asset.get_editor_property("body_setup")
if not terrain_body:
    raise RuntimeError("Rolling terrain has no BodySetup; collision could not be configured")
terrain_body.set_editor_property(
    "collision_trace_flag",
    unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE,
)
terrain_asset.post_edit_change()
unreal.EditorAssetLibrary.save_loaded_asset(terrain_asset)
mesh_actor("Northern_Rolling_Terrain", (0, 0, -8))

# Sample the imported source grid so props and characters sit on the actual
# terrain surface. The castle wall/tower footprint is flattened in the source
# mesh to create a stable foundation platform.
terrain_vertices = []
with open(os.path.join(SOURCE_DIR, "Northern_Rolling_Terrain.obj"),
          "r", encoding="utf-8") as terrain_file:
    for line in terrain_file:
        parts = line.strip().split()
        if len(parts) >= 4 and parts[0] == "v":
            terrain_vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
if len(terrain_vertices) != 49 * 49:
    raise RuntimeError("Unexpected terrain grid size; expected 49 x 49 vertices")

def terrain_world_z(x, y):
    gx = max(0.0, min(48.0, (float(x) + 4200.0) / 175.0))
    gy = max(0.0, min(48.0, (float(y) + 4200.0) / 175.0))
    ix = min(47, int(gx))
    iy = min(47, int(gy))
    fx = gx - ix
    fy = gy - iy

    def sample(column, row):
        return terrain_vertices[row * 49 + column][2]

    z00 = sample(ix, iy)
    z10 = sample(ix + 1, iy)
    z01 = sample(ix, iy + 1)
    z11 = sample(ix + 1, iy + 1)
    local_z = (
        z00 * (1.0 - fx) * (1.0 - fy)
        + z10 * fx * (1.0 - fy)
        + z01 * (1.0 - fx) * fy
        + z11 * fx * fy
    )
    return local_z - 8.0

# Landmark and flanking defensive wall: wall segments meet the gatehouse edges.
mesh_actor("Winterfell_Gatehouse", (0, 0, 0))
mesh_actor("Northern_Curtain_Wall", (-1775, 0, 0))
mesh_actor("Northern_Curtain_Wall", (1775, 0, 0), yaw=180.0)
mesh_actor("Northern_Watchtower", (-3100, 0, 0))
mesh_actor("Northern_Watchtower", (3100, 0, 0))

# Three adjoining road tiles lead from the player start to the gate.
for index, y in enumerate((-1400, -750, -100)):
    road_base_z = terrain_world_z(0, y)
    mesh_actor("Northern_Cobblestone_Road", (0, y, road_base_z),
               scale=1.0 if index else 1.03)

# Timber palisades define the settlement edge without closing the main road.
for x in (-850, 850):
    for y in (-1200, -300):
        mesh_actor("Northern_Palisade_Fence", (x, y, terrain_world_z(x, y)),
                   yaw=0.0 if x < 0 else 180.0)

# Snow-capped rock clusters frame the outer terrain and break up empty margins.
rock_positions = (
    (-3300, -1750), (-2850, -650), (-3400, 650), (-2850, 1850),
    (3300, -1700), (2850, -600), (3400, 700), (2900, 1900),
)
for index, (x, y) in enumerate(rock_positions):
    mesh_actor("Northern_Rock_Cluster", (x, y, terrain_world_z(x, y) - 6.0),
               yaw=float(index * 29), scale=0.9 + (index % 3) * 0.12)

# Settlement buildings sit off the central camera and movement corridor.
mesh_actor("Northern_Longhouse", (-1250, -800, terrain_world_z(-1250, -800)), yaw=8.0, scale=1.0)
mesh_actor("Northern_Longhouse", (1250, -650, terrain_world_z(1250, -650)), yaw=172.0, scale=0.82)
mesh_actor("Northern_Longhouse", (1850, -1150, terrain_world_z(1850, -1150)), yaw=195.0, scale=0.68)

# A small stone firepit adds a readable settlement landmark away from the road.
mesh_actor("Northern_Stone_Firepit", (1400, -1550, terrain_world_z(1400, -1550)), yaw=12.0)

# Side groves frame the approach; keep the central lane and NPC sightlines clear.
tree_positions = (
    (-1900, -1050), (-2200, -450), (-1850, 250), (-2350, 850),
    (-1700, 1450), (-2600, 1650), (2450, -1000), (2200, -350),
    (1850, 350), (2350, 950), (1750, 1500), (2650, 1750),
)
for index, (x, y) in enumerate(tree_positions):
    size = 1.05 + (index % 4) * 0.13
    mesh_actor("Northern_Pine_Tree", (x, y, terrain_world_z(x, y)),
               yaw=float((index * 37) % 360), scale=size)

# Native C++ NPCs and quest objective make the first slice playable without
# requiring Blueprint children. Temporary engine primitives are visual markers;
# replace them with licensed character/prop assets during the art pass.
npc_class = unreal.load_class(None, "/Script/LordOfTheNorth.LOTNNPC")
objective_class = unreal.load_class(None, "/Script/LordOfTheNorth.LOTNQuestObjective")
if not npc_class or not objective_class:
    raise RuntimeError("LOTNNPC/LOTNQuestObjective classes are unavailable; compile the Editor target first")

def as_text(value):
    return unreal.TextLibrary.conv_string_to_text(value)

def spawn_npc(label, display_name, location, dialogue, offers_quest=False):
    # ACharacter's capsule origin is its center; lift it by the default 96 cm
    # half-height so the capsule rests on the terrain instead of clipping below it.
    npc_location = (location[0], location[1], location[2] + 96.0)
    npc = actor_of(npc_class, label, npc_location, unreal.Rotator(0.0, 0.0, 0.0))
    npc.set_editor_property("display_name", as_text(display_name))
    npc.set_editor_property("dialogue_text", as_text(dialogue))
    npc.set_editor_property("b_offers_quest", offers_quest)
    if offers_quest:
        npc.set_editor_property("quest_id", "Winterfell_Delivery")
        npc.set_editor_property("quest_title", as_text("A Northern Errand"))
        npc.set_editor_property("quest_description", as_text(
            "Inspect the supply marker on the road, then return to the northern guard."
        ))
        npc.set_editor_property("quest_objective", as_text(
            "Inspect the supply marker beside the Winterfell approach road."
        ))
        npc.set_editor_property("required_progress", 1)
        npc.set_editor_property("gold_reward", 25)
        npc.set_editor_property("renown_reward", 5)

    return npc

spawn_npc(
    "NPC_Northern_Guard_QuestGiver",
    "Northern Guard",
    (0, -1320, terrain_world_z(0, -1320) + 16.0),
    "The supply marker on the road must be checked. Return to me when you have inspected it.",
    True,
)
spawn_npc(
    "NPC_Winterfell_Scout",
    "Winterfell Scout",
    (520, -420, terrain_world_z(520, -420)),
    "Keep to the road. The pines hide old tracks, and the gate is closer than it looks.",
    False,
)

objective = actor_of(
    objective_class,
    "OBJ_Winterfell_Supply_Marker",
    (0, -430, terrain_world_z(0, -430) + 16.0),
    unreal.Rotator(0.0, 0.0, 0.0),
)
objective.set_editor_property("quest_id", "Winterfell_Delivery")
objective.set_editor_property("progress_amount", 1)
objective.set_editor_property("interaction_label", as_text("Inspect supply marker"))
objective_mesh = objective.get_editor_property("visual_mesh")
crate = unreal.load_asset(DEST + "/Northern_Supply_Crate.Northern_Supply_Crate")
if not crate:
    raise RuntimeError("Imported northern supply crate mesh is missing")
if objective_mesh:
    objective_mesh.set_static_mesh(crate)
    objective_mesh.set_relative_location(unreal.Vector(0.0, 0.0, 0.0))
    objective_mesh.set_relative_scale3d(unreal.Vector(1.0, 1.0, 1.0))

# Player start and navigation coverage for the first walkable slice.
actor_of(unreal.PlayerStart, "PlayerStart_Landscape_Approach",
         (0, -1700, terrain_world_z(0, -1700) + 116.0), unreal.Rotator(0.0, 90.0, 0.0))
actor_of(unreal.NavMeshBoundsVolume, "NAV_WinterfellApproach",
         (0, -350, 250), unreal.Rotator(0.0, 0.0, 0.0),
         (38.0, 24.0, 5.0))

# Cold daylight baseline. Tune/bake the final lighting after materials and
# foliage are imported; keep expensive dynamic effects out of the mobile target.
sun = actor_of(unreal.DirectionalLight, "TEMP_Northern_Directional_Light",
               (0, 0, 1800), unreal.Rotator(-35.0, -35.0, 0.0))
fog = actor_of(unreal.ExponentialHeightFog, "TEMP_Northern_Atmospheric_Fog",
               (0, 0, 0), unreal.Rotator(0.0, 0.0, 0.0))

if not level_editor.save_current_level():
    raise RuntimeError("Level was created but could not be saved")

unreal.log(
    "LOTN: WinterfellApproach landscape greybox saved. "
    "Inspect terrain collision/materials, replace temporary NPC/quest markers "
    "with final art, and profile mobile lighting before sign-off."
)
