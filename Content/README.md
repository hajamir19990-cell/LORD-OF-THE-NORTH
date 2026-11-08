# Content

Unreal Editor will generate and manage binary assets here. The first editor milestone should create:

- Maps/WinterfellApproach — landscape first playable map, with PlayerStart, terrain and fortress approach
- Characters/Player — licensed player skeletal mesh, skeleton, animation blueprint and materials
- Environment/North — detailed stone fortress exterior, timber settlement, pine forest, snow and ground materials
- Environment/Props — licensed non-combat world props and quest markers
- UI/HUD — landscape-native HUD layout; the C++ native HUD is the functional baseline
- Audio/Ambience — wind, snowfall, footsteps and northern settlement ambience

Source OBJ meshes are in SourceMeshes. Import them into Unreal Editor, inspect pivots/materials/collision, then create engine-native static mesh assets. Do not commit Unreal-generated cache folders or derived build output. Add only source assets with confirmed redistribution licenses. See MAPS_README.md for the level assembly checklist and Docs/ASSET_REGISTER.md for candidate sources and mobile import checks.
