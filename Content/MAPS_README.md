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
| Quest supply marker | (450, -850) | Visible from the road, reachable without obstruction |
| Quest giver NPC | (-420, -650) | Near the settlement edge, facing the approach |
| Outer wall continuation | +/- 900, 0 | Extend wall silhouette beyond the gatehouse |

Keep the central approach readable from the player start. The gate should occupy the middle distance, not fill the whole screen. Put trees and buildings toward the side edges to frame the route without blocking the camera.

## Required actors and setup

- PlayerStart at (0, -1700, 100), yaw +90 degrees, facing the gate.
- Walkable landscape or terrain mesh with collision; the assembly script currently creates a temporary plane only.
- Imported environment meshes from Content/SourceMeshes: Winterfell_Gatehouse.obj, Northern_Pine_Tree.obj, Northern_Longhouse.obj, Northern_Cobblestone_Road.obj, Northern_Curtain_Wall.obj and Northern_Watchtower.obj.
- One Blueprint child of LOTNNPC configured to offer Winterfell_Delivery.
- Exponential height fog and a directional daylight baseline; tune conservatively for mobile.
- NavMeshBoundsVolume covering the first approach slice; verify navigation generation in Editor.
- One Blueprint child of LOTNQuestObjective using the same QuestId.
- Directional light and conservative fog setup suitable for mobile; bake/static lighting where practical.
- LOTNGameMode as the map's GameMode override unless a child GameMode is intentionally used.

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

