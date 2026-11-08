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
    text = build_cs.read_text(encoding="utf-8")
    require('"Core"' in text and '"CoreUObject"' in text and '"Engine"' in text,
            "module declares core Unreal dependencies")

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
    content = "\n".join(lines)
    if any(token in content for token in ("UCLASS(", "USTRUCT(", "UINTERFACE(")):
        require(len(generated) == 1, header.name + ": exactly one generated header include")
        if generated:
            require(bool(includes) and includes[-1] == generated[0],
                    header.name + ": generated header is the final include")

for source in sources:
    content = source.read_text(encoding="utf-8")
    if source.stem != "LordOfTheNorth":
        require('#include "' + source.stem + '.h"' in content,
                source.name + ": includes its matching header")

engine_config = ROOT / "Config/DefaultEngine.ini"
config_text = engine_config.read_text(encoding="utf-8") if engine_config.exists() else ""
require("Orientation=Portrait" in config_text, "Android orientation is portrait")
require("r.MobileHDR=False" in config_text, "mobile HDR is disabled in baseline config")

input_config = ROOT / "Config/DefaultInput.ini"
input_text = input_config.read_text(encoding="utf-8") if input_config.exists() else ""
require('ActionName="Interact"' in input_text, "interact action is mapped")

for item in checks:
    print("PASS: " + item)
for item in errors:
    print("FAIL: " + item, file=sys.stderr)
print("\nStatic structure checks: " + str(len(checks)) + " passed, " + str(len(errors)) + " failed.")
print("NOTE: This is not an Unreal C++ compile, editor validation, Android package, or device test.")
sys.exit(1 if errors else 0)
