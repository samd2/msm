// Copyright 2015 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <iostream>

#ifdef TEST_FUNCTORS
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace std;
using namespace msm::front;

namespace
{
    // events
    struct play {};
    struct end_pause {};
    struct stop {};
    struct start_pause {};
    struct open_close {};
    struct cd_detected {};

    // The list of FSM states
    struct Empty : public msm::front::state<>
    {
        // every (optional) entry/exit methods get the event passed.
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Empty" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Empty" << std::endl;}
    };
    struct Open : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Open" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Open" << std::endl;}
    };

    struct Stopped : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Stopped" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Stopped" << std::endl;}
    };

    struct Playing : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Playing" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Playing" << std::endl;}
    };

    // state not defining any entry or exit
    struct Paused : public msm::front::state<>
    {
    };

    struct start_playback
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::start_playback" << endl;
        }
    };
    struct open_drawer
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::open_drawer" << endl;
        }
    };
    struct close_drawer
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::close_drawer" << endl;
        }
    };
    struct store_cd_info
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const&, FSM& ,SourceState& ,TargetState& )
        {
            cout << "player::store_cd_info" << endl;
        }
    };
    struct process_play
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const&, FSM& fsm,SourceState& ,TargetState& )
        {
            cout << "player::process_play" << endl;
            fsm.process_event(play());
        }
    };
    struct stop_playback
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::stop_playback" << endl;
        }
    };
    struct pause_playback
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::pause_playback" << endl;
        }
    };
    struct resume_playback
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::resume_playback" << endl;
        }
    };
    struct stop_and_open
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::stop_and_open" << endl;
        }
    };

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_>
    {

        // the initial state of the player SM. Must be defined
        typedef Empty initial_state;

        // Transition table for player
        struct transition_table : boost::mpl::vector<
          Row <Stopped , play        , Playing                    , start_playback           > ,
          Row <Stopped , open_close  , Open                       , open_drawer              > ,
          Row <Stopped , stop        , Stopped                                               > ,
          //  +------------------------------------------------------------------------------+
          Row <Open    , open_close  , Empty                      , close_drawer             > ,
          //  +------------------------------------------------------------------------------+
          Row <Empty   , open_close  , Open                       , open_drawer              > ,
          Row <Empty   , cd_detected , Stopped                    , ActionSequence_<
                                                                     boost::mpl::vector<
                                                                       store_cd_info,
                                                                       process_play> >       > ,
          //  +------------------------------------------------------------------------------+
          Row <Playing , stop        , Stopped                    , stop_playback            > ,
          Row <Playing , start_pause , Paused                     , pause_playback           > ,
          Row <Playing , open_close  , Open                       , stop_and_open            > ,
          //  +------------------------------------------------------------------------------+
          Row <Paused  , end_pause   , Playing                    , resume_playback          > ,
          Row <Paused  , stop        , Stopped                    , stop_playback            > ,
          Row <Paused  , open_close  , Open                       , stop_and_open            >
          //  +------------------------------------------------------------------------------+
          > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            std::cout << "no transition from state " << state
                << " on event " << typeid(e).name() << std::endl;
        }
    };
    // Pick a back-end
    typedef msm::back::state_machine<player_> player;

    //
    // Testing utilities.
    //
    static char const* const state_names[] = { "Stopped", "Open", "Empty", "Playing", "Paused" };
    void pstate(player const& p)
    {
        std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
    }


}
void test()
{
    player p;
    // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
    p.start();
    // go to Open, call on_exit on Empty, then action, then on_entry on Open
    p.process_event(open_close()); pstate(p);
    p.process_event(open_close()); pstate(p);
    p.process_event(cd_detected()); pstate(p);

    // at this point, Play is active
    p.process_event(start_pause()); pstate(p);
    // go back to Playing
    p.process_event(end_pause());  pstate(p);
    p.process_event(start_pause()); pstate(p);
    p.process_event(stop());  pstate(p);
    // event leading to the same state
    // no action method called as it is not present in the transition table
    p.process_event(stop());  pstate(p);
}

#endif

#ifdef TEST_EUML_TABLE
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/euml/euml.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace std;


namespace
{
    // events
    struct play_impl : msm::front::euml::euml_event<play_impl> {};
    struct end_pause_impl : msm::front::euml::euml_event<end_pause_impl>{};
    struct stop_impl : msm::front::euml::euml_event<stop_impl>{};
    struct pause_impl : msm::front::euml::euml_event<pause_impl>{};
    struct open_close_impl : msm::front::euml::euml_event<open_close_impl>{};
    struct cd_detected_impl : msm::front::euml::euml_event<cd_detected_impl>{};

    // define some dummy instances for use in the transition table
    // it is also possible to default-construct them instead:
    // struct play {};
    // inside the table: play()
    play_impl play;
    end_pause_impl end_pause;
    stop_impl stop;
    pause_impl start_pause;
    open_close_impl open_close;
    cd_detected_impl cd_detected;

    // The list of FSM states
    // they have to be declared outside of the front-end only to make VC happy :(
    // note: gcc would have no problem
    struct Empty_impl : public msm::front::state<> , public msm::front::euml::euml_state<Empty_impl>
    {
        // every (optional) entry/exit methods get the event passed.
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Empty" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Empty" << std::endl;}
    };
    struct Open_impl : public msm::front::state<> , public msm::front::euml::euml_state<Open_impl>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Open" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Open" << std::endl;}
    };

    struct Stopped_impl : public msm::front::state<> , public msm::front::euml::euml_state<Stopped_impl>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Stopped" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Stopped" << std::endl;}
    };

    struct Playing_impl : public msm::front::state<> , public msm::front::euml::euml_state<Playing_impl>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Playing" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Playing" << std::endl;}
    };

    // state not defining any entry or exit
    struct Paused_impl : public msm::front::state<> , public msm::front::euml::euml_state<Paused_impl>
    {
    };
    //to make the transition table more readable
    Empty_impl Empty;
    Open_impl Open;
    Stopped_impl Stopped;
    Playing_impl Playing;
    Paused_impl Paused;

    BOOST_MSM_EUML_ACTION(start_playback)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::start_playback" << endl;
        }
    };
    BOOST_MSM_EUML_ACTION(open_drawer)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::open_drawer" << endl;
        }
    };
    BOOST_MSM_EUML_ACTION(close_drawer)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::close_drawer" << endl;
        }
    };
    BOOST_MSM_EUML_ACTION(store_cd_info)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const&, FSM& ,SourceState& ,TargetState& )
        {
            cout << "player::store_cd_info" << endl;
        }
    };
    BOOST_MSM_EUML_ACTION(process_play)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const&, FSM& fsm,SourceState& ,TargetState& )
        {
            cout << "player::process_play" << endl;
            fsm.process_event(play);
        }
    };
    BOOST_MSM_EUML_ACTION(stop_playback)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::stop_playback" << endl;
        }
    };
    BOOST_MSM_EUML_ACTION(pause_playback)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::pause_playback" << endl;
        }
    };
    BOOST_MSM_EUML_ACTION(resume_playback)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::resume_playback" << endl;
        }
    };
    BOOST_MSM_EUML_ACTION(stop_and_open)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
        {
            cout << "player::stop_and_open" << endl;
        }
    };

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_>
    {

        // the initial state of the player SM. Must be defined
        typedef Empty_impl initial_state;

        // Transition table for player
        // replaces the old transition table
        BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE((
          Stopped + play        / start_playback                    == Playing               ,
          Stopped + open_close  / open_drawer                       == Open                  ,
          Stopped + stop                                            == Stopped               ,
          //  +------------------------------------------------------------------------------+
          Open    + open_close  / close_drawer                      == Empty                 ,
          //  +------------------------------------------------------------------------------+
          Empty   + open_close  / open_drawer                       == Open                  ,
          Empty   + cd_detected /(store_cd_info,process_play)       == Stopped               ,
          //  +------------------------------------------------------------------------------+
          Playing + stop        / stop_playback                     == Stopped               ,
          Playing + start_pause / pause_playback                    == Paused                ,
          Playing + open_close  / stop_and_open                     == Open                  ,
          //  +------------------------------------------------------------------------------+
          Paused  + end_pause   / resume_playback                   == Playing               ,
          Paused  + stop        / stop_playback                     == Stopped               ,
          Paused  + open_close  / stop_and_open                     == Open
          //  +------------------------------------------------------------------------------+
          ),transition_table)

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            std::cout << "no transition from state " << state
                << " on event " << typeid(e).name() << std::endl;
        }
    };
    // Pick a back-end
    typedef msm::back::state_machine<player_> player;

    //
    // Testing utilities.
    //
    static char const* const state_names[] = { "Stopped", "Open", "Empty", "Playing", "Paused" };
    void pstate(player const& p)
    {
        std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
    }


}
void test()
{
    player p;
    // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
    p.start();
    // go to Open, call on_exit on Empty, then action, then on_entry on Open
    p.process_event(open_close); pstate(p);
    p.process_event(open_close); pstate(p);
    p.process_event(cd_detected); pstate(p);

    // at this point, Play is active
    p.process_event(start_pause); pstate(p);
    // go back to Playing
    p.process_event(end_pause);  pstate(p);
    p.process_event(start_pause); pstate(p);
    p.process_event(stop);  pstate(p);
    // event leading to the same state
    // no action method called as it is not present in the transition table
    p.process_event(stop);  pstate(p);
}

#endif

#ifdef TEST_EUML2
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

// eUML2 stt grammar
#define BOOST_MPL_LIMIT_STRING_SIZE 64
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/euml2.hpp>

using namespace std;
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;


namespace  // Concrete FSM implementation
{
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

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_,logging_base_state>
    {
        struct start_playback
        {
            typedef start_playback type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
            {
                cout << "player::start_playback" << endl;
            }
        };
        struct open_drawer
        {
            typedef open_drawer type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
            {
                cout << "player::open_drawer" << endl;
            }
        };
        struct close_drawer
        {
            typedef close_drawer type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
            {
                cout << "player::close_drawer" << endl;
            }
        };
        struct store_cd_info
        {
            typedef store_cd_info type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const&, FSM& ,SourceState& ,TargetState& )
            {
                cout << "player::store_cd_info" << endl;
            }
        };
        struct process_play
        {
            typedef process_play type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const&, FSM& fsm,SourceState& ,TargetState& )
            {
                cout << "player::process_play" << endl;
                fsm.process_event(BOOST_MSM_EUML2_EVENT("play",player_)());
            }
        };
        struct stop_playback
        {
            typedef stop_playback type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
            {
                cout << "player::stop_playback" << endl;
            }
        };
        struct pause_playback
        {
            typedef pause_playback type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
            {
                cout << "player::pause_playback" << endl;
            }
        };
        struct resume_playback
        {
            typedef resume_playback type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
            {
                cout << "player::resume_playback" << endl;
            }
        };
        struct stop_and_open
        {
            typedef stop_and_open type;
            template <class FSM,class EVT,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM&,SourceState& ,TargetState& )
            {
                cout << "player::stop_and_open" << endl;
            }
        };

        // the initial state of the player SM. Must be defined
        using initial_state = BOOST_MSM_EUML2_STATE("Empty",player_) ;

        // Transition table for player
        EUML2_STT(
            player_,
            EUML2_STT_CFG(EUML2_STT_USE("start_playback",start_playback),
                          EUML2_STT_USE("open_drawer",open_drawer),
                          EUML2_STT_USE("close_drawer",close_drawer),
                          EUML2_STT_USE("store_cd_info",store_cd_info),
                          EUML2_STT_USE("process_play",process_play),
                          EUML2_STT_USE("stop_playback",stop_playback),
                          EUML2_STT_USE("pause_playback",pause_playback),
                          EUML2_STT_USE("stop_and_open",stop_and_open),
                          EUML2_STT_USE("resume_playback",resume_playback)),
            //     +---------------------------------------------------------------------------+
            EUML2_ROW("Stopped + play        / start_playback              -> Playing"),
            EUML2_ROW("Stopped + open_close  / open_drawer                 -> Open"),
            EUML2_ROW("Stopped + stop                                      -> Stopped"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Open    + open_close  / close_drawer                -> Empty"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Empty   + open_close  / open_drawer                 -> Open"),
            EUML2_ROW("Empty   + cd_detected / store_cd_info,process_play  -> Stopped"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Playing + stop        / stop_playback               -> Stopped"),
            EUML2_ROW("Playing + start_pause / pause_playback              -> Paused"),
            EUML2_ROW("Playing + open_close  / stop_and_open               -> Open"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Paused  + end_pause   / resume_playback             -> Playing"),
            EUML2_ROW("Paused  + stop        / stop_playback               -> Stopped"),
            EUML2_ROW("Paused  + open_close  / stop_and_open               -> Open")
        )
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            std::cout << "no transition from state " << state << " on event " << e.name() << std::endl;
        }
    };


    // Pick a back-end
    typedef msm::back::state_machine<player_> player;

    // Testing utilities.
    void pstate(player const& p )
    {
        // we can ask the state its name, easier!
        std::cout << "active state -> " << p.get_state_by_id(p.current_state()[0])->name() << std::endl;
    }
}
void test()
{
    player p;
    // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
    p.start();
    // go to Open, call on_exit on Empty, then action, then on_entry on Open
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("cd_detected",player_)()); pstate(p);

    // at this point, Play is active
    p.process_event(BOOST_MSM_EUML2_EVENT("start_pause",player_)()); pstate(p);
    // go back to Playing
    p.process_event(BOOST_MSM_EUML2_EVENT("end_pause",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("start_pause",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)()); pstate(p);

    // event leading to the same state
    // no action method called as it is not present in the transition table
    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)()); pstate(p);
}
#endif

#ifdef TEST_EUML2_BIS
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

// eUML2 stt grammar
#define BOOST_MPL_LIMIT_STRING_SIZE 64
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/euml2.hpp>

using namespace std;
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;


namespace  // Concrete FSM implementation
{
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

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_,logging_base_state>
    {
        // the initial state of the player SM. Must be defined
        using initial_state = BOOST_MSM_EUML2_STATE("Empty",player_) ;

        // Transition table for player
        EUML2_STT(
            player_,
            EUML2_STT_CFG(),
            //     +---------------------------------------------------------------------------+
            EUML2_ROW("Stopped + play        / start_playback              -> Playing"),
            EUML2_ROW("Stopped + open_close  / open_drawer                 -> Open"),
            EUML2_ROW("Stopped + stop                                      -> Stopped"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Open    + open_close  / close_drawer                -> Empty"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Empty   + open_close  / open_drawer                 -> Open"),
            EUML2_ROW("Empty   + cd_detected / store_cd_info,process_play  -> Stopped"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Playing + stop        / stop_playback               -> Stopped"),
            EUML2_ROW("Playing + start_pause / pause_playback              -> Paused"),
            EUML2_ROW("Playing + open_close  / stop_and_open               -> Open"),
            //  +------------------------------------------------------------------------------+
            EUML2_ROW("Paused  + end_pause   / resume_playback             -> Playing"),
            EUML2_ROW("Paused  + stop        / stop_playback               -> Stopped"),
            EUML2_ROW("Paused  + open_close  / stop_and_open               -> Open")
        )
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            std::cout << "no transition from state " << state << " on event " << e.name() << std::endl;
        }
    };


    // Pick a back-end
    typedef msm::back::state_machine<player_> player;

    // Testing utilities.
    void pstate(player const& p )
    {
        // we can ask the state its name, easier!
        std::cout << "active state -> " << p.get_state_by_id(p.current_state()[0])->name() << std::endl;
    }
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("start_playback",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::start_playback" << endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("open_drawer",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::open_drawer" << endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("close_drawer",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::close_drawer" << endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("store_cd_info",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::store_cd_info" << endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("process_play",player_)::operator()(Event const&, Fsm& fsm,SourceState& ,TargetState& )
{
    cout << "player::process_play" << endl;
    fsm.process_event(BOOST_MSM_EUML2_EVENT("play",player_)());
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("stop_playback",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::stop_playback" << endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("pause_playback",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::pause_playback" << endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("stop_and_open",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::stop_and_open" << endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("resume_playback",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    cout << "player::resume_playback" << endl;
}

void test()
{
    player p;
    // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
    p.start();
    // go to Open, call on_exit on Empty, then action, then on_entry on Open
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("cd_detected",player_)()); pstate(p);

    // at this point, Play is active
    p.process_event(BOOST_MSM_EUML2_EVENT("start_pause",player_)()); pstate(p);
    // go back to Playing
    p.process_event(BOOST_MSM_EUML2_EVENT("end_pause",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("start_pause",player_)()); pstate(p);
    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)()); pstate(p);

    // event leading to the same state
    // no action method called as it is not present in the transition table
    p.process_event(BOOST_MSM_EUML2_EVENT("stop",player_)()); pstate(p);
}

#endif
