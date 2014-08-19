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



#define MPLLIBS_LIMIT_SEQUENCE_SIZE 12
#include <mpllibs/metaparse/any_one_of1.hpp>
#include <mpllibs/metaparse/alphanum.hpp>
#include <mpllibs/metaparse/lit_c.hpp>
#include <mpllibs/metaparse/sequence.hpp>
#include <mpllibs/metaparse/token.hpp>
#include <mpllibs/metaparse/transform.hpp>
#include <mpllibs/metaparse/string.hpp>
#include <mpllibs/metaparse/middle_of.hpp>

#include <mpllibs/metaparse/any_one_of.hpp>
#include <mpllibs/metaparse/build_parser.hpp>

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
};
template <class Name>
struct euml2_action
{
    typedef Name name_type;
    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const&, FSM&,SourceState& ,TargetState& )
    {
    }
};
template <class Name>
struct euml2_guard
{
    typedef Name name_type;
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

// guard grammar
// [guard op guard] where op=&& || ! etc.
struct guard_and_transform
{
    typedef guard_and_transform type;

    template <class ResultOfSequence>
    struct apply :
    boost::msm::front::euml::And_<
      boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
      boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 3>::type>
    >
    {};
};
typedef
    mpllibs::metaparse::sequence<
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'&'> >,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'&'> >,
        mpllibs::metaparse::token<token_name>
  > guard_and_sequence;

typedef
    mpllibs::metaparse::any_one_of<
      guard_and_sequence,
      mpllibs::metaparse::token<token_name>
    >
    guard_matching;

typedef
  mpllibs::metaparse::transform<
    mpllibs::metaparse::sequence<
      mpllibs::metaparse::token<token_name>,
      mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'&'> >,
      mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'&'> >,
      mpllibs::metaparse::token<token_name>
    >,
    guard_and_transform
  >
  guard_and;

  // terminal element
  struct guard_terminal_transform
  {
      typedef guard_terminal_transform type;

      template <class ResultOfSequence>
      struct apply :
      boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 0>::type>
      {};
  };
  typedef
    mpllibs::metaparse::transform<
        mpllibs::metaparse::token<token_name>,
        guard_terminal_transform
    >
    guard_terminal;

  // token_name is the start symbol of the grammar
  typedef mpllibs::metaparse::build_parser<
    mpllibs::metaparse::any_one_of<
      guard_and,
      guard_terminal
    >
  > guard_grammar;

// transition without guard or action "src + evt -> tgt"
struct src_evt_tgt_transform
{
    typedef src_evt_tgt_transform type;

    template <class ResultOfSequence>
    struct apply :
    boost::msm::front::Row<
      boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
      typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
      boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 5>::type>,
      typename boost::msm::front::none,
      typename boost::msm::front::none
    >
    {};
};
typedef
  mpllibs::metaparse::transform<
    mpllibs::metaparse::sequence<
      // metaparse::token is used to consume whitespaces after token_name
      mpllibs::metaparse::token<token_name>,
      mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
      mpllibs::metaparse::token<token_name>,
      mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
      mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
      mpllibs::metaparse::token<token_name>
    >,
    src_evt_tgt_transform
  >
  src_evt_tgt;

  // transition without guard "src + evt / action -> tgt"
  struct src_evt_action_tgt_transform
  {
      typedef src_evt_action_tgt_transform type;

      template <class ResultOfSequence>
      struct apply :
      boost::msm::front::Row<
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
        typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 7>::type>,
        boost::msm::front::euml2::euml2_action<typename boost::mpl::at_c<ResultOfSequence, 4>::type>,
        typename boost::msm::front::none
      >
      {};
  };
  typedef
    mpllibs::metaparse::transform<
      mpllibs::metaparse::sequence<
        // metaparse::token is used to consume whitespaces after token_name
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
        mpllibs::metaparse::token<token_name>
      >,
      src_evt_action_tgt_transform
    >
    src_evt_action_tgt;

    // transition without action "src + evt [guard] -> tgt"
    struct src_evt_guard_tgt_transform
    {
        typedef src_evt_guard_tgt_transform type;

        template <class ResultOfSequence>
        struct apply :
        boost::msm::front::Row<
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
          typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 6>::type>,
          typename boost::msm::front::none,
          boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 3>::type>
        >
        {};
    };
    typedef
      mpllibs::metaparse::transform<
        mpllibs::metaparse::sequence<
          // metaparse::token is used to consume whitespaces after token_name
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::middle_of<
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
              mpllibs::metaparse::token<token_name>,
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
          >,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
          mpllibs::metaparse::token<token_name>
        >,
        src_evt_guard_tgt_transform
      >
      src_evt_guard_tgt;

      // transition with everything "src + evt [guard] / action -> tgt"
      struct src_evt_guard_action_tgt_transform
      {
          typedef src_evt_guard_action_tgt_transform type;

          template <class ResultOfSequence>
          struct apply :
          boost::msm::front::Row<
            boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
            typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
            boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 8>::type>,
            boost::msm::front::euml2::euml2_action<typename boost::mpl::at_c<ResultOfSequence, 5>::type>,
            boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 3>::type>
          >
          {};
      };
      typedef
        mpllibs::metaparse::transform<
          mpllibs::metaparse::sequence<
            // metaparse::token is used to consume whitespaces after token_name
            mpllibs::metaparse::token<token_name>,
            mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
            mpllibs::metaparse::token<token_name>,
            mpllibs::metaparse::middle_of<
                mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
                mpllibs::metaparse::token<token_name>,
                mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
            >,
            mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
            mpllibs::metaparse::token<token_name>,
            mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
            mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
            mpllibs::metaparse::token<token_name>
          >,
          src_evt_guard_action_tgt_transform
        >
        src_evt_guard_action_tgt;

// internal transitions
// internal transition without action or guard "src + evt"
struct src_evt_transform
{
    typedef src_evt_transform type;

    template <class ResultOfSequence>
    struct apply :
      boost::msm::front::Row<
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
        typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
        typename boost::msm::front::none,
        typename boost::msm::front::none,
        typename boost::msm::front::none
      >
    {};
};
typedef
  mpllibs::metaparse::transform<
    mpllibs::metaparse::sequence<
      // metaparse::token is used to consume whitespaces after token_name
      mpllibs::metaparse::token<token_name>,
      mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
      mpllibs::metaparse::token<token_name>
    >,
    src_evt_transform
  >
  src_evt;

  // internal transition without guard "src + evt / action"
  struct src_evt_action_transform
  {
      typedef src_evt_action_transform type;

      template <class ResultOfSequence>
      struct apply :
      boost::msm::front::Row<
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
        typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
        typename boost::msm::front::none,
        boost::msm::front::euml2::euml2_action<typename boost::mpl::at_c<ResultOfSequence, 4>::type>,
        typename boost::msm::front::none
      >
      {};
  };
  typedef
    mpllibs::metaparse::transform<
      mpllibs::metaparse::sequence<
        // metaparse::token is used to consume whitespaces after token_name
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
        mpllibs::metaparse::token<token_name>
      >,
      src_evt_action_transform
    >
    src_evt_action;

    // internal transition without action "src + evt [guard]"
    struct src_evt_guard_transform
    {
        typedef src_evt_guard_transform type;

        template <class ResultOfSequence>
        struct apply :
        boost::msm::front::Row<
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
          typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
          typename boost::msm::front::none,
          typename boost::msm::front::none,
          boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 3>::type>
        >
        {};
    };
    typedef
      mpllibs::metaparse::transform<
        mpllibs::metaparse::sequence<
          // metaparse::token is used to consume whitespaces after token_name
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::middle_of<
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
              mpllibs::metaparse::token<token_name>,
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
          >
        >,
        src_evt_guard_transform
      >
      src_evt_guard;

      // internal transition with everything "src + evt [guard] / action"
      struct src_evt_guard_action_transform
      {
          typedef src_evt_guard_action_transform type;

          template <class ResultOfSequence>
          struct apply :
          boost::msm::front::Row<
            boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
            typename boost::msm::front::euml2::make_euml2_event<typename boost::mpl::at_c<ResultOfSequence, 2>::type>::type,
            boost::msm::front::none,
            boost::msm::front::euml2::euml2_action<typename boost::mpl::at_c<ResultOfSequence, 5>::type>,
            boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 3>::type>
          >
          {};
      };
      typedef
        mpllibs::metaparse::transform<
          mpllibs::metaparse::sequence<
            // metaparse::token is used to consume whitespaces after token_name
            mpllibs::metaparse::token<token_name>,
            mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
            mpllibs::metaparse::token<token_name>,
            mpllibs::metaparse::middle_of<
                mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
                mpllibs::metaparse::token<token_name>,
                mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
            >,
            mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
            mpllibs::metaparse::token<token_name>
          >,
          src_evt_guard_action_transform
        >
        src_evt_guard_action;

    // anonymous transitions
    // anonymous transition with everything "src [guard] / action -> tgt"
    struct src_guard_action_tgt_transform
    {
        typedef src_guard_action_tgt_transform type;

        template <class ResultOfSequence>
        struct apply :
        boost::msm::front::Row<
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
          boost::msm::front::none,
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 6>::type>,
          boost::msm::front::euml2::euml2_action<typename boost::mpl::at_c<ResultOfSequence, 3>::type>,
          boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 1>::type>
        >
        {};
    };
    typedef
      mpllibs::metaparse::transform<
        mpllibs::metaparse::sequence<
          // metaparse::token is used to consume whitespaces after token_name
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::middle_of<
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
              mpllibs::metaparse::token<token_name>,
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
          >,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
          mpllibs::metaparse::token<token_name>
        >,
        src_guard_action_tgt_transform
      >
      src_guard_action_tgt;

    // anonymous transition without guard "src / action -> tgt"
    struct src_action_tgt_transform
    {
      typedef src_action_tgt_transform type;

      template <class ResultOfSequence>
      struct apply :
      boost::msm::front::Row<
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
        boost::msm::front::none,
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 5>::type>,
        boost::msm::front::euml2::euml2_action<typename boost::mpl::at_c<ResultOfSequence, 2>::type>,
        boost::msm::front::none
      >
      {};
    };
    typedef
    mpllibs::metaparse::transform<
      mpllibs::metaparse::sequence<
        // metaparse::token is used to consume whitespaces after token_name
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
        mpllibs::metaparse::token<token_name>
      >,
      src_action_tgt_transform
    >
    src_action_tgt;

    // anonymous transition without action "src [guard] -> tgt"
    struct src_guard_tgt_transform
    {
        typedef src_guard_tgt_transform type;

        template <class ResultOfSequence>
        struct apply :
        boost::msm::front::Row<
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
          boost::msm::front::none,
          boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 4>::type>,
          boost::msm::front::none,
          boost::msm::front::euml2::euml2_guard<typename boost::mpl::at_c<ResultOfSequence, 1>::type>
        >
        {};
    };
    typedef
      mpllibs::metaparse::transform<
        mpllibs::metaparse::sequence<
          // metaparse::token is used to consume whitespaces after token_name
          mpllibs::metaparse::token<token_name>,
          mpllibs::metaparse::middle_of<
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
              mpllibs::metaparse::token<token_name>,
              mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
          >,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
          mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
          mpllibs::metaparse::token<token_name>
        >,
        src_guard_tgt_transform
      >
      src_guard_tgt;

    // anonymous transition without guard or action "src -> tgt"
    struct src_tgt_transform
    {
      typedef src_tgt_transform type;

      template <class ResultOfSequence>
      struct apply :
      boost::msm::front::Row<
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 0>::type>,
        boost::msm::front::none,
        boost::msm::front::euml2::euml2_state<typename boost::mpl::at_c<ResultOfSequence, 3>::type>,
        boost::msm::front::none,
        boost::msm::front::none
      >
      {};
    };
    typedef
    mpllibs::metaparse::transform<
      mpllibs::metaparse::sequence<
        // metaparse::token is used to consume whitespaces after token_name
        mpllibs::metaparse::token<token_name>,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
        mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
        mpllibs::metaparse::token<token_name>
      >,
      src_tgt_transform
    >
    src_tgt;

// token_name is the start symbol of the grammar
typedef mpllibs::metaparse::build_parser<
  mpllibs::metaparse::any_one_of<
    // transitions
    src_evt_guard_action_tgt,
    src_evt_action_tgt,
    src_evt_guard_tgt,
    src_evt_tgt,
    // internal transitions
    src_evt_guard_action,
    src_evt_action,
    src_evt_guard,
    src_evt,
    // anonymous transitions
    src_guard_action_tgt,
    src_action_tgt,
    src_guard_tgt,
    src_tgt
  >
> row_parser;

  template <class RawRow>
  struct make_msm_transition
  {
      typedef typename boost::mpl::at_c<RawRow,0>::type type;
  };
}}}}
// just for debugging purposes
#define EUML2_ROW_RAW(rowstring)                                  \
    typename boost::msm::front::euml2::row_parser::apply<MPLLIBS_STRING(rowstring)>::type

// for use in stt instead of Row
#define EUML2_ROW(rowstring)                                  \
    typename boost::msm::front::euml2::make_msm_transition<typename boost::msm::front::euml2::row_parser::apply<MPLLIBS_STRING(rowstring)>::type>::type

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
