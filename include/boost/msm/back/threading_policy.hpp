// Copyright 2014 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_BACK_THREADING_POLICY_HPP
#define BOOST_MSM_BACK_THREADING_POLICY_HPP

#include <atomic>
#include <boost/msm/back/common_types.hpp>

namespace boost { namespace msm { namespace back
{
struct thread_unsafe_policy
{
    typedef int threading_policy;
    typedef int state_index_type;
    typedef typename ::boost::mpl::false_ retry_after_missed_update;
    //TODO remove temporary
    //typedef std::atomic<int> state_index_type;
    static void assign(state_index_type& lhs,state_index_type const& rhs)
    {
        lhs = rhs;
        //lhs = rhs.load();
    }
    static void check_current_state(int current_state,int theoretical_current_state)
    {
        BOOST_ASSERT(current_state == theoretical_current_state);
    }

    template <class ActiveStateSwitching>
    static HandledEnum after_guard(int current_state,int next_state,state_index_type& fsm_state)
    {
        fsm_state = ActiveStateSwitching::after_guard(current_state,next_state);
        return HANDLED_TRUE;
    }
    template <class ActiveStateSwitching>
    static HandledEnum after_exit(int current_state,int next_state,state_index_type& fsm_state)
    {
        fsm_state = ActiveStateSwitching::after_exit(current_state,next_state);
        return HANDLED_TRUE;
    }
    template <class ActiveStateSwitching>
    static HandledEnum after_action(int current_state,int next_state,state_index_type& fsm_state)
    {
        fsm_state = ActiveStateSwitching::after_action(current_state,next_state);
        return HANDLED_TRUE;
    }
    template <class ActiveStateSwitching>
    static HandledEnum after_entry(int current_state,int next_state,state_index_type& fsm_state)
    {
        fsm_state = ActiveStateSwitching::after_entry(current_state,next_state);
        return HANDLED_TRUE;
    }
};

// for lockfree_policy, implements exchange_policy using a atomic::compare_exchange_weak
struct weak_exchange
{
    template<class AtomicType>
    static
    bool exchange(std::atomic<AtomicType>& fsm_state,AtomicType temp_current_state,AtomicType next_state)
    {
        return fsm_state.compare_exchange_weak(temp_current_state,next_state);
    }
};
// for lockfree_policy, implements exchange_policy using a atomic::compare_exchange_strong
struct strong_exchange
{
    template<class AtomicType>
    static
    bool exchange(std::atomic<AtomicType>& fsm_state,AtomicType temp_current_state,AtomicType next_state)
    {
        return fsm_state.compare_exchange_strong(temp_current_state,next_state);
    }
};

// for lockfree_policy, implements retry_policy, always forces retry if state switching fails
struct always_retry
{
    typedef ::boost::mpl::true_ type;
};
// for lockfree_policy, implements retry_policy, never forces retry if state switching fails
struct never_retry
{
    typedef ::boost::mpl::false_ type;
};

template <class exchange_policy = boost::msm::back::strong_exchange, class retry_policy= boost::msm::back::always_retry>
struct lockfree_policy
{
    typedef int threading_policy;
    typedef typename retry_policy::type retry_after_missed_update;

    typedef std::atomic<int> state_index_type;
    static void assign(state_index_type& lhs,state_index_type const& rhs)
    {
        lhs = rhs.load();
    }
    static void check_current_state(int ,int )
    {
    }
    // this policy ignores user's wish for state switching point and replaces it with after_guard
    template <class ActiveStateSwitching>
    static HandledEnum after_guard(int current_state,int next_state,state_index_type& fsm_state)
    {
        int temp_current_state= current_state;
        bool exchanged = exchange_policy::exchange(fsm_state,temp_current_state,next_state);
        if (!exchanged)
        {
            return HANDLED_REJECT_MISSED_UPDATE;
        }
        return HANDLED_TRUE;
    }
    template <class ActiveStateSwitching>
    static HandledEnum after_exit(int ,int ,state_index_type& )
    {
        return HANDLED_TRUE;
    }
    template <class ActiveStateSwitching>
    static HandledEnum after_action(int ,int ,state_index_type& )
    {
        return HANDLED_TRUE;
    }
    template <class ActiveStateSwitching>
    static HandledEnum after_entry(int ,int ,state_index_type& )
    {
        return HANDLED_TRUE;
    }
};

} } }//boost::msm::back
#endif // BOOST_MSM_BACK_THREADING_POLICY_HPP
