#!/usr/bin/env python3
"""Repository structure checks; this is not an Unreal compile or Android package."""
from __future__ import annotations
import ast
import json
import math
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
errors = []
checks = []

def require(condition, message):
    (checks if condition else errors).append(message)

try:
    project = json.loads((ROOT / "LORD_OF_THE_NORTH.uproject").read_text(encoding="utf-8"))
    require(project.get("FileVersion") == 3, "uproject JSON parses and uses FileVersion 3")
    require(any(m.get("Name") == "LordOfTheNorth" and m.get("Type") == "Runtime"
                for m in project.get("Modules", [])), "runtime module is declared")
    require("landscape" in project.get("Description", "").lower(),
            "project description identifies landscape orientation")
    enabled_plugins = {plugin.get("Name") for plugin in project.get("Plugins", [])
                       if plugin.get("Enabled") is True}
    require("PythonScriptPlugin" in enabled_plugins,
            "Python Editor scripting plugin is enabled")
    require("EditorScriptingUtilities" in enabled_plugins,
            "Editor scripting utilities plugin is enabled")
except Exception as exc:
    errors.append("uproject JSON is invalid: " + str(exc))

build_cs = ROOT / "Source/LordOfTheNorth/LordOfTheNorth.Build.cs"
require(build_cs.is_file(), "module Build.cs exists")
if build_cs.is_file():
    build_text = build_cs.read_text(encoding="utf-8")
    require('"Core"' in build_text and '"CoreUObject"' in build_text and '"Engine"' in build_text,
            "module declares core Unreal dependencies")
    require('"UMG"' in build_text, "module declares UMG dependency")

for name in ("LordOfTheNorth.Target.cs", "LordOfTheNorthEditor.Target.cs"):
    require((ROOT / "Source" / name).is_file(), name + " exists")

public_dir = ROOT / "Source/LordOfTheNorth/Public"
private_dir = ROOT / "Source/LordOfTheNorth/Private"
headers = sorted(public_dir.glob("*.h"))
sources = sorted(private_dir.glob("*.cpp"))
require(bool(headers), "public C++ headers are present")
require(bool(sources), "private C++ sources are present")

for header in headers:
    lines = header.read_text(encoding="utf-8").splitlines()
    includes = [line.strip() for line in lines if line.strip().startswith("#include")]
    generated = [line for line in includes if ".generated.h" in line]
    content_text = "\n".join(lines)
    if any(token in content_text for token in ("UCLASS(", "USTRUCT(", "UINTERFACE(")):
        require(len(generated) == 1, header.name + ": exactly one generated header include")
        if generated:
            require(bool(includes) and includes[-1] == generated[0],
                    header.name + ": generated header is the final include")

for source in sources:
    source_text = source.read_text(encoding="utf-8")
    if source.stem != "LordOfTheNorth":
        require('#include "' + source.stem + '.h"' in source_text,
                source.name + ": includes its matching header")

engine_config = ROOT / "Config/DefaultEngine.ini"
config_text = engine_config.read_text(encoding="utf-8") if engine_config.exists() else ""
require("Orientation=Landscape" in config_text, "Android orientation is landscape")
require("PackageName=com.lotn.game" in config_text, "Android package identifier is set")
require("EditorStartupMap=/Game/Maps/WinterfellApproach" in config_text,
        "WinterfellApproach is configured as the editor startup map")
require("GameDefaultMap=/Game/Maps/WinterfellApproach" in config_text,
        "WinterfellApproach is configured as the packaged game default map")
require("r.MobileHDR=False" in config_text, "mobile HDR is disabled in baseline config")

input_config = ROOT / "Config/DefaultInput.ini"
input_text = input_config.read_text(encoding="utf-8") if input_config.exists() else ""
require('ActionName="Interact"' in input_text, "interact action is mapped")
require('ActionName="Sprint"' in input_text, "sprint action is mapped")
require("DefaultVirtualJoysticks" in input_text, "default mobile virtual joysticks are configured")
require((public_dir / "LOTNInventoryComponent.h").is_file(), "inventory component header exists")
require((private_dir / "LOTNInventoryComponent.cpp").is_file(), "inventory component source exists")
require((public_dir / "LOTNInventoryItemRowWidget.h").is_file(), "inventory row widget header exists")
require((private_dir / "LOTNInventoryItemRowWidget.cpp").is_file(), "inventory row widget source exists")
require((public_dir / "LOTNWorldMapData.h").is_file(), "world map data header exists")
require((private_dir / "LOTNWorldMapData.cpp").is_file(), "world map data source exists")
character_header = (public_dir / "LOTNCharacter.h").read_text(encoding="utf-8")
character_source = (private_dir / "LOTNCharacter.cpp").read_text(encoding="utf-8")
for part in ("PlaceholderTorso", "PlaceholderHead", "PlaceholderLeftArm",
             "PlaceholderRightArm", "PlaceholderLeftLeg", "PlaceholderRightLeg"):
    require(part in character_header and part in character_source,
            "player greybox includes " + part)
require("NoCollision" in character_source and "BasicShapes/Capsule.Capsule" in character_source,
        "player greybox uses non-blocking engine primitive meshes")
require((private_dir / "Tests/LOTNGameplayTests.cpp").is_file(), "gameplay automation tests exist")
require((ROOT / ".github/workflows/build-unreal-android.yml").is_file(), "manual Unreal Android workflow exists")
player_controller = private_dir / "LOTNPlayerController.cpp"
require(player_controller.is_file() and "FInputModeGameAndUI" in player_controller.read_text(encoding="utf-8"),
        "player controller allows HUD interaction alongside gameplay input")
require((ROOT / "Content/MAPS_README.md").is_file(), "first map assembly guide exists")
source_meshes = ROOT / "Content/SourceMeshes"
for mesh_name in ("Winterfell_Gatehouse.obj", "Northern_Pine_Tree.obj",
                  "Northern_Longhouse.obj", "Northern_Cobblestone_Road.obj",
                  "Northern_Curtain_Wall.obj", "Northern_Watchtower.obj",
                  "Northern_Rolling_Terrain.obj", "Northern_Supply_Crate.obj",
                  "Northern_Palisade_Fence.obj", "Northern_Rock_Cluster.obj",
                  "Northern_Stone_Firepit.obj"):
    require((source_meshes / mesh_name).is_file(), mesh_name + " source mesh exists")

assembly_script = ROOT / "Scripts/create_winterfell_landscape.py"
require(assembly_script.is_file(), "Winterfell landscape assembly script exists")
if assembly_script.is_file():
    assembly_text = assembly_script.read_text(encoding="utf-8")
    try:
        ast.parse(assembly_text, filename=str(assembly_script))
        require(True, "Winterfell editor assembly script has valid Python syntax")
    except SyntaxError as exc:
        require(False, "Winterfell editor assembly script syntax: " + str(exc))
    for required in ("Northern_Curtain_Wall", "Northern_Watchtower",
                     "NavMeshBoundsVolume", "ExponentialHeightFog",
                     "Northern_Rolling_Terrain", "WinterfellApproach",
                     "LOTNNPC", "LOTNQuestObjective", "Winterfell_Delivery",
                     "CTF_USE_COMPLEX_AS_SIMPLE", "NPC_Northern_Guard_QuestGiver",
                     "Northern_Supply_Crate", "Northern_Palisade_Fence",
                     "Northern_Rock_Cluster", "Northern_Stone_Firepit"):
        require(required in assembly_text, "landscape assembly includes " + required)

# Validate OBJ source integrity without pretending to import Unreal assets.
for mesh_path in sorted(source_meshes.glob("*.obj")):
    vertices = []
    faces = 0
    malformed = False
    material_library = None
    used_materials = set()
    for line_number, line in enumerate(mesh_path.read_text(encoding="utf-8").splitlines(), 1):
        parts = line.strip().split()
        if not parts or parts[0].startswith("#"):
            continue
        if parts[0] == "mtllib" and len(parts) >= 2:
            material_library = parts[1]
        elif parts[0] == "usemtl" and len(parts) >= 2:
            used_materials.add(parts[1])
        elif parts[0] == "v":
            try:
                xyz = tuple(float(value) for value in parts[1:4])
                if len(xyz) != 3 or not all(math.isfinite(value) for value in xyz):
                    malformed = True
                vertices.append(xyz)
            except (ValueError, IndexError):
                malformed = True
        elif parts[0] == "f":
            faces += 1
            if len(parts) < 4:
                malformed = True
                continue
            for token in parts[1:]:
                try:
                    raw_index = int(token.split("/")[0])
                    resolved = raw_index - 1 if raw_index > 0 else len(vertices) + raw_index
                    if resolved < 0 or resolved >= len(vertices):
                        malformed = True
                except (ValueError, IndexError):
                    malformed = True
    require(bool(vertices) and faces > 0 and not malformed,
            mesh_path.name + ": OBJ vertices and face indices are valid")
    if material_library:
        mtl_path = mesh_path.parent / material_library
        require(mtl_path.is_file(), mesh_path.name + ": referenced MTL file exists")
        if mtl_path.is_file():
            declared = {
                parts[1] for line in mtl_path.read_text(encoding="utf-8").splitlines()
                if len(parts := line.strip().split()) >= 2 and parts[0] == "newmtl"
            }
            require(used_materials.issubset(declared),
                    mesh_path.name + ": all used MTL material names are declared")
    else:
        require(not used_materials, mesh_path.name + ": material usage has an MTL library")

for item in checks:
    print("PASS: " + item)
for item in errors:
    print("FAIL: " + item, file=sys.stderr)
print("\nStatic structure checks: " + str(len(checks)) + " passed, " + str(len(errors)) + " failed.")
print("NOTE: This is not an Unreal C++ compile, editor validation, Android package, or device test.")
sys.exit(1 if errors else 0)
