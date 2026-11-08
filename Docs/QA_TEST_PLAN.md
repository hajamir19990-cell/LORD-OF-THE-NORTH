# QA plan — first playable Android slice

This checklist is the acceptance gate for the first Winterfell outskirts build. Do not mark a test as passed until it has been performed in Unreal Editor or on a physical Android device.

## Editor / source validation

- [ ] Open the project in the chosen Unreal Engine 5.x version.
- [ ] Generate project files and compile the Editor target.
- [ ] Confirm no missing modules, reflection errors or asset load errors.
- [ ] Confirm the default GameMode spawns LOTNCharacter and LOTNPlayerController.
- [ ] Confirm the native landscape HUD appears, or a custom Widget Blueprint is assigned to HUDWidgetClass.
- [ ] Confirm GameAndUI input mode lets HUD buttons receive touch/click input while character gameplay input remains available.
- [ ] Run the Unreal automation tests under LOTN.Inventory, LOTN.Quests, LOTN.WorldMap and LOTN.PlayerStats.

## Movement and landscape controls

- [ ] Android orientation remains landscape through launch, pause and resume.
- [ ] Left virtual joystick moves the character in camera-relative directions.
- [ ] Right virtual joystick controls camera yaw and pitch without rotating the movement input incorrectly.
- [ ] Interact, inventory, save and load HUD buttons respond to touch; UI interaction does not permanently block movement input.
- [ ] Sprint begins while the HUD SprintButton is held and ends on release.
- [ ] Stamina decreases during sprint and regenerates while walking.
- [ ] Sprint cannot continue indefinitely after stamina is exhausted.
- [ ] Health and stamina remain within configured bounds after damage, healing, consumption and recovery.
- [ ] The lower-left virtual joystick remains unobstructed by save/load controls; right-side action buttons remain reachable.
- [ ] Touch targets remain reachable and do not overlap quest/dialogue text on 16:9, 20:9 and 16:10 screens.

## Interaction and quest loop

- [ ] Camera trace selects only an interactable actor in range.
- [ ] Context prompt appears only while a valid interactable is in view.
- [ ] No target produces a readable feedback message.
- [ ] Guard offers the Winterfell_Delivery quest once.
- [ ] Objective marker advances the matching quest only.
- [ ] Objective cannot advance an inactive or completed quest.
- [ ] Returning to the guard after completing the objective grants rewards exactly once.
- [ ] HUD updates objective text, progress, gold and renown.

## Inventory and save/load

- [ ] AddItem creates and stacks item records up to their stack limits.
- [ ] Inventory rejects empty IDs and zero quantities, and reports any amount above its per-call acceptance cap.
- [ ] RemoveItem refuses removal when quantity is insufficient.
- [ ] Inventory panel opens, lists items and shows equipped state.
- [ ] Apparel/trinket items require a valid equipment slot.
- [ ] Equip/unequip buttons update the correct slot and refresh the list.
- [ ] Only one item per equipment slot remains equipped.
- [ ] Save persists transform, current location, stats, quests, inventory and equipment.
- [ ] Load restores the saved state in the same level.
- [ ] Missing save and incompatible-level cases show a failure message without corrupting current state.

## Visual and performance acceptance

- [ ] Confirm both gatehouse towers, the two outer watchtowers, connecting curtain walls and the approach road read clearly in landscape.
- [ ] Confirm the three road tiles meet without visible gaps and remain clear from PlayerStart to the gate.
- [ ] Confirm the temporary fog does not obscure NPC interaction range or exceed the mobile performance budget.
- [ ] Confirm NavMeshBoundsVolume covers the walkable approach and Recast navigation generates successfully.
- [ ] Confirm the temporary ground plane has been replaced with a sculpted terrain Landscape.
- [ ] Confirm settlement buildings and pine groves frame, rather than obstruct, the central approach.
- [ ] Replace all blockout geometry in the playable camera view with approved licensed environment assets.
- [ ] Check scale, pivots, normals, UVs, materials, collisions and LODs for every imported asset.
- [ ] Verify the scene has no visible floating pieces, broken seams or obvious texture stretching.
- [ ] Profile on the target Android device; aim for stable 30 FPS (33.3 ms frame time) before expanding the world.
- [ ] Check memory, shader cost, draw calls, thermal behavior and loading time.
- [ ] Verify save/load after force-closing and relaunching the app.

## Release gate

An APK is not release-ready until the Editor compile, Android package, install, launch, touch controls, quest loop, save/load and device performance checks all pass. GitHub static validation alone does not satisfy this gate.
