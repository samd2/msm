// Copyright 2014 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_FRONT_EUML2_STT_GRAMMAR_H
#define BOOST_MSM_FRONT_EUML2_STT_GRAMMAR_H

#define MPLLIBS_LIMIT_SEQUENCE_SIZE 6
#include <mpllibs/metaparse/any_one_of1.hpp>
#include <mpllibs/metaparse/alphanum.hpp>
#include <mpllibs/metaparse/lit_c.hpp>
#include <mpllibs/metaparse/sequence.hpp>
#include <mpllibs/metaparse/token.hpp>
#include <mpllibs/metaparse/transform.hpp>
#include <mpllibs/metaparse/string.hpp>
#include <mpllibs/metaparse/middle_of.hpp>
#include <mpllibs/metaparse/last_of.hpp>
#include <mpllibs/metaparse/any_one_of.hpp>
#include <mpllibs/metaparse/build_parser.hpp>
#include <mpllibs/metaparse/one_of.hpp>
#include <mpllibs/metaparse/return_.hpp>

#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/at.hpp>
#include <boost/any.hpp>

#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/states.hpp>
#include <boost/msm/front/completion_event.hpp>
#include <boost/msm/front/euml/operator.hpp>

namespace boost { namespace msm { namespace front { namespace euml2
{
typedef
  mpllibs::metaparse::any_one_of1<
    mpllibs::metaparse::alphanum,
    mpllibs::metaparse::lit_c<'_'>,
    mpllibs::metaparse::lit_c<'*'>
  >
  token_name;

// parses only names
typedef mpllibs::metaparse::build_parser<
  token_name
> name_parser;

template <class Name>
struct euml2_state: public msm::front::state<>
{
    typedef euml2_state type;
    typedef Name name_type;
    template <class Event, class Fsm>
    void on_entry(Event const&, Fsm&)
    {
    }
    template <class Event, class Fsm>
    void on_exit(Event const&, Fsm&)
    {
    }
};
template <class Name>
struct euml2_event
{
    typedef Name name_type;
    typedef euml2_event type;
};
template <class Name>
struct euml2_action
{
    typedef Name name_type;
    typedef euml2_action type;
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const&, FSM&,SourceState& ,TargetState& )
    {
    }
};
template <class Name>
struct euml2_guard
{
    typedef Name name_type;
    typedef euml2_guard type;
    template <class EVT,class FSM,class SourceState,class TargetState>
    bool operator()(EVT const&, FSM&,SourceState& ,TargetState& )
    {
        return true;
    }
};

template <class T>
struct make_euml2_event
{
    typedef boost::msm::front::euml2::euml2_event<T> type;
};
template <>
struct make_euml2_event<boost::msm::front::none>
{
    typedef boost::msm::front::none type;
};
template <>
struct make_euml2_event<
        typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING("*")>::type
>
{
    typedef boost::any type;
};
template <class T>
struct make_euml2_state
{
    typedef boost::msm::front::euml2::euml2_state<T> type;
};
template <>
struct make_euml2_state<boost::msm::front::none>
{
    typedef boost::msm::front::none type;
};
template <class T>
struct make_euml2_action
{
    typedef boost::msm::front::euml2::euml2_action<T> type;
};
template <>
struct make_euml2_action<boost::msm::front::none>
{
    typedef boost::msm::front::none type;
};
template <class T>
struct make_euml2_guard
{
    typedef boost::msm::front::euml2::euml2_guard<T> type;
};
template <>
struct make_euml2_guard<boost::msm::front::none>
{
    typedef boost::msm::front::none type;
};

struct guard_and_transform
{
    typedef guard_and_transform type;

    template <class ResultOfSequence>
    struct apply :
    boost::msm::front::euml::And_<
      boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
      boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 2>::type>
    >
    {};
};
struct guard_exp;
typedef
    mpllibs::metaparse::sequence<
        guard_exp,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'&'> >,
        guard_exp
  > guard_and_sequence;

struct guard_exp: public
    mpllibs::metaparse::one_of<
      guard_and_sequence,
      mpllibs::metaparse::token<token_name>
    >
    {};

    // test with only one parser and optional
    struct transition_transform
    {
        typedef transition_transform type;

        template <class ResultOfSequence>
        struct apply :
        boost::msm::front::Row<
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
          typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 1>::type>::type,
          typename boost::msm::front::euml2::make_euml2_state<typename boost::mpl::at_c<ResultOfSequence, 4>::type>::type,
          typename boost::msm::front::euml2::make_euml2_action<typename boost::mpl::at_c<ResultOfSequence, 3>::type>::type,
          typename boost::msm::front::euml2::make_euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type
        >
        {};
    };
    typedef
      mpllibs::metaparse::transform<
        mpllibs::metaparse::sequence<
          // metaparse::token is used to consume whitespaces after token_name
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::last_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
                  mpllibs::metaparse::token<token_name>
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::middle_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
                  mpllibs::metaparse::token<token_name>,
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::last_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
                  mpllibs::metaparse::token<token_name>
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::last_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
                  mpllibs::metaparse::token<token_name>
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >
        >,
        transition_transform
      >
      transition_parser;

typedef mpllibs::metaparse::build_parser<transition_parser> row_parser;

}}}}
// just for debugging purposes
#define EUML2_ROW_RAW(rowstring)                                  \
    typename boost::msm::front::euml2::row_parser::apply<MPLLIBS_STRING(rowstring)>::type

// for use in stt instead of Row
#define EUML2_ROW(rowstring)                                  \
    typename boost::msm::front::euml2::row_parser::apply<MPLLIBS_STRING(rowstring)>::type

// name of a token, use for definition of on_entry/on_exit (states) or operator() (guard/action)
#define BOOST_MSM_EUML2_NAME(aname)                             \
    typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type

// the eUML2 state hiding behind a name token
#define BOOST_MSM_EUML2_STATE(aname)                             \
    boost::msm::front::euml2::euml2_state<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type>

// the eUML2 event hiding behind a name token
#define BOOST_MSM_EUML2_EVENT(aname)                             \
    boost::msm::front::euml2::euml2_event<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type>

// the eUML2 guard hiding behind a name token
#define BOOST_MSM_EUML2_GUARD(aname)                             \
    boost::msm::front::euml2::euml2_guard<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type>

// the eUML2 action hiding behind a name token
#define BOOST_MSM_EUML2_ACTION(aname)                             \
    boost::msm::front::euml2::euml2_action<typename boost::msm::front::euml2::name_parser::apply<MPLLIBS_STRING(aname)>::type>

#endif //BOOST_MSM_FRONT_EUML2_STT_GRAMMAR_H
