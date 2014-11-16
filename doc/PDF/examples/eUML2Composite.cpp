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

namespace msm = boost::msm;

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

    struct logging_base_state
    {
        template <class Event, class Fsm>
        void on_entry(Event const&, Fsm&)
        {
            std::cout  << name() << " on_entry" << std::endl;
        }
        template <class Event, class Fsm>
        void on_exit(Event const&, Fsm&)
        {
            std::cout << name() << " on_exit"  << std::endl;
        }
        virtual std::string name() const {return "";}
    };

    // submachine
    struct Playing_ : public msm::front::state_machine_def<Playing_,logging_base_state>
    {
        // we need to explicitly use base's entry/exit and provide name
        using logging_base_state::on_entry;
        using logging_base_state::on_exit;
        std::string name() const {return "Playing";}

        // the initial state of the player SM. Must be defined
        using initial_state = BOOST_MSM_EUML2_STATE("Song1",Playing_) ;
        // Transition table for Playing
        EUML2_STT(
            Playing_,
            EUML2_STT_CFG(),
            //     +---------------------------------------------------------------------------------------+
            EUML2_ROW("Song1 + next_song  -> Song2"),
            EUML2_ROW("Song2 + prev_song  -> Song1"),
            EUML2_ROW("Song2 + next_song  -> Song3"),
            EUML2_ROW("Song3 + prev_song  -> Song2")
            //     +---------------------------------------------------------------------------------------+
        )
    };
    // back-end
    typedef msm::back::state_machine<Playing_> Playing;

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_,logging_base_state>
    {
        // the initial state of the player SM. Must be defined
        using initial_state = BOOST_MSM_EUML2_STATE("Empty",player_) ;

        // Transition table for player
        EUML2_STT(
            player_,
            // list here the names for which we want to provide our own type, not the ones from eUML2
            // (for which an implementation is provided below)
            EUML2_STT_CFG(EUML2_STT_USE("cd_detected",cd_detected),EUML2_STT_USE("Playing",Playing)),
            //     +---------------------------------------------------------------------------------------+
            EUML2_ROW("Stopped + play        [dummy] / start_pback,test_fct -> Playing"),
            EUML2_ROW("Stopped + open_close              / open_drawer   -> Open"),
            EUML2_ROW("Stopped + stop                                    -> Stopped"),
            EUML2_ROW("Open    + open_close  [can_close] / close_drawer  -> Empty"),
            EUML2_ROW("Empty   + open_close              / open_drawer   -> Open"),
            EUML2_ROW("Empty   + cd_detected [good_disk && always_true] / store_cd -> Stopped"),
            EUML2_ROW("Playing + stop                    / stop_pback    -> Stopped"),
            EUML2_ROW("Playing + pause                   / pause_pback   -> Paused"),
            EUML2_ROW("Playing + open_close              / stop_and_open -> Open"),
            EUML2_ROW("Paused  + end_pause               / resume_pback  -> Playing"),
            EUML2_ROW("Paused  + stop                    / stop_pback    -> Stopped"),
            EUML2_ROW("Paused  + open_close              / stop_and_open -> Open")
            //     +---------------------------------------------------------------------------------------+
        )
    };
    // Pick a back-end
    typedef msm::back::state_machine<player_> player;
    // Testing utilities (now easier than before using name()
    void pstate(player const& p )
    {
        // we can ask the state its name, easier!
        std::cout << "active state -> " << p.get_state_by_id(p.current_state()[0])->name() << std::endl;
    }
}

// transition actions (the standard way)
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("start_pback",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "calling start_pback" << std::endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("store_cd",player_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    std::cout << "calling store_cd" << std::endl;
    fsm.process_event(BOOST_MSM_EUML2_EVENT("play",player_)());
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("test_fct",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "calling test_fct" << std::endl;
}
// guard conditions (the standard way)
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("good_disk",player_)::operator()(Event const& evt, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "calling good_disk" << std::endl;
    // to test a guard condition, let's say we understand only CDs, not DVD
    if (evt.disc_type != DISK_CD)
    {
        return false;
    }
    return true;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("can_close",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "calling can_close" << std::endl;
    return true;
}

void test()
{
    player p;
    // Empty is active
    p.start();pstate(p);
    // move To Open
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)());pstate(p);
    // Empty is active
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)());pstate(p);
    // DVD is not supported => guard rejects
    p.process_event(
        cd_detected("louie, louie",DISK_DVD));pstate(p);
    // CD is supported => Stopped then Playing
    p.process_event(
        cd_detected("louie, louie",DISK_CD));pstate(p);
    // Playing
    p.process_event(BOOST_MSM_EUML2_EVENT("next_song",Playing_)());pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("next_song",Playing_)());pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("prev_song",Playing_)());pstate(p);
    // Pause
    p.process_event(BOOST_MSM_EUML2_EVENT("pause",player_)());pstate(p);
    // go back to Playing
    p.process_event(BOOST_MSM_EUML2_EVENT("end_pause",player_)());pstate(p);
    // Pause
    p.process_event(BOOST_MSM_EUML2_EVENT("pause",player_)());pstate(p);
    // Stopped
    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)());pstate(p);
    // Stopped
    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)());pstate(p);
}

int main()
{
    test();
    return 0;
}


