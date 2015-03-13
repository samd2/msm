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
    struct eventRun {};
    struct eventGo {};

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_>
    {
        // no need for exception handling or message queue
        typedef int no_exception_thrown;

        // The list of FSM states
        struct State1 : public msm::front::state<>
        {
            typedef mpl::vector1<eventGo> deferred_events;
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        struct State2 : public msm::front::state<>
        {
            typedef mpl::vector1<eventRun> deferred_events;
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        // the initial state of the player SM. Must be defined
        typedef State1 initial_state;


        // guard conditions


        typedef player_ p; // makes transition table cleaner

        // Transition table for player
        struct transition_table : mpl::vector<
            //    Start     Event         Next      Action                     Guard
            //  +---------+-------------+---------+---------------------------+----------------------+
            Row < State1  , eventRun    , State2  , none                      , none                 >,
            Row < State2  , eventGo     , State1  , none                      , none                 >
            //  +---------+-------------+---------+---------------------------+----------------------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& /*evt*/, FSM&,int /*s*/)
        {
            //std::cout << "no transition from state: " << s << " and event: " << typeid(evt).name() << std::endl;
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
        }

    };
    template <class Player>
    struct func
    {
      Player& _fsm;
      boost::shared_ptr<boost::promise<void> > _promise;
      boost::shared_ptr<boost::promise<void> > _promise2;

      func(Player& fsm_,boost::shared_ptr<boost::promise<void> > aPromise,boost::shared_ptr<boost::promise<void> > aPromise2) :
        _fsm(fsm_)
      , _promise(aPromise)
      , _promise2(aPromise2)
      {
      }

      void operator()()
      {
        std::cout << "start thread" << std::endl;
        _promise->set_value();
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        for (int j = 0 ; j< 10 ; ++j)
        {
            std::cout << "thread loop: " << j << std::endl;
            for (int i = 0 ; i< 900 ; ++i)
            {
                _fsm.process_event(eventRun());
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
        _promise2->set_value();
      }
    };
    //disabled because not working yet
/*
    BOOST_AUTO_TEST_CASE( test_lockfree_deferred )
    {
        // Pick a back-end
        typedef msm::back::state_machine<player_, boost::msm::back::lockfree_policy<>> player;
        for (int c = 0 ; c< 10 ; ++c)
        {
            player p;
            std::cout << "start fsm" << std::endl;
            p.start();
            {
                boost::shared_ptr<boost::promise<void> > aPromise(new boost::promise<void>);
                boost::shared_future<void> fu = aPromise->get_future();
                boost::shared_ptr<boost::promise<void> > aPromise2(new boost::promise<void>);
                boost::shared_future<void> fu2 = aPromise2->get_future();

                boost::thread t(( func<player>(p,aPromise,aPromise2) ));
                boost::strict_scoped_thread<> g( (boost::move(t)) );
                fu.get();
                for (int j = 0 ; j< 10 ; ++j)
                {
                    std::cout << "main loop: " << j << std::endl;
                    for (int i = 0 ; i< 900 ; ++i)
                    {
                        p.process_event(eventGo());
                    }
                    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
                }
                fu2.get();
            }
            p.stop();

            std::cout << "S1 entry: " << p.get_state<player_::State1&>().entry_counter << std::endl;
            std::cout << "S2 entry: " << p.get_state<player_::State2&>().entry_counter << std::endl;

            BOOST_CHECK_MESSAGE(p.get_state<player_::State1&>().entry_counter == p.get_state<player_::State1&>().exit_counter,"State1 entry != State1 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().entry_counter == p.get_state<player_::State2&>().exit_counter,"State2 entry != State2 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State1&>().entry_counter == 90001,"State1 entry != 90001");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().entry_counter == 90000,"State2 entry != 90000");
            BOOST_CHECK_MESSAGE(p.current_state()[0] == 0,"State1 should be active"); //Stopped
        }

    }*/
}


