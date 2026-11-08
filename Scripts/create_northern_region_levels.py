"""Create eight standalone northern-region greybox maps in Unreal Editor.

Prerequisite: run create_winterfell_landscape.py once so all shared source meshes
are imported. Then run this script from the Unreal Editor Python console.
Every region gets its own saved .umap, landscape terrain, road network,
distinct landmark composition, PlayerStart, navigation bounds and mobile-safe
baseline lighting. These are greybox maps; art, collision, nav and lighting QA
remain required before release.
"""
import unreal

SOURCE = "/Game/Environment/North/SourceMeshes"
MAPS = "/Game/Maps"
REGIONS = (
    {
        "id": "TheDreadfort", "level": "LOTN_TheDreadfort",
        "terrain": (-250, 0), "start": (0, -2600),
        "landmarks": [
            ("Winterfell_Gatehouse", (0, 700), 0, 1.05),
            ("Northern_Watchtower", (-2500, 700), 0, 1.0),
            ("Northern_Watchtower", (2500, 700), 0, 1.0),
            ("Northern_Curtain_Wall", (-1550, 700), 0, 0.82),
            ("Northern_Curtain_Wall", (1550, 700), 180, 0.82),
            ("Northern_Longhouse", (-1450, -800), 12, 0.9),
            ("Northern_Longhouse", (1450, -850), 170, 0.9),
        ],
        "trees": [(-3300,-2400),(-2800,-1500),(-3400,-300),(-3000,1700),(3000,-2300),(3300,-1000),(3000,500),(2800,2200)],
        "props": [("Northern_Rock_Cluster",(-3300,900)),("Northern_Rock_Cluster",(3300,1300)),("Northern_Wolf_Banner",(-450,350)),("Northern_Wolf_Banner",(450,350))],
        "fog": 0.0030, "density": 0.0032,
    },
    {
        "id": "WhiteHarbor", "level": "LOTN_WhiteHarbor",
        "terrain": (0, 0), "start": (0, -2700),
        "landmarks": [
            ("Northern_Longhouse",(-1800,300),8,1.0),("Northern_Longhouse",(0,900),0,1.1),
            ("Northern_Longhouse",(1800,300),-8,0.9),("Northern_Watchtower",(2600,1700),0,0.85),
            ("Northern_Cobblestone_Road",(0,1000),0,1.7),("Northern_Cobblestone_Road",(0,0),0,1.7),
            ("Northern_Cobblestone_Road",(0,-1000),0,1.7),
        ],
        "trees": [(-3200,-1700),(-3000,1600),(3100,-1700),(3200,1800),(-2300,2600),(2300,2600)],
        "props": [("Northern_Handcart",(-1150,-450)),("Northern_Handcart",(1200,650)),("Northern_Woodpile",(-2050,850)),("Northern_Wolf_Banner",(-250,1200)),("Northern_Wolf_Banner",(250,1200))],
        "fog": 0.0020, "density": 0.0020,
    },
    {
        "id": "BearIsland", "level": "LOTN_BearIsland",
        "terrain": (0, 0), "start": (0, -2700),
        "landmarks": [
            ("Northern_Longhouse",(0,350),0,1.15),("Northern_Watchtower",(-2100,1200),0,0.95),
            ("Northern_Cobblestone_Road",(0,-900),0,1.35),("Northern_Stone_Firepit",(1150,-250),0,1.0),
        ],
        "trees": [(-2800,-2300),(-2200,-1600),(-3100,-700),(-2700,300),(-2300,1800),(-1500,2400),(2600,-2200),(2200,-1500),(3100,-500),(2700,800),(2300,1900),(1500,2500)],
        "props": [("Northern_Rock_Cluster",(-3300,1200)),("Northern_Rock_Cluster",(3300,1400)),("Northern_Woodpile",(900,700)),("Northern_Handcart",(1450,650))],
        "fog": 0.0025, "density": 0.0027,
    },
    {
        "id": "Karhold", "level": "LOTN_Karhold",
        "terrain": (0, 0), "start": (0, -2700),
        "landmarks": [
            ("Winterfell_Gatehouse",(0,800),0,0.95),("Northern_Watchtower",(-2600,800),0,0.9),
            ("Northern_Watchtower",(2600,800),0,0.9),("Northern_Curtain_Wall",(-1600,800),0,0.78),
            ("Northern_Curtain_Wall",(1600,800),180,0.78),("Northern_Longhouse",(0,-900),0,0.9),
        ],
        "trees": [(-3300,-2200),(-2900,-900),(-3300,1500),(3300,-2200),(2900,-900),(3300,1500)],
        "props": [("Northern_Snowdrift",(-750,-500)),("Northern_Snowdrift",(750,-500)),("Northern_Rock_Cluster",(-3100,2200)),("Northern_Rock_Cluster",(3100,2200)),("Northern_Wolf_Banner",(-420,450)),("Northern_Wolf_Banner",(420,450))],
        "fog": 0.0032, "density": 0.0034,
    },
    {
        "id": "LastHearth", "level": "LOTN_LastHearth",
        "terrain": (0, 0), "start": (0, -2700),
        "landmarks": [
            ("Northern_Longhouse",(-1500,350),5,1.05),("Northern_Longhouse",(0,900),0,1.15),
            ("Northern_Longhouse",(1500,350),-5,0.95),("Northern_Cobblestone_Road",(0,-700),0,1.45),
            ("Northern_Stone_Firepit",(0,-100),0,1.0),
        ],
        "trees": [(-3200,-2000),(-2800,-900),(-3300,1300),(3200,-2000),(2800,-900),(3300,1300),(-1900,2400),(1900,2400)],
        "props": [("Northern_Woodpile",(-1850,850)),("Northern_Woodpile",(1850,850)),("Northern_Handcart",(1900,-500)),("Northern_Wolf_Banner",(-250,1300)),("Northern_Wolf_Banner",(250,1300))],
        "fog": 0.0025, "density": 0.0023,
    },
    {
        "id": "DeepwoodMotte", "level": "LOTN_DeepwoodMotte",
        "terrain": (0, 0), "start": (0, -2700),
        "landmarks": [
            ("Northern_Longhouse",(0,500),0,1.0),("Northern_Watchtower",(1800,1200),0,0.9),
            ("Northern_Cobblestone_Road",(0,-800),0,1.3),("Northern_Stone_Firepit",(-1000,100),0,0.9),
        ],
        "trees": [(-3200,-2600),(-2600,-2200),(-3300,-1300),(-2600,-700),(-3400,200),(-2800,900),(-3300,1900),(-2200,2500),(-1500,2100),(3200,-2600),(2600,-2200),(3300,-1300),(2600,-700),(3400,200),(2800,900),(3300,1900),(2200,2500),(1500,2100)],
        "props": [("Northern_Woodpile",(-1200,950)),("Northern_Handcart",(1200,900)),("Northern_Rock_Cluster",(-2000,-200)),("Northern_Rock_Cluster",(2100,-100))],
        "fog": 0.0030, "density": 0.0028,
    },
    {
        "id": "GreywaterWatch", "level": "LOTN_GreywaterWatch",
        "terrain": (0, 0), "start": (0, -2700),
        "landmarks": [
            ("Northern_Longhouse",(-1100,450),0,0.9),("Northern_Longhouse",(1100,900),0,0.85),
            ("Northern_Watchtower",(0,1800),0,0.75),("Northern_Cobblestone_Road",(0,-800),0,1.2),
        ],
        "trees": [(-3100,-2100),(-2700,-1100),(-3200,0),(-2500,1200),(-1800,2300),(3100,-2100),(2700,-1100),(3200,0),(2500,1200),(1800,2300)],
        "props": [("Northern_Rock_Cluster",(-1700,-500)),("Northern_Rock_Cluster",(1700,-300)),("Northern_Woodpile",(-1400,900)),("Northern_Handcart",(1300,1200)),("Northern_Snowdrift",(-500,-1200)),("Northern_Snowdrift",(500,-1200))],
        "fog": 0.0040, "density": 0.0040,
    },
    {
        "id": "TheWall", "level": "LOTN_TheWall",
        "terrain": (0, 0), "start": (0, -2700),
        "landmarks": [
            ("Northern_Watchtower",(-2400,1000),0,1.1),("Northern_Watchtower",(2400,1000),0,1.1),
            ("Northern_Curtain_Wall",(0,1000),0,1.5),("Northern_Longhouse",(0,-500),0,0.85),
            ("Northern_Cobblestone_Road",(0,-1600),0,1.3),
        ],
        "trees": [(-3300,-2400),(-3000,-1500),(-3300,-500),(3300,-2400),(3000,-1500),(3300,-500)],
        "props": [("Northern_Snowdrift",(-900,-1200)),("Northern_Snowdrift",(900,-1200)),("Northern_Rock_Cluster",(-3200,2100)),("Northern_Rock_Cluster",(3200,2100))],
        "fog": 0.0038, "density": 0.0035,
    },
)

def fail(message):
    raise RuntimeError("LOTN regional assembly: " + message)

def spawn_mesh(editor_actors, name, location, yaw=0.0, scale=1.0, label=None):
    asset = unreal.load_asset(SOURCE + "/" + name + "." + name)
    if not asset:
        fail("shared mesh is missing: " + name + "; run the Winterfell import script first")
    actor = editor_actors.spawn_actor_from_object(
        asset,
        unreal.Vector(float(location[0]), float(location[1]), float(location[2])),
        unreal.Rotator(0.0, float(yaw), 0.0),
    )
    if not actor:
        fail("could not spawn " + name)
    actor.set_actor_scale3d(unreal.Vector(float(scale), float(scale), float(scale)))
    actor.set_actor_label(label or name)
    if name in ("Northern_Pine_Tree", "Northern_Rock_Cluster", "Northern_Stone_Firepit",
                "Northern_Supply_Crate", "Northern_Woodpile", "Northern_Handcart",
                "Northern_Snowdrift", "Northern_Wolf_Banner"):
        actor.set_actor_enable_collision(False)
    return actor

def spawn_class(editor_actors, cls, label, location, rotation=None, scale=None):
    actor = editor_actors.spawn_actor_from_class(
        cls, unreal.Vector(float(location[0]), float(location[1]), float(location[2])),
        rotation or unreal.Rotator(0.0, 0.0, 0.0),
    )
    if not actor:
        fail("could not spawn actor " + label)
    actor.set_actor_label(label)
    if scale:
        actor.set_actor_scale3d(unreal.Vector(float(scale[0]), float(scale[1]), float(scale[2])))
    return actor

for region in REGIONS:
    level_path = MAPS + "/" + region["level"]
    unreal.EditorAssetLibrary.make_directory(MAPS)
    level_editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    editor_actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if not level_editor or not editor_actors:
        fail("required Unreal Editor subsystems are unavailable")
    if not level_editor.new_level(level_path):
        fail("could not create level " + level_path)

    spawn_mesh(editor_actors, "Northern_Rolling_Terrain", (0, 0, -8),
               label="ENV_" + region["id"] + "_Terrain")

    for index, (name, xy, yaw, scale) in enumerate(region["landmarks"]):
        spawn_mesh(editor_actors, name, (xy[0], xy[1], 0),
                   yaw, scale, "LANDMARK_{:02d}_{}".format(index + 1, name))

    for index, (name, xy) in enumerate(region["props"]):
        spawn_mesh(editor_actors, name, (xy[0], xy[1], 0),
                   (index * 43) % 360, 0.9 + (index % 3) * 0.12,
                   "PROP_{:02d}_{}".format(index + 1, name))

    tree_class = unreal.load_class(None, "/Script/LordOfTheNorth.LOTNFoliageCluster")
    tree_mesh = unreal.load_asset(SOURCE + "/Northern_Pine_Tree.Northern_Pine_Tree")
    if not tree_class or not tree_mesh:
        fail("HISM foliage class or pine mesh is missing; compile C++ first")
    forest = spawn_class(editor_actors, tree_class,
                         "ENV_" + region["id"] + "_Forest_HISM", (0, 0, 0))
    forest.configure_tree_mesh(tree_mesh)
    for index, (x, y) in enumerate(region["trees"]):
        forest.add_tree_instance(
            unreal.Vector(float(x), float(y), 0.0),
            float((index * 47) % 360),
            float(0.88 + (index % 4) * 0.12),
        )

    spawn_class(editor_actors, unreal.PlayerStart,
                "PlayerStart_" + region["id"],
                (region["start"][0], region["start"][1], 116),
                unreal.Rotator(0.0, 90.0, 0.0))
    spawn_class(editor_actors, unreal.NavMeshBoundsVolume,
                "NAV_" + region["id"], (0, 0, 250),
                scale=(40.0, 40.0, 5.0))

    sun = spawn_class(editor_actors, unreal.DirectionalLight,
                      "TEMP_" + region["id"] + "_Sun", (0, 0, 1800),
                      unreal.Rotator(-35.0, -35.0, 0.0))
    sun_component = sun.get_component_by_class(unreal.DirectionalLightComponent)
    if not sun_component:
        fail("directional light component missing for " + region["id"])
    sun_component.set_mobility(unreal.ComponentMobility.STATIC)
    sun_component.set_editor_property("intensity", 68000.0)
    sun_component.set_editor_property("cast_shadows", True)

    fog = spawn_class(editor_actors, unreal.ExponentialHeightFog,
                      "TEMP_" + region["id"] + "_Fog", (0, 0, 0))
    fog_component = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    if not fog_component:
        fail("fog component missing for " + region["id"])
    fog_component.set_editor_property("fog_density", region["density"])
    fog_component.set_editor_property("fog_height_falloff", region["fog"])
    fog_component.set_editor_property("fog_max_opacity", 0.55)
    fog_component.set_editor_property("start_distance", 1800.0)
    fog_component.set_editor_property("enable_volumetric_fog", False)

    if not level_editor.save_current_level():
        fail("could not save " + level_path)
    unreal.log("LOTN REGION SAVED: " + level_path)
    
unreal.log("LOTN: all eight regional greybox maps were assembled and saved. Run validate_northern_region_levels.py next.")
