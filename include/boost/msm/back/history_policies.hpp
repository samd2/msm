// Copyright 2008 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_BACK_HISTORY_POLICIES_H
#define BOOST_MSM_BACK_HISTORY_POLICIES_H

#include <boost/mpl/contains.hpp>

namespace boost { namespace msm { namespace back
{

// policy classes

// Default: no history used
template <int NumberOfRegions, class ThreadingPolicy>
class NoHistoryImpl
{
public:
    NoHistoryImpl(){}
    ~NoHistoryImpl(){}
    void set_initial_states(typename ThreadingPolicy::state_index_type* const initial_states)
    {
        for (int i=0;i<NumberOfRegions;++i)
        {
            ThreadingPolicy::assign(m_initialStates[i], initial_states[i]);
        }
    }
    void history_exit(typename ThreadingPolicy::state_index_type* const )
    {
        // ignore
    }
    // returns the state where the state machine should be at start
    template <class Event>
    const typename ThreadingPolicy::state_index_type* history_entry(Event const& )
    {
        // always come back to the original state
        return m_initialStates;
    }
    NoHistoryImpl<NumberOfRegions,ThreadingPolicy>& operator=(NoHistoryImpl<NumberOfRegions,ThreadingPolicy> const& rhs)
    {
         for (int i=0; i<NumberOfRegions;++i)
         {
             ThreadingPolicy::assign(m_initialStates[i] , rhs.m_initialStates[i]);
         }
         return *this;
    }
    // this policy deletes all waiting deferred events
    template <class Event>
    bool process_deferred_events(Event const&)const
    {
        return false;
    }
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_initialStates;
    }
private:
    typename ThreadingPolicy::state_index_type m_initialStates[NumberOfRegions];
};

// not UML standard. Always activates history, no matter which event generated the transition
template <int NumberOfRegions, class ThreadingPolicy>
class AlwaysHistoryImpl
{
public:
    AlwaysHistoryImpl(){}
    ~AlwaysHistoryImpl(){}
    void set_initial_states(typename ThreadingPolicy::state_index_type* const initial_states)
    {
        for (int i=0;i<NumberOfRegions;++i)
        {
            ThreadingPolicy::assign(m_initialStates[i], initial_states[i]);
        }
    }
    void history_exit(typename ThreadingPolicy::state_index_type* const current_states)
    {
        for (int i=0;i<NumberOfRegions;++i)
        {
            ThreadingPolicy::assign(m_initialStates[i] , current_states[i]);
        }
    }
    // returns the state where the state machine should be at start
    template <class Event>
    const typename ThreadingPolicy::state_index_type* history_entry(Event const& )
    {
        // always load back the last active state
        return m_initialStates;
    }
    AlwaysHistoryImpl<NumberOfRegions,ThreadingPolicy>& operator=(AlwaysHistoryImpl<NumberOfRegions,ThreadingPolicy> const& rhs)
    {
         for (int i=0; i<NumberOfRegions;++i)
         {
             ThreadingPolicy::assign(m_initialStates[i] , rhs.m_initialStates[i]);
         }
         return *this;
    }
    // the history policy keeps all deferred events until next reentry
    template <class Event>
    bool process_deferred_events(Event const&)const
    {
        return true;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_initialStates;
    }
private:
    typename ThreadingPolicy::state_index_type m_initialStates[NumberOfRegions];
};

// UML Shallow history. For deep history, just use this policy for all the contained state machines
template <class Events,int NumberOfRegions, class ThreadingPolicy>
class ShallowHistoryImpl
{
public:
    ShallowHistoryImpl(){}
    ~ShallowHistoryImpl(){}
    void set_initial_states(typename ThreadingPolicy::state_index_type* const initial_states)
    {
        for (int i=0;i<NumberOfRegions;++i)
        {
            ThreadingPolicy::assign(m_currentStates[i] , initial_states[i]);
            ThreadingPolicy::assign(m_initialStates[i] , initial_states[i]);
        }
    }
    void history_exit(typename ThreadingPolicy::state_index_type* const current_states)
    {
        for (int i=0;i<NumberOfRegions;++i)
            ThreadingPolicy::assign(m_currentStates[i] , current_states[i]);
    }
    // returns the state where the state machine should be at start
    template <class Event>
    const typename ThreadingPolicy::state_index_type* history_entry(Event const&)
    {
        if ( ::boost::mpl::contains<Events,Event>::value)
        {
            return m_currentStates;
        }
        // not one of our events, no history
        return m_initialStates;
    }
    ShallowHistoryImpl<Events,NumberOfRegions,ThreadingPolicy>& operator=(ShallowHistoryImpl<Events,NumberOfRegions,ThreadingPolicy> const& rhs)
    {
         for (int i=0; i<NumberOfRegions;++i)
         {
             ThreadingPolicy::assign(m_initialStates[i] , rhs.m_initialStates[i]);
             ThreadingPolicy::assign(m_currentStates[i] , rhs.m_currentStates[i]);
         }
         return *this;
    }
    // the history policy keeps deferred events until next reentry if coming from our history event
    template <class Event>
    bool process_deferred_events(Event const&)const
    {
        return ::boost::mpl::contains<Events,Event>::value;
    }
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & m_initialStates;
        ar & m_currentStates;
    }
private:
    typename ThreadingPolicy::state_index_type m_initialStates[NumberOfRegions];
    typename ThreadingPolicy::state_index_type m_currentStates[NumberOfRegions];
};

struct NoHistory
{
    typedef int history_policy;
    template <int NumberOfRegions, class ThreadingPolicy>
    struct apply
    {
        typedef NoHistoryImpl<NumberOfRegions,ThreadingPolicy> type;
    };
};
struct AlwaysHistory
{
    typedef int history_policy;
    template <int NumberOfRegions, class ThreadingPolicy>
    struct apply
    {
        typedef AlwaysHistoryImpl<NumberOfRegions,ThreadingPolicy> type;
    };
};
template <class Events>
struct ShallowHistory
{
    typedef int history_policy;
    template <int NumberOfRegions, class ThreadingPolicy>
    struct apply
    {
        typedef ShallowHistoryImpl<Events,NumberOfRegions,ThreadingPolicy> type;
    };
};
} } }//boost::msm::back
#endif //BOOST_MSM_BACK_HISTORY_POLICIES_H
