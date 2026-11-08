# Visual asset register — Winterfell vertical slice

The art target is a grounded, weathered northern-fantasy environment. Avoid primitive cube blockouts as final art. Asset use is conditional on importing, inspecting and profiling the actual files in Unreal Engine 5.

## Candidate assets

| Asset | Intended use | Format / size | License | Import notes |
|---|---|---|---|---|
| Innerscene — Castle with keep | Distant fortress landmark and exterior gate silhouette for the first approach | GLB, 17.1 MB | CC0 | Real-world scale; approximately 49.3 × 49.7 × 29.1 m. The published model has no interior rooms, so do not use it as an explorable interior without additional assets. Inspect mesh density, materials, collision and LODs after import. |
| Quaternius — Medieval Village MegaKit | Modular timber-and-stone village buildings and exterior set dressing | FBX / OBJ / glTF; Standard archive 153 MB | CC0 | 300+ modular pieces. The downloadable Source project lists Unreal Engine 4.27.2, so migrate/import the actual assets into the selected UE5 version and validate materials/collisions rather than assuming the source project opens unchanged. |
| Poly Haven | Photorealistic stone, wood, snow/ground PBR materials, environmental models and lighting references | Per-asset formats and resolutions | CC0 | Verify each asset's maps, resolution and file size. Use mobile-appropriate texture sizes and baked/static lighting. |

## Source pages and license evidence

- Castle with keep: https://www.innerscene.com/tools/library/3d-parts/castle-with-keep-1a75a9e1
- Quaternius Medieval Village MegaKit: https://quaternius.itch.io/medieval-village-megakit
- Poly Haven asset license: https://polyhaven.com/license

CC0 means the publisher permits broad reuse, including commercial projects, without mandatory attribution. Keep this register and re-check the individual source page before shipping.

## Acceptance gate before adding an asset to the playable map

1. Confirm the source file downloads and opens correctly.
2. Inspect scale, pivot, normals, UVs, material slots and texture maps.
3. Create or verify simple collision; avoid expensive per-poly collision on mobile.
4. Create appropriate LODs and test screen-space transitions.
5. Check texture memory, material count and draw calls.
6. Test on the target Android device before marking the asset mobile-ready.

## Current import status

These are vetted candidate sources, not imported project assets. The repository currently contains no binary castle, village, character, animation, snow or foliage assets. Import and device profiling must happen in Unreal Editor; a source URL alone is not evidence that the asset is integrated.
