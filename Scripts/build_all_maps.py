"""Build and validate all nine LOTN maps in one Unreal Editor Python session.

Run after compiling the LordOfTheNorthEditor target:
UnrealEditor-Cmd <project> -unattended -nop4 -NullRHI -run=pythonscript -script=<this file>
"""
import os
import unreal

PROJECT_DIR = unreal.Paths.project_dir()
SCRIPTS_DIR = os.path.join(PROJECT_DIR, "Scripts")


def run_script(name):
    script_path = os.path.join(SCRIPTS_DIR, name)
    if not os.path.isfile(script_path):
        raise RuntimeError("LOTN map pipeline is missing " + script_path)
    unreal.log("LOTN PIPELINE: starting " + name)
    with open(script_path, "r", encoding="utf-8") as source_file:
        source = source_file.read()
    namespace = {"__file__": script_path, "__name__": "__main__"}
    exec(compile(source, script_path, "exec"), namespace, namespace)
    unreal.log("LOTN PIPELINE: completed " + name)


# Import the shared OBJ source meshes and assemble the first playable slice.
run_script("create_winterfell_landscape.py")

# Compile must already have registered the native HISM actor before this step.
run_script("create_northern_region_levels.py")

# The regional builder leaves The Wall loaded. Validate Winterfell first,
# then let its regional validator load and inspect each of the other eight maps.
winterfell_path = "/Game/Maps/WinterfellApproach"
if not unreal.EditorLoadingAndSavingUtils.load_map(winterfell_path):
    raise RuntimeError("LOTN pipeline could not reload WinterfellApproach for validation")
run_script("validate_winterfell_level.py")
run_script("validate_northern_region_levels.py")

expected_maps = (
    "WinterfellApproach", "LOTN_TheDreadfort", "LOTN_WhiteHarbor",
    "LOTN_BearIsland", "LOTN_Karhold", "LOTN_LastHearth",
    "LOTN_DeepwoodMotte", "LOTN_GreywaterWatch", "LOTN_TheWall",
)
for map_name in expected_maps:
    asset_path = "/Game/Maps/" + map_name
    if not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        raise RuntimeError("LOTN pipeline did not create map " + asset_path)

unreal.log("LOTN PIPELINE SUCCESS: all nine maps were assembled and passed Editor validation.")
