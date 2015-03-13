// Copyright 2008 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_FRONT_EUML2_MACROS_HPP
#define BOOST_MSM_FRONT_EUML2_MACROS_HPP

#include <boost/msm/front/euml2/stt_grammar.hpp>
#include <boost/msm/front/euml2/state_internal.hpp>

// just for debugging purposes
#define EUML2_ROW_RAW(rowstring)                                  \
    typename boost::msm::front::euml2::row_parser::apply<MPLLIBS_STRING(rowstring)>::type

// for use in stt instead of Row
#define EUML2_ROW(rowstring)                                  \
    typename boost::msm::front::euml2::row_parser::apply<MPLLIBS_STRING(rowstring)>::type

#define EUML2_INTERNAL(rowstring)                                  \
    typename boost::msm::front::euml2::internal_row_parser::apply<MPLLIBS_STRING(rowstring)>::type

// name of a token, use for definition of on_entry/on_exit (states) or operator() (guard/action)
#define BOOST_MSM_EUML2_NAME(aname)                             \
    typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type

// the eUML2 state hiding behind a name token
#define BOOST_MSM_EUML2_STATE(aname,fsm)                             \
    boost::msm::front::euml2::euml2_state<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type, fsm>

// the eUML2 event hiding behind a name token
#define BOOST_MSM_EUML2_EVENT(aname,fsm)                             \
    boost::msm::front::euml2::euml2_event<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type, fsm>

// the eUML2 guard hiding behind a name token
#define BOOST_MSM_EUML2_GUARD(aname)                             \
    boost::msm::front::euml2::euml2_guard<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type>

// the eUML2 action hiding behind a name token
#define BOOST_MSM_EUML2_ACTION(aname)                             \
    boost::msm::front::euml2::euml2_action<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type>

#define EUML2_STT_USE(key,name)                                                   \
    boost::mpl::pair<BOOST_MSM_EUML2_NAME(key),name>

#define EUML2_STT_CFG(args...)                                  \
    boost::mpl::map<args>

#define EUML2_STT(fsm,cfg,args...)                                                          \
    struct transition_table : boost::msm::front::euml2::stt_helper<fsm,cfg,boost::mpl::list<args>>::type{};

#define EUML2_STT_INTERNAL(fsm,cfg,args...)                                                          \
    struct internal_transition_table : boost::msm::front::euml2::internal_stt_helper<fsm,cfg,boost::mpl::list<args>>::type{};

// for state / action / guard / event implementations
#define BOOST_MSM_EUML2_STATE_IMPL(aname,fsm)                       \
    boost::msm::front::euml2::euml2_state<BOOST_MSM_EUML2_NAME(aname),fsm>

#define BOOST_MSM_EUML2_ACTION_IMPL(aname,fsm)                       \
    boost::msm::front::euml2::euml2_action<BOOST_MSM_EUML2_NAME(aname),fsm>

#define BOOST_MSM_EUML2_GUARD_IMPL(aname,fsm)                       \
    boost::msm::front::euml2::euml2_guard<BOOST_MSM_EUML2_NAME(aname),fsm>

#define BOOST_MSM_EUML2_EVENT_IMPL(aname,fsm)                       \
    boost::msm::front::euml2::euml2_event<BOOST_MSM_EUML2_NAME(aname),fsm>

#define BOOST_MSM_EUML2_NAME_FCT(name)                        \
    boost::mpl::c_str<typename boost::msm::front::euml2::vector_c_to_string<name>::type>::value

#endif // BOOST_MSM_FRONT_EUML2_MACROS_HPP
