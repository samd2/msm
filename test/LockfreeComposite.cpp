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
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#ifndef BOOST_MSM_NONSTANDALONE_TEST
#define BOOST_TEST_MODULE MyTest
#endif
#include <boost/test/unit_test.hpp>

#define BOOST_THREAD_VERSION 3
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
    struct eventStop {};

    // front-end: define the FSM structure
    struct player_ : public msm::front::state_machine_def<player_>
    {
        // no need for exception handling or message queue
        typedef int no_exception_thrown;
        typedef int no_message_queue;

        int m_c=0;

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
        struct State2_ : public msm::front::state_machine_def<State2_>
        {
            // no need for exception handling or message queue
            typedef int no_exception_thrown;
            typedef int no_message_queue;

            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;

            struct Sub1 : public msm::front::state<>
            {
                template <class Event,class FSM>
                void on_entry(Event const&,FSM& ) {++entry_counter;}
                template <class Event,class FSM>
                void on_exit(Event const&,FSM& ) {++exit_counter;}
                int entry_counter;
                int exit_counter;
            };
            struct Sub2 : public msm::front::state<>
            {
                template <class Event,class FSM>
                void on_entry(Event const&,FSM& ) {++entry_counter;}
                template <class Event,class FSM>
                void on_exit(Event const&,FSM& ) {++exit_counter;}
                int entry_counter;
                int exit_counter;
            };
            struct Sub3 : public msm::front::state<>
            {
                template <class Event,class FSM>
                void on_entry(Event const&,FSM& ) {++entry_counter;}
                template <class Event,class FSM>
                void on_exit(Event const&,FSM& ) {++exit_counter;}
                int entry_counter;
                int exit_counter;
            };
            typedef Sub1 initial_state;
            // Transition table for player
            struct transition_table : mpl::vector<
                //    Start     Event         Next      Action                     Guard
                //  +---------+-------------+---------+---------------------------+----------------------+
                Row < Sub1    , eventRun    , Sub2    , none                      , none                 >,
                Row < Sub2    , eventRun    , Sub3    , none                      , none                 >,
                Row < Sub3    , eventRun    , Sub1    , none                      , none                 >
                //  +---------+-------------+---------+---------------------------+----------------------+
            > {};
            // Replaces the default no-transition response.
            template <class FSM,class Event>
            void no_transition(Event const&, FSM&,int)
            {
                BOOST_FAIL("no_transition in submachine called!");
            }
            template <class FSM,class Event>
            void exception_caught (Event const&,FSM&,std::exception&)
            {
                BOOST_FAIL("exception_caught called in submachine!");
            }
        };
        typedef msm::back::state_machine<State2_, boost::msm::back::lockfree_policy<>> State2;
        struct State3 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        struct Stopped : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {++entry_counter;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {++exit_counter;}
            int entry_counter;
            int exit_counter;
        };
        // the initial state of the player SM. Must be defined
        typedef State1 initial_state;

        // transition actions

        // guard conditions


        typedef player_ p; // makes transition table cleaner

        // Transition table for player
        struct transition_table : mpl::vector<
            //    Start     Event         Next      Action                     Guard
            //  +---------+-------------+---------+---------------------------+----------------------+
            Row < State1  , eventRun    , State2  , none                      , none                 >,
            Row < State2  , eventRun    , State3  , none                      , none                 >,
            Row < State3  , eventRun    , State1  , none                      , none                 >,
            //  +---------+-------------+---------+---------------------------+----------------------+
            Row < State1  , eventStop   , Stopped , none                      , none                 >,
            Row < State2  , eventStop   , Stopped , none                      , none                 >,
            Row < State3  , eventStop   , Stopped , none                      , none                 >,
            Row < Stopped , eventRun    , none    , none                      , none                 >
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
            fsm.template get_state<player_::Stopped&>().entry_counter=0;
            fsm.template get_state<player_::Stopped&>().exit_counter=0;
            fsm.template get_state<player_::State1&>().entry_counter=0;
            fsm.template get_state<player_::State1&>().exit_counter=0;
            fsm.template get_state<player_::State2&>().entry_counter=0;
            fsm.template get_state<player_::State2&>().exit_counter=0;
            fsm.template get_state<player_::State3&>().entry_counter=0;
            fsm.template get_state<player_::State3&>().exit_counter=0;

            fsm.template get_state<player_::State2&>().template get_state<State2_::Sub1&>().entry_counter=0;
            fsm.template get_state<player_::State2&>().template get_state<State2_::Sub1&>().exit_counter=0;
            fsm.template get_state<player_::State2&>().template get_state<State2_::Sub2&>().entry_counter=0;
            fsm.template get_state<player_::State2&>().template get_state<State2_::Sub2&>().exit_counter=0;
            fsm.template get_state<player_::State2&>().template get_state<State2_::Sub3&>().entry_counter=0;
            fsm.template get_state<player_::State2&>().template get_state<State2_::Sub3&>().exit_counter=0;
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
        _promise->set_value();
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        _fsm.process_event(eventStop());
        _promise2->set_value();
      }
    };

    BOOST_AUTO_TEST_CASE( test_lockfree_composite_terminate_strong )
    {
        // Pick a back-end
        typedef msm::back::state_machine<player_, boost::msm::back::lockfree_policy<>> player;
        for (int c = 0 ; c< 100 ; ++c)
        {
            player p;
            p.start();
            {
                p.m_c=c;
                boost::shared_ptr<boost::promise<void> > aPromise(new boost::promise<void>);
                boost::shared_future<void> fu = aPromise->get_future();
                boost::shared_ptr<boost::promise<void> > aPromise2(new boost::promise<void>);
                boost::shared_future<void> fu2 = aPromise2->get_future();

                boost::thread t(( func<player>(p,aPromise,aPromise2) ));
                boost::strict_scoped_thread<> g( (boost::move(t)) );
                for (int i = 0 ; i< 1000000 ; ++i)
                {
                    p.process_event(eventRun());
                }
                fu.get();
                for (int i = 0 ; i< 1000000 ; ++i)
                {
                    p.process_event(eventRun());
                }
                fu2.get();
            }
            p.stop();

            BOOST_CHECK_MESSAGE(p.get_state<player_::State1&>().entry_counter == p.get_state<player_::State1&>().exit_counter,"State1 entry != State1 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().entry_counter == p.get_state<player_::State2&>().exit_counter,"State2 entry != State2 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State3&>().entry_counter == p.get_state<player_::State3&>().exit_counter,"State3 entry != State3 exit");

            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().get_state<player_::State2_::Sub1&>().entry_counter ==
                                p.get_state<player_::State2&>().get_state<player_::State2_::Sub1&>().exit_counter,"Sub1 entry != Sub1 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().get_state<player_::State2_::Sub2&>().entry_counter ==
                                p.get_state<player_::State2&>().get_state<player_::State2_::Sub2&>().exit_counter,"Sub2 entry != Sub2 exit");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().get_state<player_::State2_::Sub3&>().entry_counter ==
                                p.get_state<player_::State2&>().get_state<player_::State2_::Sub3&>().exit_counter,"Sub3 entry != Sub3 exit");

            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().get_state<player_::State2_::Sub1&>().entry_counter != 0,"Sub1 entry != 0");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().get_state<player_::State2_::Sub2&>().entry_counter != 0,"Sub2 entry != 0");
            BOOST_CHECK_MESSAGE(p.get_state<player_::State2&>().get_state<player_::State2_::Sub3&>().entry_counter != 0,"Sub3 entry != 0");


            BOOST_CHECK_MESSAGE(p.get_state<player_::Stopped&>().entry_counter == 1,"Stopped entry != 1");
            BOOST_CHECK_MESSAGE(p.current_state()[0] == 3,"Stopped should be active"); //Stopped
        }

    }
}


