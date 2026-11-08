# LORD OF THE NORTH — Gameplay Roadmap

## Vertical slice: Winterfell outskirts
The first playable slice is deliberately small and polished rather than nine unfinished regions.

### Player loop
1. Spawn at a northern road outside Winterfell.
2. Explore a snow-covered approach, forest edge, and gatehouse.
3. Meet a named guard NPC and open a dialogue interaction.
4. Accept a short delivery/reconnaissance quest.
5. Reach a marked point, interact, and return to the guard.
6. Receive gold and renown; save and reload progress.

### Core systems
- Third-person character movement and camera.
- Portrait HUD: health, stamina, gold, renown, quest objective.
- Touch movement joystick and right-side camera drag.
- Context interaction button.
- Dialogue data and quest state.
- Player stats component: health, stamina, gold, renown.
- Save data: location, transform, player stats, completed quest IDs.
- Location identity IDs for the nine-region world map.

### World locations
Winterfell, The Dreadfort, White Harbor, Bear Island, Karhold, Last Hearth, Deepwood Motte, Greywater Watch, and The Wall.

### Art direction
Grounded northern dark fantasy, realistic proportions, weathered stone, timber construction, snow accumulation, cold atmospheric depth, pine forests, restrained wind and snow particles. No primitive cube-only final environment.

### Asset acceptance checklist
- Source and redistribution license recorded.
- Correct scale and pivot.
- Clean topology and no broken/floating parts.
- PBR material maps where available.
- LODs and collision suitable for mobile.
- Texture dimensions and material count budgeted.
- Tested in Unreal Android preview and on a physical device.

## Milestones
- M0: project scaffold and C++ foundation.
- M1: compile in Unreal Editor; create portrait test map.
- M2: player mesh, animation, mobile camera and HUD.
- M3: first licensed/generated fortress and forest assets.
- M4: NPC dialogue, quest, rewards, save/load.
- M5: Android packaging, performance profiling, device QA.
- M6: expand to remaining regions and quest chains.

## Build truth
A GitHub source scaffold is not an Unreal build. Each milestone must be compiled and tested in Unreal Editor before it is marked complete. APK delivery requires Unreal Engine plus a configured Android toolchain.
