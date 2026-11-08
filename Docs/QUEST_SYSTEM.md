# Quest and interaction system

## Runtime classes

- ALOTNCharacter owns player stats, quests and interaction components.
- ULOTNInteractionComponent performs a visibility-channel line trace from the character's view and invokes ILOTNInteractable.
- ALOTNNPC implements dialogue and can offer a quest. Interacting again after the objective is complete turns it in and grants rewards.
- ALOTNQuestObjective is an interactable world marker. Assign its QuestId to match the NPC's quest ID; interacting advances progress.
- ULOTNQuestComponent stores active/completed quests, progress and reward values.
- ULOTNSaveSubsystem saves player transform, stats and full active/completed quest state to the LOTN_Save_01 slot.

## First quest setup in the editor

1. Create a Blueprint based on LOTNNPC.
2. Enable bOffersQuest, set QuestId to Winterfell_Delivery, configure the title, objective and rewards.
3. Bind OnDialogueRequested to the dialogue widget.
4. Create a Blueprint based on ALOTNQuestObjective.
5. Set the objective's QuestId to Winterfell_Delivery, assign a visible static mesh, and place it along the northern road.
6. Add an interaction button to the portrait HUD and call InteractPressed on the player character.
7. Display QuestComponent.ActiveQuests and progress in the HUD.
8. Add save/load controls that call the Game Instance subsystem's SaveCurrentGame and LoadCurrentGame functions.

The objective actor uses a query-only box that blocks visibility traces; its visual mesh has collision disabled. Assign a suitable mesh and test the trace in the editor.

## Current boundaries

Dialogue is exposed through a Blueprint-assignable event; a finished dialogue panel is not yet included. Active quest records are saved in full. Loading currently restores into the same level only; cross-level travel and world streaming are future work.
