// X/Common/ActorCommon.cpp

#include "ActorCommon.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/MarkerSetDefinition.h"
#include "Game.h"
#include "GlobalSNO.h"
#include "InactiveMarker.h"
#include "Marker/Marker.h"
#include "TagMaps/Tags.h"








MinimapMarkerSetInfo::MinimapMarkerSetInfo( const InactiveMarkerOwner & tMarker )
{
    // Copy only the marker set and index; the adventure and instance fields stay with the owner.
    snoMarkerSet = tMarker.snoMarkerSet;
    nMarkerIndex = tMarker.nMarkerIndex;
}




float ActorGetInheritedTagMapValueFloat(
    const SNO snoActor,
    const MinimapMarkerSetInfo & tMarkerOwner,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( tMarkerOwner.snoMarkerSet != SNO_NONE )
    {
        ScopedMarkerSetDefinitionLockR pdefMarkerSet( tMarkerOwner.snoMarkerSet );
        int32 nIndex = tMarkerOwner.nMarkerIndex;
        if ( nIndex >= 0 && nIndex < pdefMarkerSet->tMarkerSet.GetArraySize() )
        {
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
            return ActorGetInheritedTagMapValueFloat( snoActor, tMarker, dwActorTag, dwMarkerTag );
        }
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetFloat( pdefActor->hTagMap, dwActorTag );
}




int32 ActorGetInheritedTagMapValueInt(
    const SNO snoActor,
    const MinimapMarkerSetInfo & tMarkerOwner,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( tMarkerOwner.snoMarkerSet != SNO_NONE )
    {
        ScopedMarkerSetDefinitionLockR pdefMarkerSet( tMarkerOwner.snoMarkerSet );
        int32 nIndex = tMarkerOwner.nMarkerIndex;
        if ( nIndex >= 0 && nIndex < pdefMarkerSet->tMarkerSet.GetArraySize() )
        {
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
            return ActorGetInheritedTagMapValueInt( snoActor, tMarker, dwActorTag, dwMarkerTag );
        }
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetInt( pdefActor->hTagMap, dwActorTag );
}




uint32 ActorGetInheritedTagMapValueDWORD(
    const SNO snoActor,
    const MinimapMarkerSetInfo & tMarkerOwner,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( tMarkerOwner.snoMarkerSet != SNO_NONE )
    {
        ScopedMarkerSetDefinitionLockR pdefMarkerSet( tMarkerOwner.snoMarkerSet );
        int32 nIndex = tMarkerOwner.nMarkerIndex;
        if ( nIndex >= 0 && nIndex < pdefMarkerSet->tMarkerSet.GetArraySize() )
        {
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
            return ActorGetInheritedTagMapValueDWORD( snoActor, tMarker, dwActorTag, dwMarkerTag );
        }
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetDWORD( pdefActor->hTagMap, dwActorTag );
}




SNO ActorGetInheritedTagMapValueSNO(
    const SNO snoActor,
    const MinimapMarkerSetInfo & tMarkerOwner,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( tMarkerOwner.snoMarkerSet != SNO_NONE )
    {
        ScopedMarkerSetDefinitionLockR pdefMarkerSet( tMarkerOwner.snoMarkerSet );
        int32 nIndex = tMarkerOwner.nMarkerIndex;
        if ( nIndex >= 0 && nIndex < pdefMarkerSet->tMarkerSet.GetArraySize() )
        {
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
            return ActorGetInheritedTagMapValueSNO( snoActor, tMarker, dwActorTag, dwMarkerTag );
        }
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetSNO( pdefActor->hTagMap, dwActorTag );
}



GBID ActorGetInheritedTagMapValueGBID(
    const SNO snoActor,
    const MinimapMarkerSetInfo & tMarkerOwner,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( tMarkerOwner.snoMarkerSet != SNO_NONE )
    {
        ScopedMarkerSetDefinitionLockR pdefMarkerSet( tMarkerOwner.snoMarkerSet );
        int32 nIndex = tMarkerOwner.nMarkerIndex;
        if ( nIndex >= 0 && nIndex < pdefMarkerSet->tMarkerSet.GetArraySize() )
        {
            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
            return ActorGetInheritedTagMapValueGBID( snoActor, tMarker, dwActorTag, dwMarkerTag );
        }
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetGBID( pdefActor->hTagMap, dwActorTag );
}


float ActorGetInheritedTagMapValueFloat(
    const SNO snoActor,
    const Marker & tMarker,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( TagMapTagExists( tMarker.hTagMap, dwMarkerTag ) )
    {
        return TagMapGetFloat( tMarker.hTagMap, dwMarkerTag );
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetFloat( pdefActor->hTagMap, dwActorTag );
}


int32 ActorGetInheritedTagMapValueInt(
    const SNO snoActor,
    const Marker & tMarker,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( TagMapTagExists( tMarker.hTagMap, dwMarkerTag ) )
    {
        return TagMapGetInt( tMarker.hTagMap, dwMarkerTag );
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetInt( pdefActor->hTagMap, dwActorTag );
}


uint32 ActorGetInheritedTagMapValueDWORD(
    const SNO snoActor,
    const Marker & tMarker,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( TagMapTagExists( tMarker.hTagMap, dwMarkerTag ) )
    {
        return TagMapGetDWORD( tMarker.hTagMap, dwMarkerTag );
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetDWORD( pdefActor->hTagMap, dwActorTag );
}


SNO ActorGetInheritedTagMapValueSNO(
    const SNO snoActor,
    const Marker & tMarker,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( TagMapTagExists( tMarker.hTagMap, dwMarkerTag ) )
    {
        return TagMapGetSNO( tMarker.hTagMap, dwMarkerTag );
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetSNO( pdefActor->hTagMap, dwActorTag );
}


GBID ActorGetInheritedTagMapValueGBID(
    const SNO snoActor,
    const Marker & tMarker,
    const uint32 dwActorTag,
    const uint32 dwMarkerTag )
{
    if ( TagMapTagExists( tMarker.hTagMap, dwMarkerTag ) )
    {
        return TagMapGetGBID( tMarker.hTagMap, dwMarkerTag );
    }

    ScopedActorDefinitionLockR pdefActor( snoActor );
    return TagMapGetGBID( pdefActor->hTagMap, dwActorTag );
}



float ActorGetHandicapHealthScalar( int32 nHandicapLevel )
{
    float flScalar = 1.0f;

    if ( nHandicapLevel >= 0 )
    {
        HandicapLevel * ptLevel = GameGetInfoForHandicapLevel( nHandicapLevel );
        if ( ptLevel )
        {
            flScalar = ptLevel->tHandicapValues.flHealthScalar;
        }
    }
    return flScalar;
}




float ActorGetHandicapHealthScalar()
{
    int32 nHandicapLevel = GameGetHandicapLevel();
    return ActorGetHandicapHealthScalar( nHandicapLevel );
}




BOOL ActorIsKeywarden( SNO snoActor )
{
    if ( snoActor == GlobalSNOGet( GLOBALSNO_ACTOR_KEY_WARDEN_A1 ) ||
         snoActor == GlobalSNOGet( GLOBALSNO_ACTOR_KEY_WARDEN_A2 ) ||
         snoActor == GlobalSNOGet( GLOBALSNO_ACTOR_KEY_WARDEN_A3 ) ||
         snoActor == GlobalSNOGet( GLOBALSNO_ACTOR_KEY_WARDEN_A4 ) )
    {
        return TRUE;
    }

    return FALSE;
}
