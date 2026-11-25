// Original file: X/Server/Achievements/AchievementsEngine/achievements_evaluator.cpp

#include "achievements_evaluator.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "achievements_listener.h"
#include "achievements_progress_data.h"
#include "achievements_update_structs.h"
#include "../../../Common/Achievements/AchievementsEngine/achievements_static_data.h"
#include "../../../Common/Achievements/AchievementsEngine/achievements_internal_static_data_structs.h"

namespace OnlineServiceServer { Blizzard::Time::UnixTime64 GetCurrentUnixTime(); }  // OnlineServiceServer.cpp:1436

namespace OnlineService {
namespace AchievementsEngine {













AchievementsEvaluator::AchievementsEvaluator(uint32 uUnk0, const AchievementsStaticData * staticData, ProgressData * snapshot, uint32 activePartitionsMask)
    : unk_0(uUnk0), m_staticData(staticData)
    , m_activePartitionsMask(activePartitionsMask)
    , m_snapshot(snapshot)
    , m_stateListener(NULL)
    , m_flushImmediately(false)
    , m_currentEventType(ADVANCE_EVENT)
    , m_checkEventHadSideEffects(false)
    , m_notificationsEnabled(false) {
}






bool AchievementsEvaluator::CheckEvent(CriteriaEventId eventId, CriteriaComparand eventComparand, CriteriaQuantity quantity, uint64 updateTimeSeconds, int32 nUnk, const CheckEventCallback & cb) {
    m_checkEventHadSideEffects = false;
    m_subjetModifierHintMap.clear();
    m_objectModifierHintMap.clear();
    EventKey genericKey(eventId, 0, false);
    EventKey specificKey(eventId, eventComparand, true);
    m_currentEventType = FAIL_EVENT;
    CheckEventMap(genericKey, m_staticData->GetFailEvents(), quantity, updateTimeSeconds, nUnk, cb);
    CheckEventMap(specificKey, m_staticData->GetFailEvents(), quantity, updateTimeSeconds, nUnk, cb);
    m_currentEventType = START_EVENT;
    CheckEventMap(genericKey, m_staticData->GetStartEvents(), quantity, updateTimeSeconds, nUnk, cb);
    CheckEventMap(specificKey, m_staticData->GetStartEvents(), quantity, updateTimeSeconds, nUnk, cb);
    if (quantity != 0) {
        m_currentEventType = ADVANCE_EVENT;
        CheckEventMap(genericKey, m_staticData->GetAdvanceEvents(), quantity, updateTimeSeconds, nUnk, cb);
        CheckEventMap(specificKey, m_staticData->GetAdvanceEvents(), quantity, updateTimeSeconds, nUnk, cb);
    }
    return m_checkEventHadSideEffects;
}

























inline void AchievementsEvaluator::CheckEventMap(EventKey key, const PartitionedCriteriaEventMap & eventMap, uint64 quantity, uint64 updateTimeSeconds, int32 nUnk, const CheckEventCallback & cb) {
    for (uint32 partition = 0; (1u << partition) <= m_activePartitionsMask; ++partition) {
        if (!(nUnk & (1u << partition)) || !(m_activePartitionsMask & (1u << partition)))
            continue;
        if (partition >= eventMap.size())
            break;
        CheckEventMap(key, eventMap[partition], quantity, updateTimeSeconds, cb);
    }
}















void AchievementsEvaluator::CheckEventMap(EventKey key, const CriteriaEventMap & eventMap, uint64 quantity, uint64 updateTimeSeconds, const CheckEventCallback & cb) {
    CriteriaEventMap::const_iterator eventIterator = eventMap.find(key);
    if (eventIterator == eventMap.end())
        return;
    const CriteriaVector & criteriaVector = eventIterator->second;
    CriteriaVector::const_iterator criteriaIteratorStart = criteriaVector.begin();
    CriteriaVector::const_iterator criteriaIteratorEnd = criteriaVector.end();
    for (; criteriaIteratorStart != criteriaIteratorEnd; ++criteriaIteratorStart) {
        uint32 flags;
        const InternalCriteriaDefinition * criteria = *criteriaIteratorStart;
        const CriteriaEvent * event;
        switch (m_currentEventType) {
        case ADVANCE_EVENT:
            flags = 0;
            event = &criteria->m_advanceEvent;
            break;
        case START_EVENT:
            quantity = 0;
            flags = TRACKED_CRITERIA_FLAG_STARTED;
            event = &criteria->m_startEvent;
            break;
        case FAIL_EVENT:
            quantity = 0;
            flags = TRACKED_CRITERIA_FLAG_FAILED;
            event = &criteria->m_failEvent;
            break;
        default:
            continue;
        }
        if (!cb || EvaluateModifiers(criteria, *event, cb)) {
            CriteriaUpdateRecord criteriaUpdate(unk_0);
            criteriaUpdate.m_criteria = criteria;
            criteriaUpdate.m_updateTime = updateTimeSeconds;
            criteriaUpdate.m_quantity = quantity;
            criteriaUpdate.m_flags = flags;
            UpdateCriteria(criteriaUpdate, false);
        }
    }
}


























bool AchievementsEvaluator::EvaluateModifiers(const InternalCriteriaDefinition *, const CriteriaEvent & event, const CheckEventCallback & cb) {
    bool retval = true;
    const int32 modifierCount = event.m_dwModifierCount;
    for (int i = 0; i < modifierCount; i++) {
        if (i > 0) {
            if (event.m_modifier[i].m_operand == CriteriaModifierOperand::OR && retval)
                return true;
            if (event.m_modifier[i].m_operand == CriteriaModifierOperand::AND && !retval)
                continue;
        }
        const CriteriaModifier & modifier = event.m_modifier[i];
        const uint64 condition = modifier.m_necessaryCondition;
        const uint64 comparand = modifier.m_comparand;
        EventKey hintKey(condition, comparand, true);
        if (modifier.m_target.GetValue() == 'Subj') {
            HintMap::const_iterator it = m_subjetModifierHintMap.find(hintKey);
            if (it != m_subjetModifierHintMap.end()) {
                retval = it->second;
            } else {
                retval = cb(condition, comparand);
                m_subjetModifierHintMap[hintKey] = retval;
            }
        } else if (modifier.m_target.GetValue() == 'Obj') {
            HintMap::const_iterator it = m_objectModifierHintMap.find(hintKey);
            if (it != m_objectModifierHintMap.end()) {
                retval = it->second;
            } else {
                retval = cb(condition, comparand);
                m_objectModifierHintMap[hintKey] = retval;
            }
        } else {
            retval = cb(condition, comparand);
        }
    }
    return retval;
}
































inline bool AchievementsEvaluator::ShouldProcessCriteria(const CriteriaUpdateRecord & criteriaUpdate, const InternalCriteriaDefinition * criteria, const InternalAchievementDefinition * achievement) const {
    if ((criteriaUpdate.m_flags & TRACKED_CRITERIA_FLAG_CHEAT) && criteria->m_proxyCriteria)
        return true;
    if (criteria->m_sharingThisCriteria || achievement->m_sharedChildrenAchievements.size() != 0)
        return true;
    Achievement * achievementAwarded;
    if (m_snapshot->FindAchievement(unk_0, achievement->m_id, achievementAwarded))
        return false;
    Criteria * criteriaSnapshot = NULL;
    if (m_snapshot->FindCriteria(unk_0, criteria->m_id, criteriaSnapshot) && HasSatisfiedCriteria(criteriaSnapshot, criteria))
        return false;
    return true;
}



















inline bool AchievementsEvaluator::HasSatisfiedCriteria(Criteria * trackedCriteria, const InternalCriteriaDefinition * criteriaDefinition) const {
    return GetCriteriaQuantity(trackedCriteria) >= criteriaDefinition->m_necessaryQuantity;
}






inline CriteriaQuantity AchievementsEvaluator::GetCriteriaQuantity(Criteria * trackedCriteria) const
{
    CriteriaQuantity quantity = trackedCriteria->m_baseQuantity + trackedCriteria->m_deltaQuantity;
    return quantity;
}




void AchievementsEvaluator::LoadCriteria(CriteriaId idCriteria, CriteriaQuantity uQuantity) {
    Criteria * trackedCriteria = NULL;
    m_snapshot->AllocateTrackedCriteria(unk_0, idCriteria, trackedCriteria);
    trackedCriteria->m_criteriaId = idCriteria;
    trackedCriteria->m_baseQuantity = uQuantity;
}






ErrorCode AchievementsEvaluator::UpdateCriteria(const CriteriaUpdateRecord & criteriaUpdate, bool fromDB) {
    const InternalCriteriaDefinition * criteria = criteriaUpdate.m_criteria;
    if (!criteria)
        return ERROR_NO_ACHIEVEMENT;
    if (!ShouldProcessCriteria(criteriaUpdate, criteria, criteria->m_parentAchievement))
        return ERROR_GLOBAL_SUCCESS;
    Criteria * trackedCriteria = NULL;
    m_snapshot->FindCriteria(unk_0, criteria->m_id, trackedCriteria);
    if (fromDB) {
        DisplayInternalError(NULL, "", 338, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    if (trackedCriteria) {
        const bool hasStartEvent = criteria->m_startEvent.m_id != 0;
        const bool hasDuration = criteria->m_startEvent.m_timerDuration != 0;
        const bool hasStartTime = trackedCriteria->m_startTime != 0;
        trackedCriteria->m_updateTime = criteriaUpdate.m_updateTime;
        if (hasStartEvent && hasDuration && hasStartTime) {
            const uint64 timeWhenCriteriaShouldFail = trackedCriteria->m_startTime + criteria->m_startEvent.m_timerDuration;
            if (GetSeconds() > timeWhenCriteriaShouldFail) {
                FailCriteria(criteria, 0, criteriaUpdate.m_updateTime, fromDB, false);
            }
        }
    }
    if (criteriaUpdate.m_flags & TRACKED_CRITERIA_FLAG_FAILED) {
        FailCriteria(criteria, criteriaUpdate.m_flags, criteriaUpdate.m_updateTime, fromDB, false);
    }
    if (criteriaUpdate.m_flags & TRACKED_CRITERIA_FLAG_STARTED) {
        StartCriteria(criteriaUpdate, criteria, fromDB);
    }
    if (criteriaUpdate.m_quantity != 0) {
        UpdateCriteriaQuantity(criteriaUpdate, criteria, fromDB);
    }
    return ERROR_GLOBAL_SUCCESS;
}







































void AchievementsEvaluator::DeleteAchievementTrackedCriteria(const InternalAchievementDefinition * staticAchievement) {
    if (staticAchievement->m_nMinRequiredCriteria == 0 && staticAchievement->m_sharedChildrenAchievements.size() == 0) {
        for (size_t i = 0; i < staticAchievement->m_criteriaList.size(); i++) {
            const InternalCriteriaDefinition * staticCriteria = staticAchievement->m_criteriaList[i];
            if (!staticCriteria->m_sharingThisCriteria) {
                DeleteTrackedCriteria(staticCriteria->m_id, staticCriteria);
            }
        }
    }
}


















inline void AchievementsEvaluator::DeleteTrackedCriteria(CriteriaId id, const InternalCriteriaDefinition * criteria) {
    Criteria * trackedCriteria = NULL;
    if (m_snapshot->FindCriteria(unk_0, id, trackedCriteria)) {
        if ((trackedCriteria->m_flags & TRACKED_CRITERIA_FLAG_SERIALIZED) && (criteria->m_parentAchievement->m_partitionMask & m_activePartitionsMask)) {
            trackedCriteria->m_flags |= TRACKED_CRITERIA_FLAG_DELETE_FROM_DB;
            MarkDirty(trackedCriteria, criteria);
        } else {
            FreeTrackedCriteria(criteria);
        }
    }
}













void AchievementsEvaluator::FreeTrackedCriteria(const InternalCriteriaDefinition * criteria) {
    if (m_snapshot->FreeCriteria(unk_0, criteria->m_id)) {
        CriteriaDeleteRecord tDeleteRecord(unk_0);
        tDeleteRecord.m_criteria = criteria;
        InformListeners(tDeleteRecord);

        MarkClean(criteria->m_id);
    }

    if (!m_notificationsEnabled) {
        m_failedCriteria.push_back(criteria->m_id);
    }
}









void AchievementsEvaluator::StartCriteria(const CriteriaUpdateRecord & criteriaUpdate, const InternalCriteriaDefinition * criteria, bool fromDB) {
    Criteria * trackedCriteria = NULL;
    m_snapshot->FindCriteria(unk_0, criteria->m_id, trackedCriteria);
    if (fromDB) {
        DisplayInternalError(NULL, "", 478, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    if (!trackedCriteria) {
        m_snapshot->AllocateTrackedCriteria(unk_0, criteria->m_id, trackedCriteria);
        trackedCriteria->m_criteriaId = criteria->m_id;
    } else if (!(trackedCriteria->m_flags & TRACKED_CRITERIA_FLAG_FAILED) && !(criteria->m_flags & CRITERIA_FLAG_RESET_ON_START)) {
        return;
    }
    trackedCriteria->m_flags &= ~TRACKED_CRITERIA_FLAG_FAILED;
    if (criteria->m_startEvent.m_timerDuration != 0) {
        trackedCriteria->m_startTime = criteriaUpdate.m_startTime ? criteriaUpdate.m_startTime : GetSeconds();
    }
    trackedCriteria->m_baseQuantity = 0;
    trackedCriteria->m_deltaQuantity = 0;
    trackedCriteria->m_updateTime = criteriaUpdate.m_updateTime;
    MarkDirty(trackedCriteria, criteria);
    CriteriaUpdateRecord newUpdate(unk_0);
    newUpdate.m_flags = TRACKED_CRITERIA_FLAG_STARTED;
    newUpdate.m_criteria = criteria;
    newUpdate.m_startTime = trackedCriteria->m_startTime;
    newUpdate.m_updateTime = trackedCriteria->m_updateTime;
    InformListeners(newUpdate);
}
















































































void AchievementsEvaluator::FailCriteria(
    const InternalCriteriaDefinition * criteria,
    uint32 flags,
    uint64 updateTimeSeconds,
    bool fromDB,
    bool forceFail)
{
    Achievement * sharedPtr;

    if (m_snapshot->FindAchievement(unk_0, criteria->m_parentAchievementId, sharedPtr) &&
        !(flags & TRACKED_CRITERIA_FLAG_CHEAT))
    {
        return;
    }

    if (flags & TRACKED_CRITERIA_FLAG_CHEAT)  // The original combines this bit with forceFail at 0x6ccdd0.
    {
        forceFail = true;
    }


    Criteria * trackedCriteria = NULL;
    m_snapshot->FindCriteria(unk_0, criteria->m_id, trackedCriteria);

    if (fromDB)
    {

        DisplayInternalError(NULL, "", 599, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }




























    if (!trackedCriteria)
    {


        if (criteria->m_flags & CRITERIA_FLAG_FAIL_ACHIEVEMENT)
        {


            FailAllCriteria(criteria->m_parentAchievement, updateTimeSeconds, fromDB);
        }
        return;
    }





    if (!forceFail && HasSatisfiedCriteria(trackedCriteria, criteria))
    {
        return;
    }





    trackedCriteria->m_flags &= ~TRACKED_CRITERIA_FLAG_STARTED;
    trackedCriteria->m_flags |= TRACKED_CRITERIA_FLAG_FAILED;
    trackedCriteria->m_flags |= flags & TRACKED_CRITERIA_FLAG_CHEAT;
    trackedCriteria->m_deltaQuantity = 0;
    trackedCriteria->m_updateTime = updateTimeSeconds;



    MarkDirty(trackedCriteria, criteria);

    trackedCriteria->m_baseQuantity = 0;














    if (!(trackedCriteria->m_flags & TRACKED_CRITERIA_FLAG_SERIALIZED))
    {
        FreeTrackedCriteria(criteria);
    }


    if (criteria->m_flags & CRITERIA_FLAG_FAIL_ACHIEVEMENT)
    {


        FailAllCriteria(criteria->m_parentAchievement, updateTimeSeconds, fromDB);
    }
}


inline void AchievementsEvaluator::FailAllCriteria(const InternalAchievementDefinition * staticAchievement, uint64 updateTimeSeconds, bool fromDB) {
    if (staticAchievement->m_flags & ACHIEVEMENT_FLAG_STATISTIC)
        return;
    for (size_t i = 0; i < staticAchievement->m_criteriaList.size(); i++) {
        Criteria * trackedCriteria = NULL;
        if (m_snapshot->FindCriteria(unk_0, staticAchievement->m_criteriaList[i]->m_id, trackedCriteria) && !(trackedCriteria->m_flags & TRACKED_CRITERIA_FLAG_FAILED)) {
            FailCriteria(staticAchievement->m_criteriaList[i], 0, updateTimeSeconds, fromDB, true);
        }
    }
}

























































































void AchievementsEvaluator::UpdateCriteriaQuantity(const CriteriaUpdateRecord & criteriaUpdate, const InternalCriteriaDefinition * criteria, bool fromDB) {
    if ((criteriaUpdate.m_flags & TRACKED_CRITERIA_FLAG_CHEAT) && criteria->m_proxyCriteria) {
        CriteriaUpdateRecord proxyUpdate = criteriaUpdate;
        proxyUpdate.m_criteria = criteria->m_proxyCriteria;
        proxyUpdate.m_updateTime = criteriaUpdate.m_updateTime;
        UpdateCriteriaQuantity(proxyUpdate, criteria->m_proxyCriteria, fromDB);
        return;
    }
    Criteria * trackedCriteria = NULL;
    m_snapshot->FindCriteria(unk_0, criteria->m_id, trackedCriteria);
    if (!trackedCriteria) {
        if (!fromDB && criteria->m_startEvent.m_id != 0)
            return;
        m_snapshot->AllocateTrackedCriteria(unk_0, criteria->m_id, trackedCriteria);
        trackedCriteria->m_criteriaId = criteria->m_id;
        trackedCriteria->m_startTime = criteriaUpdate.m_startTime;
    } else if (!fromDB && criteria->m_startEvent.m_id != 0 && (trackedCriteria->m_flags & TRACKED_CRITERIA_FLAG_FAILED)) {
        return;
    }
    if (criteria->m_sharingThisCriteria && (fromDB || (criteriaUpdate.m_flags & TRACKED_CRITERIA_FLAG_CHEAT))) {
        PropagateSharedCriteriaDBValue(criteriaUpdate, criteria, fromDB);
    }
    trackedCriteria->m_flags &= ~TRACKED_CRITERIA_FLAG_FAILED;
    trackedCriteria->m_updateTime = criteriaUpdate.m_updateTime;
    if (fromDB) {
        DisplayInternalError(NULL, "", 865, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    BOOL preIncrementSatisfied = HasSatisfiedCriteria(trackedCriteria, criteria);
    trackedCriteria->m_deltaQuantity += criteriaUpdate.m_quantity;
    MarkDirty(trackedCriteria, criteria);
    CriteriaUpdateRecord newUpdate(unk_0);
    newUpdate.m_criteria = criteria;
    newUpdate.m_startTime = trackedCriteria->m_startTime;
    newUpdate.m_quantity = GetCriteriaQuantity(trackedCriteria);
    newUpdate.m_flags = 0;
    newUpdate.m_updateTime = trackedCriteria->m_updateTime;
    InformListeners(newUpdate);
    if (!preIncrementSatisfied && trackedCriteria->m_deltaQuantity != 0 && HasSatisfiedCriteria(trackedCriteria, criteria)) {
        SignalImmediateFlush();
    }
    EvaluateAwardAchievement(criteria->m_parentAchievement, criteriaUpdate.m_updateTime);
}
























































































void AchievementsEvaluator::PropagateSharedCriteriaDBValue(const CriteriaUpdateRecord & criteriaUpdate, const InternalCriteriaDefinition * criteria, bool fromDB) {
    if (criteria->m_advanceEvent.m_id == 0)
        return;
    EventKey key(criteria->m_advanceEvent.m_id, criteria->m_advanceEvent.m_comparand, criteria->m_advanceEvent.m_usesComparand);
    const PartitionedCriteriaEventMap & partitionedAdvanceEvents = m_staticData->GetAdvanceEvents();
    for (uint32 partition = 0; (1u << partition) <= m_activePartitionsMask && partition < partitionedAdvanceEvents.size(); ++partition) {
        if (!(m_activePartitionsMask & (1u << partition)))
            continue;
        const CriteriaEventMap & advanceEvents = partitionedAdvanceEvents[partition];
        CriteriaEventMap::const_iterator it = advanceEvents.find(key);
        if (it == advanceEvents.end())
            continue;
        const CriteriaVector & criteriaList = it->second;
        for (size_t i = 0; i < criteriaList.size(); ++i) {
            if (criteriaList[i]->m_proxyCriteria == criteria) {
                CriteriaUpdateRecord sharedUpdate = criteriaUpdate;
                sharedUpdate.m_criteria = criteriaList[i];
                sharedUpdate.m_flags &= ~TRACKED_CRITERIA_FLAG_CHEAT;
                UpdateCriteriaQuantity(sharedUpdate, criteriaList[i], fromDB);
            }
        }
    }
}


































inline void AchievementsEvaluator::InformListeners(const CriteriaUpdateRecord & criteriaUpdate) const {
    for (uint32 i = 0; i < m_listeners.size(); i++) {
        m_listeners[i]->OnCriteriaUpdate(criteriaUpdate);
    }
}



inline void AchievementsEvaluator::InformListeners(const CriteriaDeleteRecord & criteriaDeleted) const {
    for (uint32 i = 0; i < m_listeners.size(); i++) {
        m_listeners[i]->OnCriteriaDeleted(criteriaDeleted);
    }
}



inline void AchievementsEvaluator::InformListeners(const AchievementUpdateRecord & achievementUpdate) const {
    for (uint32 i = 0; i < m_listeners.size(); i++) {
        if (achievementUpdate.m_completion != 0) {
            m_listeners[i]->OnAchievementUpdate(achievementUpdate);
        }
    }
}
















void AchievementsEvaluator::RegisterListener(blz::shared_ptr<Listener> listener) {
    m_listeners.push_back(listener);
}


bool AchievementsEvaluator::UnregisterListener(blz::shared_ptr<Listener> listener) {
    for (ListenerVector::const_iterator iter = m_listeners.begin(); iter != m_listeners.end(); ++iter) {
        if (*iter == listener) {
            m_listeners.erase(iter);
            return true;
        }
    }
    return false;
}









void AchievementsEvaluator::EnableNotifications() {
    for (size_t i = 0; i < m_listeners.size(); i++) {
        m_listeners[i]->EnableNotifications();
    }
    m_notificationsEnabled = true;
    m_failedCriteria.clear();
}





void AchievementsEvaluator::SendAccumulatedUpdates() {
    for (uint32 i = 0; i < m_listeners.size(); i++) {
        m_listeners[i]->SendAccumulatedUpdates();
    }
}




void AchievementsEvaluator::EvaluateAwardAchievement(const InternalAchievementDefinition * staticAchievement, uint64 updateTimeSeconds) {
    uint64 numCriteriaExceeded;
    uint64 totalCriteria;
    if (!(staticAchievement->m_flags & ACHIEVEMENT_FLAG_STATISTIC)) {
        AccumulateAchievementCriteriaQuantity(staticAchievement, (AchievementStaticFlags)staticAchievement->m_flags, numCriteriaExceeded, totalCriteria);
        EvaluateAccumulatedAchievementCriteriaQuantity(staticAchievement, numCriteriaExceeded, totalCriteria, updateTimeSeconds);
    }
    for (size_t i = 0; i < staticAchievement->m_sharedChildrenAchievements.size(); i++) {
        const InternalAchievementDefinition * sharedAchievement = staticAchievement->m_sharedChildrenAchievements[i];
        if (!(sharedAchievement->m_flags & ACHIEVEMENT_FLAG_STATISTIC)) {
            AccumulateAchievementCriteriaQuantity(staticAchievement, (AchievementStaticFlags)sharedAchievement->m_flags, numCriteriaExceeded, totalCriteria);
            EvaluateAccumulatedAchievementCriteriaQuantity(sharedAchievement, numCriteriaExceeded, totalCriteria, updateTimeSeconds);
        }
    }
}





























void AchievementsEvaluator::AccumulateAchievementCriteriaQuantity(
    const InternalAchievementDefinition * staticAchievement,
    const AchievementStaticFlags accumulationFlags,
    uint64 & numCriteriaExceeded,
    uint64 & totalCriteria)
{
    numCriteriaExceeded = 0;
    totalCriteria = staticAchievement->m_criteriaList.size();

    for (size_t i = 0; i < staticAchievement->m_criteriaList.size(); ++i)
    {
        const InternalCriteriaDefinition * staticCriteria = staticAchievement->m_criteriaList.at(i);


        if ((accumulationFlags & ACHIEVEMENT_FLAG_CUMULATIVE) &&
            staticCriteria->m_uiOrder == 0)
        {
            totalCriteria = staticCriteria->m_necessaryQuantity;
        }

        Criteria * trackedCriteria = NULL;
        if (!m_snapshot->FindCriteria(unk_0, staticCriteria->m_id, trackedCriteria))
        {
            continue;
        }

        if (accumulationFlags & ACHIEVEMENT_FLAG_CUMULATIVE)
        {
            numCriteriaExceeded += GetCriteriaQuantity(trackedCriteria);
        }
        else
        {
            if (HasSatisfiedCriteria(trackedCriteria, staticCriteria))
            {
                ++numCriteriaExceeded;
            }
        }
    }
}

inline void AchievementsEvaluator::EvaluateAccumulatedAchievementCriteriaQuantity(const InternalAchievementDefinition * staticAchievement, uint64 & numCriteriaExceeded, uint64 & totalCriteria, uint64 updateTimeSeconds) {
    if ((staticAchievement->m_nMinRequiredCriteria != 0 && numCriteriaExceeded >= staticAchievement->m_nMinRequiredCriteria) || numCriteriaExceeded >= totalCriteria) {
        Achievement achievementRecord;
        achievementRecord.m_achievementId = staticAchievement->m_id;
        achievementRecord.m_completion = updateTimeSeconds ? updateTimeSeconds : GetSeconds();
        UpdateAchievementInternal(staticAchievement, &achievementRecord, false);
    }
}








inline void AchievementsEvaluator::MarkDirty(Criteria * trackedCriteria, const InternalCriteriaDefinition * staticCriteria) const {
    if (staticCriteria->m_proxyCriteria) {
        trackedCriteria->m_baseQuantity += trackedCriteria->m_deltaQuantity;
        trackedCriteria->m_deltaQuantity = 0;
    }
    if (!staticCriteria->m_proxyCriteria) {
        m_dirtyCriteria[trackedCriteria->m_criteriaId] = trackedCriteria;
        m_checkEventHadSideEffects = true;
    }
}







inline void AchievementsEvaluator::MarkDirty(Achievement * trackedAchievement, const InternalAchievementDefinition *) const {
    m_dirtyAchievements[trackedAchievement->m_achievementId] = trackedAchievement;
    m_checkEventHadSideEffects = true;
}




inline void AchievementsEvaluator::MarkClean(CriteriaId id) const {
    m_dirtyCriteria.erase(id);
}


void AchievementsEvaluator::Flush() {
    FlushAchievements();
    FlushCriteria();
    if (m_stateListener) {
        m_stateListener->SendAccumulatedUpdates();
    }
    m_lastFlushTime = Blizzard::Time::Microseconds();
    m_flushImmediately = false;
}






















// NON_MATCHING: clear() loads m_buckets and m_bucket_count separately; the original loads them with one ldp
void AchievementsEvaluator::FlushAchievements()
{
    if (m_stateListener)
    {
        TrackedAchievementMap::const_iterator it = m_dirtyAchievements.begin();
        for (; it != m_dirtyAchievements.end(); ++it)
        {
            AchievementUpdateRecord * achievement = it->second;
            m_stateListener->OnAchievementUpdate(*achievement);
        }

        m_dirtyAchievements.clear();
    }
}
// NON_MATCHING: ours 972 vs 960 bytes; empty-map path duplicates the clear()/free tail and begin() scan block is laid out later
void AchievementsEvaluator::FlushCriteria()
{
    const CriteriaMap & criteriaMap = m_staticData->GetCriteriaMap();
    blz::vector<unsigned long> criteriaToRemove;

    for (TrackedCriteriaMap::const_iterator iter = m_dirtyCriteria.begin(); iter != m_dirtyCriteria.end(); ++iter)
    {
        Criteria * trackedCriteria = iter->second;
        trackedCriteria->m_baseQuantity += trackedCriteria->m_deltaQuantity;
        trackedCriteria->m_deltaQuantity = 0;

        if (m_stateListener)
        {  // Original row 1249:80 puts the addition after m_deltaQuantity.
            CriteriaUpdateRecord criteriaUpdateRecord(unk_0);
            criteriaUpdateRecord.m_criteria = criteriaMap.at(trackedCriteria->m_criteriaId);
            criteriaUpdateRecord.m_flags = trackedCriteria->m_flags;
            criteriaUpdateRecord.m_quantity = trackedCriteria->m_deltaQuantity +
                trackedCriteria->m_baseQuantity;
            criteriaUpdateRecord.m_startTime = trackedCriteria->m_startTime;
            criteriaUpdateRecord.m_updateTime = trackedCriteria->m_updateTime;

            if (trackedCriteria->m_flags & TRACKED_CRITERIA_FLAG_FAILED)
            {
                criteriaToRemove.push_back(trackedCriteria->m_criteriaId);
            }

            m_stateListener->OnCriteriaUpdate(criteriaUpdateRecord);
        }

        else if (trackedCriteria->m_flags & TRACKED_CRITERIA_FLAG_FAILED)
        {
            criteriaToRemove.push_back(trackedCriteria->m_criteriaId);
        }
    }


    for (size_t i = 0; i < criteriaToRemove.size(); ++i)
    {
        const InternalCriteriaDefinition * criteria = criteriaMap.at(criteriaToRemove[i]);
        FreeTrackedCriteria(criteria);
    }

    m_dirtyCriteria.clear();
}

inline void AchievementsEvaluator::SignalImmediateFlush() const {
    m_flushImmediately = true;
}


void AchievementsEvaluator::UpdateAchievement(const AchievementUpdateRecord & achievementUpdate, bool fromDB) {
    const InternalAchievementDefinition * staticAchievement = m_staticData->GetAchievementDefinition(achievementUpdate.m_achievementId);
    if (staticAchievement) {
        UpdateAchievementInternal(staticAchievement, &achievementUpdate, fromDB);
    }
}












void AchievementsEvaluator::UpdateAchievementInternal(const InternalAchievementDefinition * staticAchievement, const Achievement * achievement, const bool fromDB) {
    Achievement * trackedAchievement;
    bool alreadyExists;
    if (achievement->m_completion == 0) {
        if (fromDB) {
            m_snapshot->FreeAchievement(unk_0, achievement->m_achievementId);
        } else {
            Achievement * foundAchievement = NULL;
            if (m_snapshot->FindAchievement(unk_0, achievement->m_achievementId, foundAchievement)) {
                foundAchievement->m_completion = 0;
                MarkDirty(foundAchievement, staticAchievement);
                SignalImmediateFlush();
            }
        }
        InformListeners(*achievement);
    } else {
        alreadyExists = m_snapshot->FindAchievement(unk_0, achievement->m_achievementId, trackedAchievement);
        if (!alreadyExists) {
            m_snapshot->AllocateAchievement(unk_0, achievement->m_achievementId, trackedAchievement);
            trackedAchievement->m_achievementId = achievement->m_achievementId;
            trackedAchievement->m_completion = achievement->m_completion;
            DeleteAchievementTrackedCriteria(staticAchievement);
            if (!fromDB) {
                InformListeners(*achievement);
                MarkDirty(trackedAchievement, staticAchievement);
                SignalImmediateFlush();
                EvaluateMetaAchievement(achievement->m_achievementId);
            }
        }
    }
}



























































inline void AchievementsEvaluator::EvaluateMetaAchievement(AchievementId achievementId) {
    const uint32 EARNED_ACHIEVEMENT = 200;
    CheckEvent(EARNED_ACHIEVEMENT, achievementId, 1, 0, -1, CheckEventCallback());
}






inline uint64 AchievementsEvaluator::GetSeconds() const {
    return OnlineServiceServer::GetCurrentUnixTime();
}






}  // namespace AchievementsEngine
}  // namespace OnlineService
