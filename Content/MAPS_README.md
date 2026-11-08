# First playable map: WinterfellApproach

Target: Unreal Engine 5, Android landscape (16:9 reference composition; keep important gameplay readable at 20:9 and 16:10).

This document is the level assembly specification. The actual binary .umap must be created and saved by Unreal Editor.

## Art direction

A grounded northern-fantasy frontier: weathered grey granite, dark oak, iron fittings, snow-covered roofs, muddy snow paths, dense pine silhouettes and cold overcast light. The gatehouse is the visual landmark. Avoid a cube-only final environment.

## Greybox dimensions and layout

Use centimeters; world Z is vertical. The gatehouse front faces toward negative Y, with the approach road coming from negative Y.

| Area | Suggested center (X,Y) | Footprint / notes |
|---|---:|---|
| Gatehouse | (0, 0) | Imported gatehouse source mesh; focal landmark |
| Curtain walls | (+/- 1775, 0) | Modular crenellated wall segments connect gatehouse to outer watchtowers |
| Watchtowers | (+/- 3100, 0) | Square defensive towers at the ends of the first wall section |
| Player start | (0, -1700) | Facing +Y toward gate; keep clear of the road |
| Main approach | (0, -900) | Three adjoining road mesh tiles from player start to gate |
| West settlement edge | (-1250, -350) | Longhouse, storehouse, timber fence |
| East settlement edge | (1250, -250) | Two small buildings, stacked firewood, carts/props |
| West pine grove | (-1900, 250) | 5–8 trees, varied scale and spacing |
| East pine grove | (1900, 350) | 5–8 trees, preserve playable sightlines |
| Quest supply marker | (0, -430) | Native interactable objective beside the approach road |
| Quest giver NPC | (0, -1320) | Native interactable quest giver near PlayerStart |
| Outer wall continuation | +/- 900, 0 | Extend wall silhouette beyond the gatehouse |

Keep the central approach readable from the player start. The gate should occupy the middle distance, not fill the whole screen. Put trees and buildings toward the side edges to frame the route without blocking the camera.

## Required actors and setup

- PlayerStart at (0, -1700, 116), yaw +90 degrees, facing the gate; the road source mesh rises to approximately 16 cm.
- Walkable rolling terrain source mesh; verify imported collision and walkability in Unreal Editor before treating it as production-ready.
- Imported environment meshes from Content/SourceMeshes: gatehouse, pine, longhouse, cobblestone road, curtain wall, watchtower, rolling terrain, supply crate, palisade fence, snow-capped rock cluster and stone firepit.
- Native LOTNNPC quest giver named Northern Guard, configured to offer Winterfell_Delivery; temporary pillar visual must be replaced with a skeletal character asset.
- Exponential height fog and a directional daylight baseline; tune conservatively for mobile.
- NavMeshBoundsVolume covering the first approach slice; verify navigation generation in Editor.
- Native LOTNQuestObjective supply marker using the same QuestId; timber-and-iron crate source mesh is assigned as a greybox prop; replace it with a final art asset after import.
- Directional light and conservative fog setup suitable for mobile; bake/static lighting where practical.
- LOTNGameMode as the map's GameMode override unless a child GameMode is intentionally used.

After the assembly script saves the binary map, the project configuration points both the Editor startup map and packaged game default map at `/Game/Maps/WinterfellApproach`.

## Landscape composition and camera

- Android orientation must be Landscape.
- Keep a wide horizon and show both towers plus the full approach road in the initial camera view.
- Use a third-person camera distance that keeps the player in the lower central third and the gatehouse in the upper-middle distance.
- Check camera collision around walls, trees and buildings.
- Validate on 16:9, 20:9 and 16:10 aspect ratios; do not anchor essential level objectives to screen edges.

## Mobile performance targets

- Prefer baked/static lighting; no Lumen or Nanite dependency for the Android target.
- Use simple collision, LODs, instancing for repeated trees and props, and texture sizes appropriate to device memory.
- Keep foliage density and translucent snow/fog effects conservative.
- Profile draw calls, shader complexity, texture memory and frame pacing on a real Android device.

## Acceptance

- PlayerStart is reachable and the character does not spawn inside geometry.
- Both towers and the main road read clearly in landscape.
- NPC and objective actors block the Visibility trace through their interaction bounds.
- All environment meshes have simple collision and mobile-appropriate LODs.
- Test movement, camera, interaction, quest, inventory, sprint and save/load before expanding the map.


## Additional greybox placement

- Four 640 cm palisade modules at X = +/-850 and Y = -1200/-600 frame the road with a clear center lane.
- Eight snow-capped rock clusters are distributed on the outer terrain margins, away from the central path.
- A stone firepit sits near the east settlement building at approximately (1600, -1000).
- NPC capsule roots and the quest crate are placed at Z=16 to align with the road surface; confirm the actual imported collision and terrain intersection in Editor.
