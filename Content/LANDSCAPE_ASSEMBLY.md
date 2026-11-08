# Landscape environment assembly

## Fast editor setup

1. Open the project in Unreal Engine 5; PythonScriptPlugin and EditorScriptingUtilities are enabled in the project file. Restart the editor if Unreal prompts you.
2. Open the Python console or execute the script through the editor's Python execution command.
3. Run `Scripts/create_winterfell_landscape.py` from the project root.
4. Open `/Game/Maps/WinterfellApproach` and inspect imported mesh scale, wall alignment, road continuity, fog and actor placement.
5. Inspect terrain collision; add snow/rock materials, foliage, a mobile-appropriate sky and baked/static lighting. Consider converting the static terrain mesh to a sculpted Landscape for layered materials.
6. The assembly script places native LOTNNPC and LOTNQuestObjective actors for the first quest loop; replace their temporary pillar marker and greybox crate with approved character and prop assets after import.
7. Set the map's GameMode override to LOTNGameMode, save, then run the QA checklist.

The script imports the eleven OBJ sources into `/Game/Environment/North/SourceMeshes`, places the gatehouse, two crenellated wall segments, two watchtowers, three adjoining road tiles, three longhouses, four palisade fence segments, eight snow-capped rock clusters, a stone firepit, twelve pine trees, a rolling terrain surface and PlayerStart. It also spawns a quest-giving northern guard, a scout, and a matching supply-marker objective using the native C++ gameplay classes. Temporary engine primitive markers stand in for final character/prop art. A navigation bounds volume, directional light and temporary exponential-height fog are added before the new level is saved.

## Important limitations

- This is a greybox assembly script. The OBJ meshes are first-pass geometry and need art polish.
- The rolling terrain and low-poly environment meshes use complex-as-simple collision in this prototype so the player cannot walk through the world. Verify collision cooking and profile its cost on target phones; replace with simplified UCX collision before release. The terrain is a first-pass static mesh, not a sculpted Unreal Landscape.
- Curtain wall and watchtower OBJ files are modular greybox geometry, not detailed production-ready castle art.
- The navigation volume is only a starting bound; verify Recast generation and walkable slopes in the editor.
- The script adds a fog actor but leaves its final density/material/lighting tuning to editor review.
- The script must run after the C++ Editor target is compiled because it loads the native LOTNNPC and LOTNQuestObjective classes. It does not generate a .umap outside Unreal Editor.
- Verify imported mesh pivots, material assignments, collision, normals and scale in the editor.
- No compile, Android package or device performance result is implied by running the script.
