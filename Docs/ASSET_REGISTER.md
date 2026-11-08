# Visual asset register — Winterfell landscape vertical slice

The art target is a grounded, weathered northern-fantasy environment in a wide landscape composition. Avoid primitive cube blockouts as final art. Asset use is conditional on importing, inspecting and profiling the actual files in Unreal Engine 5.

## Project-authored source meshes

| Source file | Intended use | Status |
|---|---|---|
| Content/SourceMeshes/Winterfell_Gatehouse.obj | Main twin-tower gatehouse landmark | Detailed source OBJ with open oak gate leaves, iron bands and layered masonry accents; needs Unreal import and visual review |
| Content/SourceMeshes/Northern_Pine_Tree.obj | Repeated forest silhouette | Layered source OBJ committed: tapered trunk, six overlapping bough tiers and snow caps; assembled as 24 HISM instances with collision disabled; needs Unreal import, LOD and culling review |
| Content/SourceMeshes/Northern_Longhouse.obj | Settlement-edge building | Detailed source OBJ with timber framing, layered roof shingles and chimney; needs Unreal import and visual review |
| Content/SourceMeshes/Northern_Cobblestone_Road.obj | Approach road paving | Source OBJ committed; needs Unreal import and collision review |
| Content/SourceMeshes/Northern_Curtain_Wall.obj | Modular crenellated outer wall | Detailed source OBJ with ashlar courses, projecting buttresses and stone coping; needs Unreal import, UV/material and collision review |
| Content/SourceMeshes/Northern_Watchtower.obj | Outer defensive tower | Detailed source OBJ with corner quoins, stone parapet and architectural accents; needs Unreal import, UV/material and collision review |
| Content/SourceMeshes/Northern_Rolling_Terrain.obj | Gently rolling northern approach terrain | Source OBJ committed; needs Unreal import, collision and snow/rock material pass |
| Content/SourceMeshes/Northern_Supply_Crate.obj | Quest supply marker prop | Greybox OBJ committed; needs Unreal import and art polish |
| Content/SourceMeshes/Northern_Palisade_Fence.obj | Modular timber settlement boundary | Greybox OBJ committed; needs Unreal import, collision and art polish |
| Content/SourceMeshes/Northern_Rock_Cluster.obj | Snow-capped outer-terrain rock dressing | Greybox OBJ committed; needs Unreal import and collision review |
| Content/SourceMeshes/Northern_Stone_Firepit.obj | Settlement landmark prop | Greybox OBJ committed; needs Unreal import and collision review |
| Content/SourceMeshes/Northern_Woodpile.obj | Stacked winter firewood | Detailed source OBJ; non-blocking decorative prop |
| Content/SourceMeshes/Northern_Handcart.obj | Settlement supply cart | Detailed source OBJ with spoked wheels, axle, plank bed and barrel; non-blocking |
| Content/SourceMeshes/Northern_Snowdrift.obj | Roadside snow berms | Procedural low-poly snow mound; non-blocking |
| Content/SourceMeshes/Northern_Wolf_Banner.obj | Gatehouse heraldic standards | Wavy red cloth, gold wolf-head crest and iron crossbar; non-blocking |
| Content/SourceMeshes/LOTN_Northern_Environment.mtl | Basic material definitions for source meshes | Starter material definitions; replace/upgrade with PBR textures |

These are fifteen editable interchange source meshes (plus the shared MTL library), not Unreal .uasset files and not yet integrated into a playable map. The assembly script enables Unreal-generated lightmap UVs for every imported static mesh; verify the generated channels in the editor. The meshes remain first-pass geometry, not final production art.

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
- The player currently uses a six-part engine-primitive humanoid greybox; source and integrate a licensed skeletal character and animation set before release.

## Acceptance gate before adding an asset to the playable map

1. Confirm the source file downloads and opens correctly.
2. Inspect scale, pivot, normals, UVs, material slots and texture maps.
3. Create or verify simple collision; avoid expensive per-poly collision on mobile.
4. Create appropriate LODs and test screen-space transitions.
5. Check texture memory, material count and draw calls.
6. Test on the target Android device before marking it mobile-ready.

## Current import status

Fifteen starter OBJ source meshes are committed, including the layered pine, woodpile, handcart, snowdrift and heraldic banner updates, but no Unreal binary environment, character, animation, snow or foliage assets have been imported yet. Import and device profiling must happen in Unreal Editor; a source file alone is not evidence that it is integrated.


## Northern supply crate
- `Content/SourceMeshes/Northern_Supply_Crate.obj` — 216 vertices / 162 quad faces; separate timber planks, dark corner posts and iron straps. Uses the shared northern environment MTL. The Winterfell assembly script imports it and assigns it to the native quest objective marker when run inside Unreal Editor.
- This is a greybox prop mesh; verify normals, material slots, collision-off objective visual and mobile rendering in Unreal Editor.


## Settlement edge and ground dressing
- `Content/SourceMeshes/Northern_Palisade_Fence.obj` — modular timber palisade segment with pointed stakes and cross rails; used to frame both sides of the approach.
- `Content/SourceMeshes/Northern_Rock_Cluster.obj` — five low-poly stones with snow caps; placed along the outer terrain margins.
- `Content/SourceMeshes/Northern_Stone_Firepit.obj` — stone ring and crossed charred logs; settlement landmark near the east longhouse.
- These are greybox source meshes using the shared MTL palette. Verify normals, collision settings and mobile material appearance after import.


## Settlement edge and ground dressing
- `Content/SourceMeshes/Northern_Palisade_Fence.obj` — modular timber palisade segment with pointed stakes and cross rails; used to frame both sides of the approach.
- `Content/SourceMeshes/Northern_Rock_Cluster.obj` — five low-poly stones with snow caps; placed along the outer terrain margins.
- `Content/SourceMeshes/Northern_Stone_Firepit.obj` — stone ring and crossed charred logs; settlement landmark near the east longhouse.
- These are greybox source meshes using the shared MTL palette. Verify normals, collision settings and mobile material appearance after import.


## Architecture detail pass

The gatehouse, curtain wall, watchtower and longhouse source meshes have received a geometry detail pass. The gatehouse includes modeled gate leaves and masonry accents; the wall and tower have additional stone-course and corner detailing; the longhouse includes timber framing, roof shingle geometry and a chimney. These remain source OBJ meshes: verify shading, pivots, UVs, material assignment, and silhouette in Unreal Editor before approving them as final production art.
