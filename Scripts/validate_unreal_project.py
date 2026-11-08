#!/usr/bin/env python3
"""Repository structure checks for LOTN's Unreal source scaffold.

This script deliberately does not claim to compile Unreal C++ or package Android.
Those checks require an installed Unreal Engine and Android toolchain.
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
errors: list[str] = []
checks: list[str] = []

def require(condition: bool, message: str) -> None:
    if condition:
        checks.append(message)
    else:
        errors.append(message)

project_path = ROOT / "LORD_OF_THE_NORTH.uproject"
try:
    project = json.loads(project_path.read_text(encoding="utf-8"))
    require(project.get("FileVersion") == 3, "uproject JSON parses and uses FileVersion 3")
    modules = project.get("Modules", [])
    require(any(m.get("Name") == "LordOfTheNorth" and m.get("Type") == "Runtime" for m in modules),
            "runtime module is declared in uproject")
except Exception as exc:
    errors.append(f"uproject JSON is invalid: {exc}")

build_cs = ROOT / "Source/LordOfTheNorth/LordOfTheNorth.Build.cs"
require(build_cs.is_file(), "module Build.cs exists")
if build_cs.is_file():
    text = build_cs.read_text(encoding="utf-8")
    require('"Core"' in text and '"CoreUObject"' in text and '"Engine"' in text,
            "module declares core Unreal dependencies")

target_files = [
    ROOT / "Source/LordOfTheNorth.Target.cs",
    ROOT / "Source/LordOfTheNorthEditor.Target.cs",
]
for path in target_files:
    require(path.is_file(), f"{path.name} exists")

public_dir = ROOT / "Source/LordOfTheNorth/Public"
private_dir = ROOT / "Source/LordOfTheNorth/Private"
headers = sorted(public_dir.glob("*.h"))
sources = sorted(private_dir.glob("*.cpp"))
require(bool(headers), "public C++ headers are present")
require(bool(sources), "private C++ sources are present")

for header in headers:
    content = header.read_text(encoding="utf-8")
    generated = re.findall(r'^#include\s+"[^"]+\.generated\.h"\s*$', content, re.M)
    if "UCLASS(" in content or "USTRUCT(" in content or "UINTERFACE(" in content:
        require(len(generated) == 1, f"{header.name}: exactly one generated header include")
        if generated:
            last_include = [line for line in content.splitlines() if line.startswith("#include")][-1]
            require(last_include == generated[0], f"{header.name}: generated header is the final include")

for source in sources:
    content = source.read_text(encoding="utf-8")
    expected_header = source.stem + ".h"
    require(f'#include "{expected_header}"' in content, f"{source.name}: includes its matching header")

engine_config = ROOT / "Config/DefaultEngine.ini"
config_text = engine_config.read_text(encoding="utf-8") if engine_config.exists() else ""
require("Orientation=Portrait" in config_text, "Android orientation is portrait")
require("r.MobileHDR=False" in config_text, "mobile HDR is disabled in baseline config")

input_config = ROOT / "Config/DefaultInput.ini"
input_text = input_config.read_text(encoding="utf-8") if input_config.exists() else ""
require('ActionName="Interact"' in input_text, "interact action is mapped")

for item in checks:
    print(f"PASS: {item}")
for item in errors:
    print(f"FAIL: {item}", file=sys.stderr)

print(f"\nStatic structure checks: {len(checks)} passed, {len(errors)} failed.")
print("NOTE: This is not an Unreal C++ compile, editor validation, Android package, or device test.")
sys.exit(1 if errors else 0)
