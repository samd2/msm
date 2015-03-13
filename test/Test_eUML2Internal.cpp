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
#define BOOST_MPL_LIMIT_STRING_SIZE 80
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/euml2.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

namespace
{
    // events
    // A "complicated" event type that carries some data.
    enum DiskTypeEnum
    {
        DISK_CD=0,
        DISK_DVD=1
    };
    struct cd_detected
    {
        cd_detected(std::string name, DiskTypeEnum diskType)
            : name(name),
            disc_type(diskType)
        {}

        std::string name;
        DiskTypeEnum disc_type;
    };

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
    struct player_ : public msm::front::state_machine_def<player_,counting_base_state>
    {
        unsigned int start_playback_counter;
        unsigned int can_close_drawer_counter;
        unsigned int internal_action_counter;
        unsigned int internal_guard_counter;

        player_():
        start_playback_counter(0),
        can_close_drawer_counter(0),
        internal_action_counter(0),
        internal_guard_counter(0)
        {}

        // a special state with its own internal stt
        struct Empty : public msm::front::state<counting_base_state>
        {
            using counting_base_state::on_entry;
            using counting_base_state::on_exit;
            unsigned int empty_internal_guard_counter=0;
            unsigned int empty_internal_action_counter=0;
            struct internal_guard_fct
            {
                typedef internal_guard_fct type;
                template <class EVT,class FSM,class SourceState,class TargetState>
                bool operator()(EVT const& ,FSM&,SourceState& src,TargetState& )
                {
                    ++src.empty_internal_guard_counter;
                    return false;
                }
            };
            struct internal_action_fct
            {
                typedef internal_action_fct type;
                template <class EVT,class FSM,class SourceState,class TargetState>
                void operator()(EVT const& ,FSM& ,SourceState& src,TargetState& )
                {
                    ++src.empty_internal_action_counter;
                }
            };
            // Transition table for Empty
            EUML2_STT_INTERNAL(
                player_,
                // list here the names for which we want to provide our own type, not the ones from eUML2
                // (for which an implementation is provided below)
                EUML2_STT_CFG(EUML2_STT_USE("internal_guard_fct",internal_guard_fct),
                              EUML2_STT_USE("internal_action_fct",internal_action_fct)),
                // +---------------------------------------------------------------------------------------+
                EUML2_INTERNAL("internal_evt [internal_guard_fct] / internal_action_fct")
                // +---------------------------------------------------------------------------------------+
            )
        };

        // the initial state of the player SM. Must be defined
        using initial_state = Empty;

        // Transition table for player
        EUML2_STT(
            player_,
            // list here the names for which we want to provide our own type, not the ones from eUML2
            // (for which an implementation is provided below)
            EUML2_STT_CFG(EUML2_STT_USE("cd_detected",cd_detected),
                          EUML2_STT_USE("Empty",Empty)),
            //     +---------------------------------------------------------------------------------------+
            EUML2_ROW("Stopped + play        [dummy]     / start_pback   -> Playing"),
            EUML2_ROW("Stopped + open_close              / open_drawer   -> Open"),
            EUML2_ROW("Stopped + stop                                    -> Stopped"),
            EUML2_ROW("Open    + open_close  [can_close]                 -> Empty"),
            EUML2_ROW("Empty   + open_close              / open_drawer   -> Open"),
            EUML2_ROW("Empty   + cd_detected [good_disk] / store_cd      -> Stopped"),
            EUML2_ROW("Empty   + internal_evt [internal_guard2] / internal_action"),
            EUML2_ROW("Empty   + to_ignore"),
            EUML2_ROW("Empty   + cd_detected [internal_guard]"),
            EUML2_ROW("Playing + stop                    / stop_pback    -> Stopped"),
            EUML2_ROW("Playing + pause                   / pause_pback   -> Paused"),
            EUML2_ROW("Playing + open_close              / stop_and_open -> Open"),
            EUML2_ROW("Paused  + end_pause               / resume_pback  -> Playing"),
            EUML2_ROW("Paused  + stop                    / stop_pback    -> Stopped"),
            EUML2_ROW("Paused  + open_close              / stop_and_open -> Open")
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
    typedef msm::back::state_machine<player_> player;
}

// transition actions (the standard way)
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("start_pback",player_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.start_playback_counter;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("internal_action",player_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.internal_action_counter;
}

// guard conditions (the standard way)
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("good_disk",player_)::operator()(Event const& evt, Fsm&,SourceState& ,TargetState& )
{
    // to test a guard condition, let's say we understand only CDs, not DVD
    if (evt.disc_type != DISK_CD)
    {
        return false;
    }
    return true;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("can_close",player_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.can_close_drawer_counter;
    return true;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("internal_guard",player_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.internal_guard_counter;
    return false;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("internal_guard2",player_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    ++fsm.internal_guard_counter;
    return true;
}

BOOST_AUTO_TEST_CASE( test_euml2_internal )
{
    player p;

    p.start();
    BOOST_CHECK_MESSAGE(p.get_state<player_::Empty&>().entry_counter == 1,"Empty entry not called correctly");
    // internal events
    p.process_event(BOOST_MSM_EUML2_EVENT("to_ignore",player_)());
    p.process_event(BOOST_MSM_EUML2_EVENT("internal_evt",player_)());
    BOOST_CHECK_MESSAGE(p.internal_action_counter == 1,"Internal action not called correctly");
    BOOST_CHECK_MESSAGE(p.internal_guard_counter == 1,"Internal guard not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<player_::Empty&>().empty_internal_action_counter == 0,"Empty internal action not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<player_::Empty&>().empty_internal_guard_counter == 1,"Empty internal guard not called correctly");

    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 1,"Open should be active"); //Open
    BOOST_CHECK_MESSAGE(p.get_state<player_::Empty&>().exit_counter == 1,"Empty exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Open",player_)&>().entry_counter == 1,"Open entry not called correctly");

    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 2,"Empty should be active"); //Empty
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Open",player_)&>().exit_counter == 1,"Open exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<player_::Empty&>().entry_counter == 2,"Empty entry not called correctly");
    BOOST_CHECK_MESSAGE(p.can_close_drawer_counter == 1,"guard not called correctly");

    p.process_event(
        cd_detected("louie, louie",DISK_DVD));
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 2,"Empty should be active"); //Empty
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Open",player_)&>().exit_counter == 1,"Open exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<player_::Empty&>().entry_counter == 2,"Empty entry not called correctly");
    BOOST_CHECK_MESSAGE(p.internal_guard_counter == 2,"Internal guard not called correctly");

    p.process_event(
        cd_detected("louie, louie",DISK_CD));
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 0,"Stopped should be active"); //Stopped
    BOOST_CHECK_MESSAGE(p.get_state<player_::Empty&>().exit_counter == 2,"Empty exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Stopped",player_)&>().entry_counter == 1,"Stopped entry not called correctly");
    BOOST_CHECK_MESSAGE(p.internal_guard_counter == 3,"Internal guard not called correctly");

    p.process_event(BOOST_MSM_EUML2_EVENT("play",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 3,"Playing should be active"); //Playing
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Stopped",player_)&>().exit_counter == 1,"Stopped exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Playing",player_)&>().entry_counter == 1,"Playing entry not called correctly");
    BOOST_CHECK_MESSAGE(p.start_playback_counter == 1,"action not called correctly");

    p.process_event(BOOST_MSM_EUML2_EVENT("pause",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 4,"Paused should be active"); //Paused
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Playing",player_)&>().exit_counter == 1,"Playing exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Paused",player_)&>().entry_counter == 1,"Paused entry not called correctly");

    // go back to Playing
    p.process_event(BOOST_MSM_EUML2_EVENT("end_pause",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 3,"Playing should be active"); //Playing
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Paused",player_)&>().exit_counter == 1,"Paused exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Playing",player_)&>().entry_counter == 2,"Playing entry not called correctly");

    p.process_event(BOOST_MSM_EUML2_EVENT("pause",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 4,"Paused should be active"); //Paused
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Playing",player_)&>().exit_counter == 2,"Playing exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Paused",player_)&>().entry_counter == 2,"Paused entry not called correctly");

    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 0,"Stopped should be active"); //Stopped
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Paused",player_)&>().exit_counter == 2,"Paused exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Stopped",player_)&>().entry_counter == 2,"Stopped entry not called correctly");

    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)());
    BOOST_CHECK_MESSAGE(p.current_state()[0] == 0,"Stopped should be active"); //Stopped
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Stopped",player_)&>().exit_counter == 2,"Stopped exit not called correctly");
    BOOST_CHECK_MESSAGE(p.get_state<BOOST_MSM_EUML2_STATE_IMPL("Stopped",player_)&>().entry_counter == 3,"Stopped entry not called correctly");
}

