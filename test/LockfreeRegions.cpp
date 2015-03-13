// Copyright 2010 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_THREAD_VERSION 3
#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#ifndef BOOST_MSM_NONSTANDALONE_TEST
#define BOOST_TEST_MODULE MyTest
#endif
#include <boost/test/unit_test.hpp>


#include <boost/thread/thread.hpp>
#include <boost/thread/scoped_thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/shared_ptr.hpp>
namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

namespace
{
    // events
    struct event1 {};
    struct event2 {};

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_>
    {
        // no need for exception handling or message queue
        typedef int no_exception_thrown;
        typedef int no_message_queue;

        int m_c=0;
        int action_counter1=0;
        int action_counter2=0;

        // The list of FSM states
        struct State1 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        struct State2 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        struct State3 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        struct State11 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        struct State12 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        struct State13 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        // the initial state of the player SM. Must be defined
        typedef boost::mpl::vector<State1,State11> initial_state;

        // transition actions
        struct action1
        {
            template <class EVT,class FSM,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM& fsm,SourceState& ,TargetState& )
            {
                ++fsm.action_counter1;
            }
        };
        struct action2
        {
            template <class EVT,class FSM,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM& fsm,SourceState& ,TargetState& )
            {
                ++fsm.action_counter2;
            }
        };
        // guard conditions


        typedef player_ p; // makes transition table cleaner

        // Transition table for player
        struct transition_table : mpl::vector<
            //    Start     Event         Next      Action                     Guard
            //  +---------+-------------+---------+---------------------------+----------------------+
            Row < State1  , event1      , State2  , none                      , none                 >,
            Row < State2  , event1      , State3  , action1                   , none                 >,
            Row < State3  , event1      , State1  , none                      , none                 >,
            //  +---------+-------------+---------+---------------------------+----------------------+
            Row < State11 , event2      , State12 , none                      , none                 >,
            Row < State12 , event2      , State13 , action2                   , none                 >,
            Row < State13 , event2      , State11 , none                      , none                 >
            //  +---------+-------------+---------+---------------------------+----------------------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& /*evt*/, FSM&,int /*s*/)
        {
            //std::cout << "no transition from state: " << s << " and event: " << typeid(evt).name() << " and counter: " << m_c << std::endl;
            BOOST_FAIL("no_transition called!");
        }
        template <class FSM,class Event>
        void exception_caught (Event const&,FSM&,std::exception&)
        {
            BOOST_FAIL("exception_caught called!");
        }
        // init counters
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& fsm)
        {
            fsm.template get_state<player_::State1&>().entry_counter=0;
            fsm.template get_state<player_::State1&>().exit_counter=0;
            fsm.template get_state<player_::State2&>().entry_counter=0;
            fsm.template get_state<player_::State2&>().exit_counter=0;
            fsm.template get_state<player_::State3&>().entry_counter=0;
            fsm.template get_state<player_::State3&>().exit_counter=0;

            fsm.template get_state<player_::State11&>().entry_counter=0;
            fsm.template get_state<player_::State11&>().exit_counter=0;
            fsm.template get_state<player_::State12&>().entry_counter=0;
            fsm.template get_state<player_::State12&>().exit_counter=0;
            fsm.template get_state<player_::State13&>().entry_counter=0;
            fsm.template get_state<player_::State13&>().exit_counter=0;
        }

    };
    template <class Player>
    struct func
    {
      Player& _fsm;
      boost::shared_ptr<boost::promise<void> > _promise;
      boost::shared_ptr<boost::promise<bool> > _promise2;

      func(Player& fsm_,boost::shared_ptr<boost::promise<void> > aPromise,boost::shared_ptr<boost::promise<bool> > aPromise2) :
        _fsm(fsm_)
      , _promise(aPromise)
      , _promise2(aPromise2)
      {
      }

      void operator()()
      {
        _promise->set_value();
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        bool success = true;
        for (int i = 0 ; i< 1000000 ; ++i)
        {
            boost::msm::back::HandledEnum res = _fsm.process_event(event2());
            if (res != boost::msm::back::HANDLED_TRUE)
                success = false;
        }
        _promise2->set_value(success);
      }
    };

    BOOST_AUTO_TEST_CASE( test_lockfree_regions_weak )
    {
        // Pick a back-end
        // we never retry because there should be no conflict between regions
        typedef msm::back::state_machine<player_, boost::msm::back::lockfree_policy<boost::msm::back::weak_exchange,boost::msm::back::never_retry>> player;
        for (int c = 0 ; c< 100 ; ++c)
        {
            player p;
            p.start();
            {
                p.m_c=c;
                boost::shared_ptr<boost::promise<void> > aPromise(new boost::promise<void>);
                boost::shared_future<void> fu = aPromise->get_future();
                boost::shared_ptr<boost::promise<bool> > aPromise2(new boost::promise<bool>);
                boost::shared_future<bool> fu2 = aPromise2->get_future();

                boost::thread t(( func<player>(p,aPromise,aPromise2) ));
                boost::strict_scoped_thread<> g( (boost::move(t)) );
                for (int i = 0 ; i< 1000000 ; ++i)
                {
                    p.process_event(event1());
                }
                fu.get();
                for (int i = 0 ; i< 1000000 ; ++i)
                {
                    boost::msm::back::HandledEnum res = p.process_event(event1());
                    BOOST_CHECK_MESSAGE(res == boost::msm::back::HANDLED_TRUE,"process_event in region 1 should have succeeded");
                }
                bool region2_success = fu2.get();
                BOOST_CHECK_MESSAGE(region2_success,"process_event in region 2 should have succeeded");
            }
            p.stop();

            BOOST_CHECK_MESSAGE(p.get_state<player_::State1&>().entry_counter == p.get_state<player_::State1&>().exit_counter,"State1 entry != State1 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().entry_counter == p.get_state<player_::State2&>().exit_counter,"State2 entry != State2 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State3&>().entry_counter == p.get_state<player_::State3&>().exit_counter,"State3 entry != State3 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State3&>().entry_counter == p.action_counter1,"State3 entry != action_counter1");


            BOOST_CHECK_MESSAGE(p.get_state<player_::State11&>().entry_counter == p.get_state<player_::State11&>().exit_counter,"State11 entry != State11 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State12&>().entry_counter == p.get_state<player_::State12&>().exit_counter,"State12 entry != State12 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State13&>().entry_counter == p.get_state<player_::State13&>().exit_counter,"State13 entry != State13 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State13&>().entry_counter == p.action_counter2,"State13 entry != action_counter2");
        }

    }
}


