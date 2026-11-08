# First map: VerticalPrototype

Create this map in Unreal Editor and save it as VerticalPrototype.umap.

## Required actors and setup

- PlayerStart on the northern road, facing toward the fortress approach.
- Walkable landscape or terrain mesh with a clear collision surface.
- A detailed, licensed exterior environment set: fortress silhouette, snow path, pine-tree clusters and a small settlement edge.
- One Blueprint child of LOTNNPC configured to offer Winterfell_Delivery.
- One Blueprint child of LOTNQuestObjective using the same QuestId.
- A directional light and sky/atmosphere setup suitable for mobile; bake/static-light where practical.
- LOTNGameMode as the map's GameMode override unless a child GameMode is intentionally used.

## Acceptance

- No final cube-only environment.
- The PlayerStart is reachable and the character does not spawn inside geometry.
- The camera has a clear portrait composition and does not clip through the gatehouse.
- The NPC and objective actors block the Visibility trace through their interaction bounds.
- All environment meshes have simple collision and mobile-appropriate LODs.
- Test the complete quest, inventory, sprint and save/load loop before expanding the map.

This file is an editor setup specification, not a generated Unreal map asset. The actual .umap must be created and saved by Unreal Editor.
