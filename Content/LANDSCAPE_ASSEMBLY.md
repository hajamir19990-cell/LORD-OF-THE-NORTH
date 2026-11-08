# Landscape environment assembly

## Fast editor setup

1. Open the project in Unreal Engine 5 and enable **Python Editor Script Plugin** (restart the editor if requested).
2. Open the Python console or execute the script through the editor's Python execution command.
3. Run `Scripts/create_winterfell_landscape.py` from the project root.
4. Open `/Game/Maps/WinterfellApproach` and inspect the imported meshes and actor placement.
5. Replace the temporary ground plane with a sculpted Landscape; add collision, material layers, foliage and a proper sky.
6. Create Blueprint children of LOTNNPC and LOTNQuestObjective and place them at the coordinates in MAPS_README.md.
7. Set the map's GameMode override to LOTNGameMode, save, then run the QA checklist.

The script imports the four OBJ sources into `/Game/Environment/North/SourceMeshes`, places the gatehouse, road, two longhouses, ten pine trees and PlayerStart, adds a temporary directional light, and saves the new level.

## Important limitations

- This is a greybox assembly script. The OBJ meshes are first-pass geometry and need art polish.
- The ground plane is explicitly temporary; it is not a finished Landscape.
- The script does not generate a final .umap outside Unreal Editor and does not create NPC/quest Blueprints.
- Verify imported mesh pivots, material assignments, collision, normals and scale in the editor.
- No compile, Android package or device performance result is implied by running the script.
