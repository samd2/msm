// Copyright 2010 Christophe Henry
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
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

// eUML2 stt grammar
#define BOOST_MPL_LIMIT_STRING_SIZE 55
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/euml2.hpp>

namespace msm = boost::msm;

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
                EUML2_STT_CFG(), // will be explained later on
                EUML2_ROW("Empty + open_close             / open_drawer  -> Open"),
                EUML2_ROW("Open  + open_close [can_close] / close_drawer -> Empty")
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
// provide action / guard definition
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("open_drawer",player_)::operator()(Event const&, Fsm&, SourceState&, TargetState& )
{
    // do something useful
    std::cout << "calling open_drawer" << std::endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("close_drawer",player_)::operator()(Event const&, Fsm&, SourceState&, TargetState& )
{
    // do something useful
    std::cout << "calling close_drawer" << std::endl;
}
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("can_close",player_)::operator()(Event const&, Fsm&, SourceState&, TargetState& )
{
    // do something useful
    std::cout << "calling can_close" << std::endl;
    return true;
}

void test()
{
    player p;
    // needed to start the highest-level SM. This will call on_entry of Empty and mark the start of the SM
    p.start();
    // go to Open
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)()); pstate(p);
    // go to Empty
    p.process_event(BOOST_MSM_EUML2_EVENT("open_close",player_)()); pstate(p);
    // this will call on_exit of Empty
    p.stop();

}

int main()
{
    test();
    return 0;
}
