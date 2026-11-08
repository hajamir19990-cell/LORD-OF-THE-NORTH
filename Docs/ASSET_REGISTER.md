# Visual asset register — Winterfell landscape vertical slice

The art target is a grounded, weathered northern-fantasy environment in a wide landscape composition. Avoid primitive cube blockouts as final art. Asset use is conditional on importing, inspecting and profiling the actual files in Unreal Engine 5.

## Project-authored source meshes

| Source file | Intended use | Status |
|---|---|---|
| Content/SourceMeshes/Winterfell_Gatehouse.obj | Main twin-tower gatehouse landmark | Source OBJ committed; needs Unreal import and art polish |
| Content/SourceMeshes/Northern_Pine_Tree.obj | Repeated forest silhouette | Source OBJ committed; needs Unreal import, collision and LOD review |
| Content/SourceMeshes/Northern_Longhouse.obj | Settlement-edge building | Source OBJ committed; needs Unreal import and art polish |
| Content/SourceMeshes/Northern_Cobblestone_Road.obj | Approach road paving | Source OBJ committed; needs Unreal import and collision review |
| Content/SourceMeshes/Northern_Curtain_Wall.obj | Modular crenellated outer wall | Greybox OBJ committed; needs detail pass, UV/material and collision review |
| Content/SourceMeshes/Northern_Watchtower.obj | Outer defensive tower | Greybox OBJ committed; needs detail pass, UV/material and collision review |
| Content/SourceMeshes/Northern_Rolling_Terrain.obj | Gently rolling northern approach terrain | Source OBJ committed; needs Unreal import, collision and snow/rock material pass |
| Content/SourceMeshes/LOTN_Northern_Environment.mtl | Basic material definitions for source meshes | Starter material definitions; replace/upgrade with PBR textures |

These are editable interchange source meshes, not Unreal .uasset files and not yet integrated into a playable map. They are first-pass geometry, not final production art.

## Approved source library

| Source | Intended use | License | Import notes |
|---|---|---|---|
| Poly Haven | PBR stone, timber, snow/ground materials, environmental models and lighting references | CC0 | Verify each asset's maps, resolution and file size |

## Source page and license evidence

- Poly Haven asset library: https://polyhaven.com/
- Poly Haven asset license: https://polyhaven.com/license

CC0 means the publisher permits broad reuse, including commercial projects, without mandatory attribution. Re-check the individual source page before shipping.

## Landscape visual production plan

- Use the gatehouse as the focal landmark and frame it with the two settlement edges and pine groves.
- Keep the approach road clear and wide enough for the player camera and NPC interactions.
- Upgrade starter meshes with bevels, stone block variation, roof details, timber grain and proper UVs.
- Use verified PBR stone, timber, snow and ground materials from CC0 sources.
- Source a licensed player character and animation set separately; an unimported source URL is not an integrated asset.

## Acceptance gate before adding an asset to the playable map

1. Confirm the source file downloads and opens correctly.
2. Inspect scale, pivot, normals, UVs, material slots and texture maps.
3. Create or verify simple collision; avoid expensive per-poly collision on mobile.
4. Create appropriate LODs and test screen-space transitions.
5. Check texture memory, material count and draw calls.
6. Test on the target Android device before marking it mobile-ready.

## Current import status

The seven starter OBJ source meshes are committed, but no Unreal binary environment, character, animation, snow or foliage assets have been imported yet. Import and device profiling must happen in Unreal Editor; a source file alone is not evidence that it is integrated.
