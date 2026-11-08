# LORD OF THE NORTH — Gameplay Roadmap

## Vertical slice: Winterfell outskirts

The first playable slice is deliberately small and polished rather than nine unfinished regions.

### Player loop
1. Spawn on a northern road outside Winterfell.
2. Explore a snow-covered approach, forest edge and gatehouse.
3. Meet a named guard NPC and open a dialogue interaction.
4. Accept a short inspection quest.
5. Reach the marked supply point and interact with it.
6. Return to the guard, turn in the quest and receive gold/renown.
7. Save, close and reload the game to verify persistence.

### Core systems in source
- Third-person character movement and spring-arm camera.
- Portrait HUD base class: health, stamina, gold, renown and quest progress.
- Touch movement through Unreal's virtual joystick; HUD interaction button.
- Context interaction through a line trace and interactable interface.
- NPC dialogue event, quest offer, objective progress, turn-in and rewards.
- Player stats component.
- Save data for player transform, stats, active quests and completed quest IDs.
- Data asset seeded with nine northern location definitions.

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
- M0: project scaffold and C++ source foundation — source files are present; compile is not yet verified.
- M1: compile in Unreal Editor and create portrait test map — pending editor/toolchain access.
- M2: player mesh, animation, mobile camera and HUD Blueprint — C++ HUD base exists; visual widget asset and character art remain pending.
- M3: first licensed fortress and forest assets — candidate sources recorded; binary assets are not imported yet.
- M4: NPC dialogue, quest, rewards and save/load — runtime C++ source is present; compile and in-editor behavior remain unverified.
- M5: Android packaging, performance profiling and device QA — pending Unreal/Android toolchain.
- M6: expand to remaining regions and quest chains.

## Build truth
A GitHub source scaffold is not an Unreal build. Each milestone must be compiled and tested in Unreal Editor before it is marked complete. APK delivery requires Unreal Engine plus a configured Android toolchain.
