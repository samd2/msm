// Copyright 2010 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// eUML2 stt grammar
#define BOOST_MPL_LIMIT_STRING_SIZE 55
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/euml2.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

namespace
{
    // base class for states is not necessary but makes test easier
    struct counting_base_state
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {++entry_counter;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {++exit_counter;}
        int entry_counter=0;
        int exit_counter=0;
    };
    // front-end: define the FSM structure
    struct my_machine_ : public msm::front::state_machine_def<my_machine_,counting_base_state>
    {
        unsigned int state2_to_state3_counter;
        unsigned int state3_to_state4_counter;
        unsigned int always_true_counter;
        unsigned int always_false_counter;

        my_machine_():
        state2_to_state3_counter(0),
        state3_to_state4_counter(0),
        always_true_counter(0),
        always_false_counter(0)
        {}

        // the initial state of the player SM. Must be defined
        using initial_state = BOOST_MSM_EUML2_STATE("State1",my_machine_) ;

        // Transition table
        EUML2_STT(
            my_machine_,
            EUML2_STT_CFG(),
            //     +---------------------------------------------------------------------------------------+
            EUML2_ROW("State1 -> State2"),
            EUML2_ROW("State2 / State2ToState3 -> State3"),
            EUML2_ROW("State3 [always_true] / State3ToState4 -> State4"),
            EUML2_ROW("State3 [always_false] -> State4"),
            EUML2_ROW("State4 + * -> State1")
            //     +---------------------------------------------------------------------------------------+
        )
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const&, FSM&,int)
        {
            BOOST_ERROR("no_transition called!");
        }
    };
    // Pick a back-end
    typedef msm::back::state_machine<my_machine_> my_machine;
}

// transition actions (the standard way)
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("State2ToState3",my_machine_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.state2_to_state3_counter;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("State3ToState4",my_machine_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.state3_to_state4_counter;
}

// guard conditions (the standard way)
// always_true need not be defined (true by default) but we need to update counter
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("always_true",my_machine_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.always_true_counter;
    return true;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("always_false",my_machine_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.always_false_counter;
    return false;
}


BOOST_AUTO_TEST_CASE( test_euml2_anonymous_kleene )
{
    my_machine p;

    p.start();
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 3,"State4 should be active"); //State4
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State1",my_machine_)&>().exit_counter == 1,"State1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State1",my_machine_)&>().entry_counter == 1,"State1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State2",my_machine_)&>().exit_counter == 1,"State2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State2",my_machine_)&>().entry_counter == 1,"State2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State3",my_machine_)&>().exit_counter == 1,"State3 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State3",my_machine_)&>().entry_counter == 1,"State3 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State4",my_machine_)&>().entry_counter == 1,"State4 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.always_true_counter == 1,"guard not called correctly");
    BOOST_CHECK_MESSAGE(p.always_false_counter == 1,"guard not called correctly");
    BOOST_CHECK_MESSAGE(p.state2_to_state3_counter == 1,"action not called correctly");
    BOOST_CHECK_MESSAGE(p.state3_to_state4_counter == 1,"action not called correctly");

    // this event, will bring us back to the initial state and thus, a new "loop" will be started
    // this sill use the kleen (*) transition. event1 is "any" event.
    p.process_event(BOOST_MSM_EUML2_EVENT("event1",my_machine_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 3,"State4 should be active"); //State4
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State1",my_machine_)&>().exit_counter == 2,"State1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State1",my_machine_)&>().entry_counter == 2,"State1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State2",my_machine_)&>().exit_counter == 2,"State2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State2",my_machine_)&>().entry_counter == 2,"State2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State3",my_machine_)&>().exit_counter == 2,"State3 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State3",my_machine_)&>().entry_counter == 2,"State3 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State4",my_machine_)&>().entry_counter == 2,"State4 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("State4",my_machine_)&>().exit_counter == 1,"State4 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.always_true_counter == 2,"guard not called correctly");
    BOOST_CHECK_MESSAGE(p.always_false_counter == 2,"guard not called correctly");
    BOOST_CHECK_MESSAGE(p.state2_to_state3_counter == 2,"action not called correctly");
    BOOST_CHECK_MESSAGE(p.state3_to_state4_counter == 2,"action not called correctly");
}




