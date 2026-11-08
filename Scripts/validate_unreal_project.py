#!/usr/bin/env python3
"""Repository structure checks; this is not an Unreal compile or Android package."""
from __future__ import annotations
import json
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
require("Orientation=Portrait" in config_text, "Android orientation is portrait")
require("PackageName=com.lotn.game" in config_text, "Android package identifier is set")
require("r.MobileHDR=False" in config_text, "mobile HDR is disabled in baseline config")

input_config = ROOT / "Config/DefaultInput.ini"
input_text = input_config.read_text(encoding="utf-8") if input_config.exists() else ""
require('ActionName="Interact"' in input_text, "interact action is mapped")
require('ActionName="Sprint"' in input_text, "sprint action is mapped")
require((public_dir / "LOTNInventoryComponent.h").is_file(), "inventory component header exists")
require((private_dir / "LOTNInventoryComponent.cpp").is_file(), "inventory component source exists")
require((public_dir / "LOTNInventoryItemRowWidget.h").is_file(), "inventory row widget header exists")
require((private_dir / "LOTNInventoryItemRowWidget.cpp").is_file(), "inventory row widget source exists")
require((public_dir / "LOTNWorldMapData.h").is_file(), "world map data header exists")
require((private_dir / "LOTNWorldMapData.cpp").is_file(), "world map data source exists")
require((private_dir / "Tests/LOTNGameplayTests.cpp").is_file(), "gameplay automation tests exist")
require((ROOT / ".github/workflows/build-unreal-android.yml").is_file(), "manual Unreal Android workflow exists")
require((ROOT / "Content/MAPS_README.md").is_file(), "first map assembly guide exists")

for item in checks:
    print("PASS: " + item)
for item in errors:
    print("FAIL: " + item, file=sys.stderr)
print("\nStatic structure checks: " + str(len(checks)) + " passed, " + str(len(errors)) + " failed.")
print("NOTE: This is not an Unreal C++ compile, editor validation, Android package, or device test.")
sys.exit(1 if errors else 0)
