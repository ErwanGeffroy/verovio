/////////////////////////////////////////////////////////////////////////////
// Name:        attcomparison.h
// Author:      Laurent Pugin
// Created:     2015
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_ATT_COMPARISON_H__
#define __VRV_ATT_COMPARISON_H__

#include "artic.h"
#include "atts_shared.h"
#include "durationinterface.h"
#include "horizontalaligner.h"
#include "note.h"
#include "object.h"

namespace vrv {

enum DurExtreme { LONGEST = 0, SHORTEST };

//----------------------------------------------------------------------------
// IsAttributeComparison
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object is of a certain ClassId and is an attribute in the original MEI.
 */
class IsAttributeComparison : public AttComparison {

public:
    IsAttributeComparison(ClassId AttClassId) : AttComparison(AttClassId) {}

    virtual bool operator()(Object *object)
    {
        if (!MatchesType(object)) return false;
        if (object->IsAttribute()) return true;
        return false;
    }
};

//----------------------------------------------------------------------------
// AttCommonNComparison
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object is of a certain ClassId and has a @n of value n.
 */
class AttCommonNComparison : public AttComparison {

public:
    AttCommonNComparison(ClassId AttClassId, const int n) : AttComparison(AttClassId) { m_n = n; }

    void SetN(int n) { m_n = n; }

    virtual bool operator()(Object *object)
    {
        if (!MatchesType(object)) return false;
        // This should not happen, but just in case
        if (!object->HasAttClass(ATT_COMMON)) return false;
        AttCommon *element = dynamic_cast<AttCommon *>(object);
        assert(element);
        return (element->GetN() == m_n);
    }

private:
    int m_n;
};

//----------------------------------------------------------------------------
// AttCommonNComparisonAny
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object is of a certain ClassId and has a @n of value n.
 */
class AttCommonNComparisonAny : public AttComparison {

public:
    AttCommonNComparisonAny(ClassId AttClassId, std::vector<int> ns) : AttComparison(AttClassId) { m_ns = ns; }

    void SetNs(std::vector<int> ns) { m_ns = ns; }

    virtual bool operator()(Object *object)
    {
        if (!MatchesType(object)) return false;
        // This should not happen, but just in case
        if (!object->HasAttClass(ATT_COMMON)) return false;
        AttCommon *element = dynamic_cast<AttCommon *>(object);
        assert(element);
        return (std::find(m_ns.begin(), m_ns.end(), element->GetN()) != m_ns.end());
    }

private:
    std::vector<int> m_ns;
};

//----------------------------------------------------------------------------
// AttComparisonAny
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object is of a certain ClassId and has a @n of value n.
 */
class AttComparisonAny : public AttComparison {

public:
    AttComparisonAny(std::vector<ClassId> classIds) : AttComparison(OBJECT) { m_classIds = classIds; }

    void SetClassIds(std::vector<ClassId> classIds) { m_classIds = classIds; }

    virtual bool operator()(Object *object)
    {
        ClassId classId = object->GetClassId();
        return (std::find(m_classIds.begin(), m_classIds.end(), classId) != m_classIds.end());
    }

private:
    std::vector<ClassId> m_classIds;
};

//----------------------------------------------------------------------------
// AttDurExtreme
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object the extreme duration so far
 * The object has to have a DurationInterface and to have a @dur.
 * The class can look for LONGEST or SHORTEST duration (Constructor)
 */
class AttDurExtreme : public AttComparison {

public:
    AttDurExtreme(DurExtreme extremeType) : AttComparison(OBJECT)
    {
        m_extremeType = extremeType;
        if (m_extremeType == LONGEST)
            m_extremeDur = -VRV_UNSET;
        else
            m_extremeDur = VRV_UNSET;
    }

    virtual bool operator()(Object *object)
    {
        if (!object->HasInterface(INTERFACE_DURATION)) return false;
        DurationInterface *interface = dynamic_cast<DurationInterface *>(object);
        assert(interface);
        if (interface->HasDur()) {
            if ((m_extremeType == LONGEST) && (interface->GetActualDur() < m_extremeDur)) {
                m_extremeDur = interface->GetActualDur();
                return true;
            }
            else if ((m_extremeType == SHORTEST) && (interface->GetActualDur() > m_extremeDur)) {
                m_extremeDur = interface->GetActualDur();
                return true;
            }
        }
        return false;
    }

private:
    int m_extremeDur;
    DurExtreme m_extremeType;
};

//----------------------------------------------------------------------------
// ArticPartTypeComparison
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object is an Alignment of a certain type
 */
class ArticPartTypeComparison : public AttComparison {

public:
    ArticPartTypeComparison(const ArticPartType type) : AttComparison(OBJECT) { m_type = type; }

    void SetType(ArticPartType type) { m_type = type; }

    virtual bool operator()(Object *object)
    {
        ArticPart *articPart = dynamic_cast<ArticPart *>(object);
        if (!articPart) return false;
        return (articPart->GetType() == m_type);
    }

private:
    ArticPartType m_type;
};

//----------------------------------------------------------------------------
// MeasureAlignerTypeComparison
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object is an Alignment of a certain type
 */
class MeasureAlignerTypeComparison : public AttComparison {

public:
    MeasureAlignerTypeComparison(const AlignmentType type) : AttComparison(OBJECT) { m_type = type; }

    void SetType(AlignmentType type) { m_type = type; }

    virtual bool operator()(Object *object)
    {
        Alignment *alignment = dynamic_cast<Alignment *>(object);
        if (!alignment) return false;
        return (alignment->GetType() == m_type);
    }

private:
    AlignmentType m_type;
};

//----------------------------------------------------------------------------
// NoteOnsetOffsetComparison
//----------------------------------------------------------------------------

/**
 * This class evaluates if the object is of a certain ClassId and has a @n of value n.
 */
class NoteOnsetOffsetComparison : public AttComparison {

public:
    NoteOnsetOffsetComparison(const double time) : AttComparison(NOTE) { m_time = time; }

    void SetTime(int time) { m_time = time; }

    virtual bool operator()(Object *object)
    {
        if (!MatchesType(object)) return false;
        // This should not happen, but just in case
        if (!object->Is(NOTE)) return false;
        Note *note = dynamic_cast<Note *>(object);
        assert(note);
        return ((note->m_playingOnset < m_time) && (note->m_playingOffset > m_time));
    }

private:
    double m_time;
};

} // namespace vrv

#endif
