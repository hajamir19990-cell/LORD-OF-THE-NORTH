#include "LOTNWorldMapData.h"

ULOTNWorldMapData::ULOTNWorldMapData()
{
    const auto AddLocation = [this](FName Id, const TCHAR* Name, const TCHAR* Description, int32 Renown, bool bStartingLocation)
    {
        FLOTNWorldLocation Entry;
        Entry.LocationId = Id;
        Entry.DisplayName = FText::FromString(Name);
        Entry.Description = FText::FromString(Description);
        Entry.LevelName = Id;
        Entry.bUnlockedByDefault = bStartingLocation;
        Entry.RequiredRenown = Renown;
        Locations.Add(Entry);
    };

    AddLocation(TEXT("Winterfell"), TEXT("Winterfell"), TEXT("The ancestral seat of House Stark, surrounded by old stone walls and northern woods."), 0, true);
    AddLocation(TEXT("TheDreadfort"), TEXT("The Dreadfort"), TEXT("A grim stronghold in the eastern lands."), 30, false);
    AddLocation(TEXT("WhiteHarbor"), TEXT("White Harbor"), TEXT("The North's great port, alive with ships, merchants and sailors."), 15, false);
    AddLocation(TEXT("BearIsland"), TEXT("Bear Island"), TEXT("A rugged island of cold shores and hardy people."), 45, false);
    AddLocation(TEXT("Karhold"), TEXT("Karhold"), TEXT("An ancient northern fortress beneath a hard sky."), 25, false);
    AddLocation(TEXT("LastHearth"), TEXT("Last Hearth"), TEXT("A remote keep guarding the northern roads."), 20, false);
    AddLocation(TEXT("DeepwoodMotte"), TEXT("Deepwood Motte"), TEXT("A timber-and-stone holdfast deep within the wolfswood."), 35, false);
    AddLocation(TEXT("GreywaterWatch"), TEXT("Greywater Watch"), TEXT("A hidden stronghold among the shifting waters of the Neck."), 60, false);
    AddLocation(TEXT("TheWall"), TEXT("The Wall"), TEXT("The immense frontier of ice at the edge of the known world."), 80, false);
}
