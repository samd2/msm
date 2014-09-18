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
#define BOOST_MPL_LIMIT_STRING_SIZE 65
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/stt_grammar.hpp>

#include <boost/test/unit_test.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace
{
    // events
    struct event1 {};
    struct event2 {};
    struct event3 {};
    struct event4 {};
    struct event6
    {
        event6(){}
        template <class Event>
        event6(Event const&){}
    };
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
    // submachine
    struct SubFsm2_ : public msm::front::state_machine_def<SubFsm2_,counting_base_state>
    {
        typedef msm::back::state_machine<SubFsm2_> SubFsm2;

        unsigned int entry_action_counter=0;

        using counting_base_state::on_entry;
        using counting_base_state::on_exit;

        struct SubState2 : public msm::front::state<counting_base_state> , public msm::front::explicit_entry<0>
        {
            using counting_base_state::on_entry;
            using counting_base_state::on_exit;
        };
        struct SubState2b : public msm::front::state<counting_base_state> , public msm::front::explicit_entry<1>
        {
            using counting_base_state::on_entry;
            using counting_base_state::on_exit;
        };
        // test with a pseudo entry
        struct PseudoEntry1 : public msm::front::entry_pseudo_state<0,counting_base_state>
        {
            using counting_base_state::on_entry;
            using counting_base_state::on_exit;
        };

        struct PseudoExit1 : public msm::front::exit_pseudo_state<event6,counting_base_state>
        {
            using counting_base_state::on_entry;
            using counting_base_state::on_exit;
        };
        // the initial state. Must be defined
        typedef mpl::vector<BOOST_MSM_EUML2_STATE("SubState1",SubFsm2_),BOOST_MSM_EUML2_STATE("SubState1b",SubFsm2_)> initial_state;

        typedef mpl::vector<SubState2b> explicit_creation;

        // Transition table for SubFsm2
        EUML2_STT(
            SubFsm2_,
            EUML2_STT_CFG(EUML2_STT_USE("SubState2",SubState2),
                          EUML2_STT_USE("SubState2b",SubState2b),
                          EUML2_STT_USE("PseudoEntry1",PseudoEntry1),
                          EUML2_STT_USE("PseudoExit1",PseudoExit1),
                          EUML2_STT_USE("event6",event6),
                          EUML2_STT_USE("event4",event4)),
            //     +---------------------------------------------------------------------------------------+
            EUML2_ROW("PseudoEntry1 + event4 / entry_action -> SubState3"),
            EUML2_ROW("SubState2    + event6                -> SubState1"),
            EUML2_ROW("SubState3    + event5                -> PseudoExit1")
            //     +---------------------------------------------------------------------------------------+
        )
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const&, FSM&,int)
        {
            BOOST_FAIL("no_transition called!");
        }
    };
    typedef msm::back::state_machine<SubFsm2_> SubFsm2;
    // front-end: define the FSM structure
    struct Fsm_ : public msm::front::state_machine_def<Fsm_,counting_base_state>
    {

        // The list of FSM states
        struct State1 : public msm::front::state<counting_base_state>
        {
            using counting_base_state::on_entry;
            using counting_base_state::on_exit;
        };
        struct State2 : public msm::front::state<counting_base_state>
        {
            using counting_base_state::on_entry;
            using counting_base_state::on_exit;
        };
        // the initial state of the player SM. Must be defined
        using initial_state = State1 ;

        // Transition table for Fsm
        struct transition_table : mpl::vector<
            //    Start                 Event    Next                                 Action  Guard
            //   +---------------------+--------+------------------------------------+-------+--------+
            _row < State1              , event1 , SubFsm2                                             >,
            _row < State1              , event2 , SubFsm2::direct<SubFsm2_::SubState2>                >,
            _row < State1              , event3 , mpl::vector<SubFsm2::direct<SubFsm2_::SubState2>,
                                                              SubFsm2::direct<SubFsm2_::SubState2b> > >,
            _row < State1              , event4 , SubFsm2::entry_pt
                                                        <SubFsm2_::PseudoEntry1>                      >,
            //   +---------------------+--------+------------------------------------+-------+--------+
            _row < SubFsm2             , event1 , State1                                              >,
            _row < SubFsm2::exit_pt
                <SubFsm2_::PseudoExit1>, event6 , State2                                              >
            //   +---------------------+--------+------------------------------------+-------+--------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const&, FSM&,int)
        {
            BOOST_ERROR("no_transition called!");
        }
    };
    typedef msm::back::state_machine<Fsm_> Fsm;
}
// transition actions (the standard way)
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("entry_action",SubFsm2_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.entry_action_counter;
}

BOOST_AUTO_TEST_CASE( my_euml2_entries )
{
    Fsm p;

    p.start();
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().entry_counter == 1,"State1 entry not called correctly");

    p.process_event(event1());
    p.process_event(event1());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 0,"State1 should be active");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().exit_counter == 1,"State1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().entry_counter == 2,"State1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().exit_counter == 1,"SubFsm2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().entry_counter == 1,"SubFsm2 entry not called correctly");

    p.process_event(event2());
    p.process_event(event6());
    p.process_event(event1());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 0,"State1 should be active");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().exit_counter == 2,"State1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().entry_counter == 3,"State1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().exit_counter == 2,"SubFsm2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().entry_counter == 2,"SubFsm2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::SubState2&>().entry_counter == 1,
                        "SubFsm2::SubState2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::SubState2&>().exit_counter == 1,
                        "SubFsm2::SubState2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<BOOST_MSM_EUML2_STATE("SubState1",SubFsm2_)&>().entry_counter == 2,
                        "SubFsm2::SubState1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<BOOST_MSM_EUML2_STATE("SubState1",SubFsm2_)&>().exit_counter == 2,
                        "SubFsm2::SubState1 exit not called correctly");

    p.process_event(event3());
    p.process_event(event1());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 0,"State1 should be active");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().exit_counter == 3,"State1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().entry_counter == 4,"State1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().exit_counter == 3,"SubFsm2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().entry_counter == 3,"SubFsm2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::SubState2&>().entry_counter == 2,
                        "SubFsm2::SubState2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::SubState2&>().exit_counter == 2,
                        "SubFsm2::SubState2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::SubState2b&>().entry_counter == 1,
                        "SubFsm2::SubState2b entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::SubState2b&>().exit_counter == 1,
                        "SubFsm2::SubState2b exit not called correctly");

    p.process_event(event4());
    p.process_event(BOOST_MSM_EUML2_EVENT("event5",SubFsm2_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 2,"State2 should be active");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State1&>().exit_counter == 4,"State1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<Fsm_::State2&>().entry_counter == 1,"State2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().exit_counter == 4,"SubFsm2 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().entry_counter == 4,"SubFsm2 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::PseudoEntry1&>().entry_counter == 1,
                        "SubFsm2::PseudoEntry1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2_::PseudoEntry1&>().exit_counter == 1,
                        "SubFsm2::PseudoEntry1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<BOOST_MSM_EUML2_STATE("SubState3",SubFsm2_)&>().entry_counter == 1,
                        "SubFsm2::SubState3 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<BOOST_MSM_EUML2_STATE("SubState3",SubFsm2_)&>().exit_counter == 1,
                        "SubFsm2::SubState3 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2::exit_pt<SubFsm2_::PseudoExit1>&>().entry_counter == 1,
                        "SubFsm2::PseudoExit1 entry not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().get_state<SubFsm2::exit_pt<SubFsm2_::PseudoExit1>&>().exit_counter == 1,
                        "SubFsm2::PseudoExit1 exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<SubFsm2&>().entry_action_counter == 1,"Action not called correctly");

}



