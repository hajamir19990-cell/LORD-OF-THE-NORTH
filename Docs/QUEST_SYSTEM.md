# Quest, interaction and player progression systems

## Runtime classes

- ALOTNCharacter owns player stats, quests, inventory, interaction and the current location ID.
- ULOTNInteractionComponent traces from the active player-camera view and invokes ILOTNInteractable.
- ALOTNNPC implements dialogue and can offer a quest. Interacting again after the objective is complete turns it in and grants rewards.
- ALOTNQuestObjective is an interactable world marker. Assign its QuestId to match the NPC's quest ID; interacting advances progress.
- ULOTNQuestComponent stores active/completed quests, objective progress and reward values.
- ULOTNInventoryComponent manages stackable items, apparel/trinket equipment slots and inventory change events.
- ULOTNHUDWidget receives interaction messages automatically and refreshes stats, inventory count and quest progress from component events.
- ULOTNSaveSubsystem saves player transform, location ID, stats, inventory, equipment and full active/completed quest state to the LOTN_Save_01 slot.
- ULOTNWorldMapData contains nine location records, renown unlock checks and a travel-readiness check. A location is not travel-ready until its real level name is assigned.
- Sprint is stamina-limited. The character drains stamina while sprinting and regenerates it while walking; the portrait HUD uses a hold button so sprint stops when the player releases it.

## First quest setup in the editor

1. Create a Blueprint based on LOTNNPC.
2. Enable bOffersQuest, set QuestId to Winterfell_Delivery, configure the title, objective and rewards.
3. Create a Blueprint based on ALOTNQuestObjective.
4. Set the objective's QuestId to Winterfell_Delivery, assign a visible static mesh, and place it along the northern road.
5. Create a Widget Blueprint based on LOTNHUDWidget. Add optional named widgets: HealthBar, StaminaBar, GoldText, RenownText, InventoryCountText, QuestTitleText, QuestObjectiveText, QuestProgressText, DialogueText, InteractButton, SprintButton, SaveButton and LoadButton. Use a portrait-safe layout with large touch targets.
6. Configure SprintButton as a press-and-hold control; the widget binds OnPressed and OnReleased to sprint start/stop.
7. Create a Player Controller Blueprint based on LOTNPlayerController and set HUDWidgetClass to the HUD Widget Blueprint.
8. Create a GameMode Blueprint based on LOTNGameMode only if you need to override defaults; otherwise the C++ GameMode already selects the C++ player and controller classes.
9. In the level, assign the NPC and objective meshes/materials. The dialogue message is forwarded through the interaction component to the HUD automatically.
10. Add a LOTNWorldMapData asset and review its nine location entries. Assign actual level names only after those maps exist.

The objective actor uses a query-only box that blocks visibility traces; its visual mesh has collision disabled. Assign a suitable mesh and test the camera trace in the editor.

## Inventory and equipment setup

Create inventory item data through the InventoryComponent AddItem function. Stack size is bounded per call; apparel and trinkets require an equipment slot before they can be equipped. Equipment state is stored by item ID and saved with the inventory. The current equipment layer tracks state only; attaching visual meshes to a character is a separate art/animation task.

## Save/load boundaries

Active quest records and inventory/equipment state are saved in full. Loading currently restores into the same level only; cross-level travel and world streaming are future work. Save/load buttons show success or failure text in the HUD.
