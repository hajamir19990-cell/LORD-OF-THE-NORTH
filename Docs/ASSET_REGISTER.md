# Visual asset register — Winterfell vertical slice

The art target is a grounded, weathered northern-fantasy environment. Avoid primitive cube blockouts as final art. Asset use is conditional on importing, inspecting and profiling the actual files in Unreal Engine 5.

## Approved source library

| Source | Intended use | License | Import notes |
|---|---|---|---|
| Poly Haven | PBR stone, timber, snow/ground materials, environmental models and lighting references | CC0 | Verify each asset's maps, resolution and file size. Use mobile-appropriate texture sizes and baked/static lighting. |

## Source page and license evidence

- Poly Haven asset library: https://polyhaven.com/
- Poly Haven asset license: https://polyhaven.com/license

CC0 means the publisher permits broad reuse, including commercial projects, without mandatory attribution. Re-check the individual source page before shipping.

## Visual production plan

- Create an original northern stone-and-timber landmark with a readable silhouette and believable scale.
- Build the playable settlement edge from separately verified architectural pieces and custom meshes.
- Use high-quality PBR stone, timber, snow and ground materials from CC0 sources.
- Source a licensed player character and animation set separately; do not treat an unimported source URL as an integrated asset.

## Acceptance gate before adding an asset to the playable map

1. Confirm the source file downloads and opens correctly.
2. Inspect scale, pivot, normals, UVs, material slots and texture maps.
3. Create or verify simple collision; avoid expensive per-poly collision on mobile.
4. Create appropriate LODs and test screen-space transitions.
5. Check texture memory, material count and draw calls.
6. Test on the target Android device before marking the asset mobile-ready.

## Current import status

No binary environment, character, animation, snow or foliage assets have been imported yet. Import and device profiling must happen in Unreal Editor; a source URL alone is not evidence that the asset is integrated.
