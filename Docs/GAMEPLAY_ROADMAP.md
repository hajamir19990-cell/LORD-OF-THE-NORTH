# LORD OF THE NORTH — Gameplay Roadmap

## Vertical slice: Winterfell outskirts

The first playable slice is deliberately small and polished rather than nine unfinished regions.

### Player loop
1. Spawn at a PlayerStart on the northern road outside Winterfell.
2. Explore a snow-covered approach, forest edge and gatehouse.
3. Meet a northern guard and see a contextual interaction prompt.
4. Accept a short inspection quest.
5. Reach the marked supply point and interact with it.
6. Return to the guard, turn in the quest and receive gold/renown.
7. Collect and manage a few non-combat inventory items; equip apparel or a trinket.
8. Save, close and reload the game to verify persistence.

### Core systems in source
- Third-person character movement and spring-arm camera.
- Portrait-native HUD with health, stamina, gold, renown, inventory count, quest progress, dialogue and touch controls.
- Stamina-limited sprint with hold-to-run input and regeneration.
- Touch movement through Unreal's virtual joystick.
- Context interaction through a camera-view line trace and interactable interface.
- NPC dialogue event, quest offer, objective progress, turn-in and rewards.
- Player stats component.
- Stackable inventory and apparel/trinket equipment slots.
- Save data for player transform, location ID, stats, inventory, equipment, active quests and completed quest IDs.
- Data asset seeded with nine northern location definitions and renown unlock checks.
- Unreal automation tests for inventory stacking, equipment slots, quest progress and location unlock rules.

### World locations
Winterfell, The Dreadfort, White Harbor, Bear Island, Karhold, Last Hearth, Deepwood Motte, Greywater Watch and The Wall.

### Art direction
Grounded northern fantasy, realistic proportions, weathered stone, timber construction, snow accumulation, cold atmospheric depth, pine forests and restrained particles. No primitive cube-only final environment.

### Asset acceptance checklist
- Source and redistribution license recorded.
- Correct scale and pivot.
- Clean topology and no broken/floating parts.
- PBR material maps where available.
- LODs and collision suitable for mobile.
- Texture dimensions and material count budgeted.
- Tested in Unreal Android preview and on a physical device.

## Milestones
- M0: project scaffold and C++ source foundation — source files are present; Unreal compile is not yet verified.
- M1: compile in Unreal Editor and create portrait test map — pending editor/toolchain access.
- M2: player mesh, animation, mobile camera and HUD — C++ movement, sprint and native HUD are present; character art and map asset remain pending.
- M3: first licensed fortress, village and forest assets — candidate sources recorded; binary assets are not imported yet.
- M4: NPC dialogue, quest, inventory, equipment and save/load — runtime C++ source and automation tests are present; editor test execution remains pending.
- M5: Android packaging, performance profiling and device QA — a manual self-hosted BuildCookRun workflow exists; runner, Unreal toolchain, map and art assets are pending.
- M6: expand to remaining regions and quest chains.

## Build truth
A GitHub source scaffold is not an Unreal build. Static CI checks repository structure only. Each gameplay milestone must be compiled and tested in Unreal Editor before it is marked complete. APK delivery requires Unreal Engine, a configured Android toolchain, a real map and imported game assets.
