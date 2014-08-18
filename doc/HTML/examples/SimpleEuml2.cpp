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
#define BOOST_MPL_LIMIT_STRING_SIZE 64
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/stt_grammar.hpp>
#include <typeinfo>
#include <cxxabi.h>

using namespace std;
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

namespace  // Concrete FSM implementation
{

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_>
    {                
        // the initial state of the player SM. Must be defined
        typedef BOOST_MSM_EUML2_STATE("State1") initial_state;

        // Transition table for player
        struct transition_table : mpl::vector<
        //     +---------------------------------------------------------------------------------------+
        EUML2_ROW("State1 + Event1 / doIt       -> State2"),
        EUML2_ROW("State2 + Event2 [ok] / doIt  -> State1"),
        EUML2_ROW("State2 + Event2 [noway]      -> State1")
        //     +---------------------------------------------------------------------------------------+
        > {};
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            char   *realname;
            realname = abi::__cxa_demangle(typeid(e).name(), 0, 0, 0);
            std::cout << "no transition from state " << state
                << " on event " << realname << std::endl;
            delete realname;
        }
    };
    // Pick a back-end
    typedef msm::back::state_machine<player_> player;

    //
    // Testing utilities.
    //
    static char const* const state_names[] = { "State1", "State2"};
    void pstate(player const& p)
    {
        std::cout << "active state -> " << state_names[p.current_state()[0]] << std::endl;
    }

    void test()
    {
        player p;
        // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
        p.start();
        // go to State2
        p.process_event(BOOST_MSM_EUML2_EVENT("Event1")()); pstate(p);
        // go to State1
        p.process_event(BOOST_MSM_EUML2_EVENT("Event2")()); pstate(p);
        std::cout << "stop fsm" << std::endl;
        p.stop();

    }
}

// we want to overwrite on_entry and on_exit only for this state
template<>
template <class Event, class Fsm>
void boost::msm::front::euml2::euml2_state<BOOST_MSM_EUML2_NAME("State1")>::on_entry(Event const&, Fsm&)
{
    std::cout << "State1::on_entry" << std::endl;
}
template<>
template <class Event, class Fsm>
void boost::msm::front::euml2::euml2_state<BOOST_MSM_EUML2_NAME("State1")>::on_exit(Event const&, Fsm&)
{
    std::cout << "State1::on_exit" << std::endl;
}
// provide doIt definition
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void boost::msm::front::euml2::euml2_action<BOOST_MSM_EUML2_NAME("doIt")>::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "called doIt" << std::endl;
}
// provide noway definition
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool boost::msm::front::euml2::euml2_guard<BOOST_MSM_EUML2_NAME("noway")>::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "called noway" << std::endl;
    return false;
}
// provide ok definition
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool boost::msm::front::euml2::euml2_guard<BOOST_MSM_EUML2_NAME("ok")>::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "called ok" << std::endl;
    return true;
}
int main()
{
    test();
    return 0;
}
