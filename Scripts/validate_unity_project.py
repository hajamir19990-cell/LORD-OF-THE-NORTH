#!/usr/bin/env python3
"""Static validation for the LORD OF THE NORTH Unity migration.

This checks repository structure and source OBJ integrity only. It does not
compile C#, import assets in Unity, create .unity scenes, or produce an APK.
"""
from pathlib import Path
import json
import math
import sys

ROOT = Path(__file__).resolve().parents[1]
PROJECT = ROOT / "UnityProject"
MESH_DIR = PROJECT / "Assets" / "Art" / "SourceOBJ"
EXPECTED_MESHES = {
    "Northern_Cobblestone_Road.obj", "Northern_Curtain_Wall.obj",
    "Northern_Handcart.obj", "Northern_Longhouse.obj",
    "Northern_Palisade_Fence.obj", "Northern_Pine_Tree.obj",
    "Northern_Rock_Cluster.obj", "Northern_Rolling_Terrain.obj",
    "Northern_Snowdrift.obj", "Northern_Stone_Firepit.obj",
    "Northern_Supply_Crate.obj", "Northern_Watchtower.obj",
    "Northern_Wolf_Banner.obj", "Northern_Woodpile.obj",
    "Winterfell_Gatehouse.obj",
}
REQUIRED_FILES = [
    PROJECT / "ProjectSettings" / "ProjectVersion.txt",
    PROJECT / "Packages" / "manifest.json",
    PROJECT / "Assets" / "Editor" / "LOTNProjectBootstrap.cs",
    PROJECT / "Assets" / "Editor" / "LOTNBuildPipeline.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNPlayerController.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNCameraFollow.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNPlayerStats.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNGameSession.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNQuestManager.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNInteractable.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNWorldMap.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNRegionData.cs",
    PROJECT / "Assets" / "Scripts" / "LOTNTouchInput.cs",
]

def validate_obj(path: Path) -> tuple[int, int]:
    vertices = 0
    faces = 0
    errors = []
    for line_no, line in enumerate(path.read_text(encoding="utf-8", errors="strict").splitlines(), 1):
        parts = line.strip().split()
        if not parts or parts[0].startswith("#"):
            continue
        if parts[0] == "v":
            if len(parts) < 4:
                errors.append(f"line {line_no}: malformed vertex")
                continue
            try:
                values = [float(v) for v in parts[1:4]]
                if not all(math.isfinite(v) for v in values):
                    errors.append(f"line {line_no}: non-finite vertex")
                vertices += 1
            except ValueError:
                errors.append(f"line {line_no}: invalid vertex number")
        elif parts[0] == "f":
            if len(parts) < 4:
                errors.append(f"line {line_no}: face has fewer than 3 vertices")
                continue
            for token in parts[1:]:
                try:
                    raw_index = int(token.split("/")[0])
                    index = raw_index if raw_index > 0 else vertices + raw_index + 1
                    if index < 1 or index > vertices:
                        errors.append(f"line {line_no}: vertex index {raw_index} out of range")
                except (ValueError, IndexError):
                    errors.append(f"line {line_no}: invalid face index {token}")
            faces += 1
    if vertices == 0 or faces == 0:
        errors.append("mesh contains no vertices or faces")
    if errors:
        raise ValueError("; ".join(errors[:8]))
    return vertices, faces

def main() -> int:
    failures = []
    for path in REQUIRED_FILES:
        if not path.is_file():
            failures.append(f"missing required file: {path.relative_to(ROOT)}")

    version_file = PROJECT / "ProjectSettings" / "ProjectVersion.txt"
    if version_file.exists():
        version = version_file.read_text(encoding="utf-8")
        if "6000.3." not in version:
            failures.append("ProjectVersion.txt must pin Unity 6.3 LTS (6000.3.x)")

    manifest_file = PROJECT / "Packages" / "manifest.json"
    if manifest_file.exists():
        try:
            manifest = json.loads(manifest_file.read_text(encoding="utf-8"))
            deps = manifest.get("dependencies", {})
            for package in ("com.unity.render-pipelines.universal", "com.unity.ugui"):
                if package not in deps:
                    failures.append(f"manifest missing package: {package}")
        except json.JSONDecodeError as exc:
            failures.append(f"invalid manifest.json: {exc}")

    present = {p.name for p in MESH_DIR.glob("*.obj")} if MESH_DIR.exists() else set()
    for missing in sorted(EXPECTED_MESHES - present):
        failures.append(f"missing OBJ source mesh: {missing}")

    valid_meshes = 0
    total_vertices = 0
    total_faces = 0
    for path in sorted(MESH_DIR.glob("*.obj")) if MESH_DIR.exists() else []:
        try:
            vertices, faces = validate_obj(path)
            valid_meshes += 1
            total_vertices += vertices
            total_faces += faces
        except (OSError, ValueError) as exc:
            failures.append(f"invalid {path.name}: {exc}")

    if failures:
        print("Unity migration validation: FAILED")
        for failure in failures:
            print(" - " + failure)
        return 1

    print("Unity migration validation: PASSED")
    print(f" - Unity editor: {(version_file.read_text().splitlines()[0]).strip()}")
    print(f" - source meshes: {valid_meshes} OBJ files")
    print(f" - geometry: {total_vertices} vertices / {total_faces} faces")
    print(" - URP, UGUI, gameplay scripts and Editor build pipeline: present")
    print(" - NOTE: no Unity Editor compile, scene generation or APK build is implied.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
