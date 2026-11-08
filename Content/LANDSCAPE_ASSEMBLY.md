# Landscape environment assembly

## Fast editor setup

1. Open the project in Unreal Engine 5; PythonScriptPlugin and EditorScriptingUtilities are enabled in the project file. Restart the editor if Unreal prompts you.
2. Open the Python console or execute the script through the editor's Python execution command. The assembly uses LevelEditorSubsystem and EditorActorSubsystem rather than the deprecated EditorLevelLibrary.
3. Run `Scripts/create_winterfell_landscape.py` from the project root.
4. Open `/Game/Maps/WinterfellApproach`, then run `Scripts/validate_winterfell_level.py` from the Unreal Editor Python console to check imported assets, expected actor counts and the 24-instance HISM forest. Inspect mesh scale, wall alignment, road continuity, fog and actor placement.
5. Inspect terrain collision, generated lightmap UV channels and imported MTL material slots; replace flat starter colors with optimized snow/rock PBR textures, add foliage and a mobile-appropriate sky, then bake/static-light the scene. Consider converting the static terrain mesh to a sculpted Landscape for layered materials.
6. The assembly script places native LOTNNPC and LOTNQuestObjective actors for the first quest loop; replace their temporary humanoid primitive visuals and greybox crate with approved character and prop assets after import.
7. Set the map's GameMode override to LOTNGameMode, save, then run the QA checklist.

The script imports the fifteen OBJ sources into `/Game/Environment/North/SourceMeshes`, verifies that every import produced an asset, and places the gatehouse, two crenellated wall segments, two watchtowers, three adjoining road tiles, three longhouses, four terrain-snapped palisade fence segments, eight terrain-snapped snow-capped rock clusters, one terrain-snapped stone firepit, two stacked firewood props, one handcart with barrel, six roadside snow berms, two gatehouse wolf banners and twenty-four terrain-snapped layered pine instances batched into one HISM actor, a rolling terrain surface and PlayerStart. Decorative trees, rocks, firepit, woodpiles, handcart, snow berms and banners are explicitly non-blocking; terrain, road, walls, towers, longhouses and palisades retain prototype blocking collision. It also spawns a quest-giving northern guard, a scout, and a matching supply-marker objective using the native C++ gameplay classes. Temporary engine primitive markers stand in for final character/prop art. A navigation bounds volume, directional light and temporary exponential-height fog are added before the new level is saved.

## Important limitations

- This is an editor assembly script using detailed first-pass OBJ source geometry. The gatehouse, curtain wall, watchtower and longhouse have additional architectural detail; verify the imported silhouettes, shading and materials in-editor before production sign-off.
- The rolling terrain includes a flattened foundation platform beneath the gatehouse, curtain walls and watchtowers. The assembly script samples the terrain grid and snaps roads, settlement buildings, trees, rocks, fences, firepit, NPCs, quest objective and PlayerStart to the terrain or road surface. The rolling terrain, road and structural meshes use complex-as-simple collision in this prototype. Decorative trees, rocks and the firepit are non-blocking to prevent snagging and reduce mobile collision work. Verify collision cooking and profile its cost on target phones; replace structural collision with simplified UCX meshes before release. The terrain is a first-pass static mesh, not a sculpted Unreal Landscape.
- Curtain wall and watchtower OBJ files now include additional stone-course and corner detailing, but remain source geometry requiring editor inspection, UV/material polish and mobile profiling.
- The navigation volume is only a starting bound; verify Recast generation and walkable slopes in the editor.
- The script adds a fog actor but leaves its final density/material/lighting tuning to editor review.
- The script must run after the C++ Editor target is compiled because it loads the native LOTNNPC and LOTNQuestObjective classes. It does not generate a .umap outside Unreal Editor.
- The assembly enables Unreal's generated lightmap UV setting on all fifteen imported static meshes before placement. Verify that the generated channel is valid and non-overlapping in the Static Mesh Editor; this setting is not proof that a lighting build has succeeded.
- Verify imported mesh pivots, material assignments, collision, normals and scale in the editor.
- No compile, Android package or device performance result is implied by running the script.


## Terrain and mobile atmosphere pass

The rolling terrain OBJ now has broad, low-frequency hills (roughly 5–6 m peak-to-peak), with the castle foundation, central road corridor and settlement building pads kept level. Terrain faces use the existing snow material on gentle ground and stone-light/stone-dark slots on steeper slopes to break up the all-white silhouette. The assembly adds an outer tree line for a more enclosed northern-forest horizon. The shared pine source mesh uses a tapered trunk, six overlapping lobed bough tiers and snow-capped tips, improving the repeated forest silhouette without adding texture dependencies.

The directional sun is configured as a static light at a 72,000-lux baseline. The height fog is deliberately non-volumetric, with restrained density and a 2,200 cm start distance so the road, guard and objective remain readable. Build static lighting in Unreal Editor, inspect the result on the target mobile renderer, and reduce shadow distance/foliage density if frame time exceeds budget. These are configuration targets, not a verified lighting bake or device benchmark.


## Architecture and settlement art pass

The gatehouse source mesh includes modeled open oak gate leaves, iron bands and layered masonry accents. Curtain walls have additional ashlar courses, buttresses and coping; watchtowers have corner quoins and parapet detailing; longhouses have timber framing, layered roof shingles and a chimney. The settlement dressing adds paired firewood stacks, a detailed handcart, six roadside snow berms and two red-and-gold wolf standards. All of these source assets are imported by the assembly script, receive lightmap-UV generation preparation and are placed in the landscape; editor import, visual inspection, lighting build and Android profiling are still required.


## Mobile foliage batching

The 24 pine transforms are added to one ALOTNFoliageCluster actor backed by UHierarchicalInstancedStaticMeshComponent. The component has collision disabled, does not affect navigation and uses conservative instance distance-culling values. Verify the generated cluster bounds, LOD behavior, shadows and culling transitions on target Android hardware.
