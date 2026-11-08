#include "LOTNWorldMapData.h"
#include <initializer_list>

ULOTNWorldMapData::ULOTNWorldMapData()
{
    const auto AddLocation = [this](
        FName Id,
        const TCHAR* Name,
        const TCHAR* Description,
        int32 Renown,
        bool bStartingLocation,
        const FVector2D& MapPosition,
        FName RegionType,
        std::initializer_list<FName> Connections)
    {
        FLOTNWorldLocation Entry;
        Entry.LocationId = Id;
        Entry.DisplayName = FText::FromString(Name);
        Entry.Description = FText::FromString(Description);
        Entry.LevelName = bStartingLocation ? FName(TEXT("WinterfellApproach")) : NAME_None;
        Entry.bUnlockedByDefault = bStartingLocation;
        Entry.RequiredRenown = Renown;
        Entry.MapPosition = MapPosition;
        Entry.RegionType = RegionType;
        Entry.ConnectedLocationIds = TArray<FName>(Connections);
        Locations.Add(Entry);
    };

    // Normalized map coordinates keep the complete northern route readable
    // across landscape screen sizes; links form a traversable regional network.
    AddLocation(TEXT("Winterfell"), TEXT("Winterfell"),
        TEXT("The ancestral seat of House Stark, surrounded by old stone walls and northern woods."),
        0, true, FVector2D(0.50f, 0.53f), TEXT("Castle"),
        { TEXT("TheDreadfort"), TEXT("WhiteHarbor"), TEXT("Karhold"),
          TEXT("LastHearth"), TEXT("DeepwoodMotte"), TEXT("GreywaterWatch"), TEXT("TheWall") });

    AddLocation(TEXT("TheDreadfort"), TEXT("The Dreadfort"),
        TEXT("A grim stronghold in the eastern lands."),
        30, false, FVector2D(0.82f, 0.57f), TEXT("Castle"),
        { TEXT("Winterfell"), TEXT("WhiteHarbor"), TEXT("Karhold") });

    AddLocation(TEXT("WhiteHarbor"), TEXT("White Harbor"),
        TEXT("The North's great port, alive with ships, merchants and sailors."),
        15, false, FVector2D(0.72f, 0.22f), TEXT("Port"),
        { TEXT("Winterfell"), TEXT("TheDreadfort"), TEXT("BearIsland") });

    AddLocation(TEXT("BearIsland"), TEXT("Bear Island"),
        TEXT("A rugged island of cold shores and hardy people."),
        45, false, FVector2D(0.14f, 0.20f), TEXT("Island"),
        { TEXT("WhiteHarbor"), TEXT("DeepwoodMotte") });

    AddLocation(TEXT("Karhold"), TEXT("Karhold"),
        TEXT("An ancient northern fortress beneath a hard sky."),
        25, false, FVector2D(0.84f, 0.34f), TEXT("Castle"),
        { TEXT("Winterfell"), TEXT("TheDreadfort"), TEXT("LastHearth"), TEXT("TheWall") });

    AddLocation(TEXT("LastHearth"), TEXT("Last Hearth"),
        TEXT("A remote keep guarding the northern roads."),
        20, false, FVector2D(0.66f, 0.78f), TEXT("Keep"),
        { TEXT("Winterfell"), TEXT("Karhold"), TEXT("TheWall") });

    AddLocation(TEXT("DeepwoodMotte"), TEXT("Deepwood Motte"),
        TEXT("A timber-and-stone holdfast deep within the wolfswood."),
        35, false, FVector2D(0.25f, 0.45f), TEXT("Woodland"),
        { TEXT("Winterfell"), TEXT("BearIsland"), TEXT("GreywaterWatch") });

    AddLocation(TEXT("GreywaterWatch"), TEXT("Greywater Watch"),
        TEXT("A hidden stronghold among the shifting waters of the Neck."),
        60, false, FVector2D(0.35f, 0.86f), TEXT("Marsh"),
        { TEXT("Winterfell"), TEXT("DeepwoodMotte"), TEXT("TheWall") });

    AddLocation(TEXT("TheWall"), TEXT("The Wall"),
        TEXT("The immense frontier of ice at the edge of the known world."),
        80, false, FVector2D(0.49f, 0.08f), TEXT("Frontier"),
        { TEXT("Winterfell"), TEXT("Karhold"), TEXT("LastHearth"), TEXT("GreywaterWatch") });
}

bool ULOTNWorldMapData::GetLocationById(FName LocationId, FLOTNWorldLocation& OutLocation) const
{
    const FLOTNWorldLocation* Found = Locations.FindByPredicate(
        [LocationId](const FLOTNWorldLocation& Entry) { return Entry.LocationId == LocationId; });

    if (!Found)
    {
        return false;
    }

    OutLocation = *Found;
    return true;
}

bool ULOTNWorldMapData::IsLocationUnlocked(FName LocationId, int32 CurrentRenown) const
{
    FLOTNWorldLocation Location;
    return GetLocationById(LocationId, Location) &&
        (Location.bUnlockedByDefault || CurrentRenown >= Location.RequiredRenown);
}

bool ULOTNWorldMapData::IsLocationReadyToTravel(FName LocationId, int32 CurrentRenown) const
{
    FLOTNWorldLocation Location;
    return GetLocationById(LocationId, Location) &&
        IsLocationUnlocked(LocationId, CurrentRenown) &&
        !Location.LevelName.IsNone();
}
