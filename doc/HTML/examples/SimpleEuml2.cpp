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

using namespace std;
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

// overwrite event
namespace{
struct player_;
}

namespace boost { namespace msm { namespace front { namespace euml2
{
template<>
struct euml2_event<BOOST_MSM_EUML2_NAME("Event1"),player_>
{
    // if we specialize, we need to provide this
    typedef euml2_event type;
    std::string name()const
    {
        return BOOST_MSM_EUML2_NAME_FCT(BOOST_MSM_EUML2_NAME("Event1"));
    }
    // we want our own ctor
    euml2_event()
    {
        std::cout << "Event1 ctor called" << std::endl;
    }

};
}}}}

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
        using initial_state = BOOST_MSM_EUML2_STATE("State1",player_) ;

        // Transition table for player
        EUML2_STT(
            player_,
            EUML2_STT_CFG(),
            //     +---------------------------------------------------------------------------------------+
            EUML2_ROW("State1 + Event1 / doIt       -> State2"),
            EUML2_ROW("State1 + *      / doIt       -> State2"),
            EUML2_ROW("State2 + Event2 [ok] / doIt  -> State1"),
            EUML2_ROW("State2 + Event2 [noway]      -> State1")
            //     +---------------------------------------------------------------------------------------+
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
    void pstate(player const& p)
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
    // go to State2
    p.process_event(BOOST_MSM_EUML2_EVENT("Event1",player_)()); pstate(p);
    // go to State1
    p.process_event(BOOST_MSM_EUML2_EVENT("Event2",player_)()); pstate(p);
    // go to State2 using kleene (* = any) event
    p.process_event(BOOST_MSM_EUML2_EVENT("whatever",player_)()); pstate(p);
    std::cout << "stop fsm" << std::endl;
    p.stop();

}


// we want to overwrite on_entry and on_exit only for this state
template<>
template <class Event, class Fsm>
void BOOST_MSM_EUML2_STATE_IMPL("State1",player_)::on_entry(Event const&, Fsm&)
{
    std::cout << "State1::on_entry" << std::endl;
}
template<>
template <class Event, class Fsm>
void BOOST_MSM_EUML2_STATE_IMPL("State1",player_)::on_exit(Event const&, Fsm&)
{
    std::cout << "State1::on_exit" << std::endl;
}
// provide doIt definition
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
void BOOST_MSM_EUML2_ACTION_IMPL("doIt",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "called doIt" << std::endl;
}
// provide noway definition
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("noway",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "called noway" << std::endl;
    return false;
}
// provide ok definition
template<>
template <class Event,class Fsm,class SourceState,class TargetState>
bool BOOST_MSM_EUML2_GUARD_IMPL("ok",player_)::operator()(Event const&, Fsm&,SourceState& ,TargetState& )
{
    std::cout << "called ok" << std::endl;
    return true;
}


int main()
{
    test();
    return 0;
}
