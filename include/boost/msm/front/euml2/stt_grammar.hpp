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
#include <mpllibs/metaparse/foldlp.hpp>
#include <mpllibs/metaparse/foldrp.hpp>

#include <boost/mpl/map.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/at.hpp>
#include <boost/any.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/string.hpp>

#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/states.hpp>
#include <boost/msm/front/completion_event.hpp>
#include <boost/msm/front/euml/operator.hpp>

namespace boost { namespace msm { namespace front { namespace euml2
{
struct kleene : public boost::any
{
    typedef kleene type;
    template<typename ValueType>
    kleene(const ValueType & v) : boost::any(v){}
};
}}}}
namespace boost { namespace msm{
    template<>
    struct is_kleene_event< boost::msm::front::euml2::kleene >
    {
      typedef ::boost::mpl::true_ type;
    };
}}

namespace boost { namespace msm { namespace front { namespace euml2
{
BOOST_MPL_HAS_XXX_TRAIT_DEF(name_type)
// helper, converts a vector_c to a mpl::string
template <class Vec>
struct vector_c_to_string
{
    typedef typename ::boost::mpl::fold<
            Vec,
            boost::mpl::string<>,
            boost::mpl::push_back< ::boost::mpl::placeholders::_1, ::boost::mpl::placeholders::_2>
            >::type type;
};

typedef
mpllibs::metaparse::token<
  mpllibs::metaparse::any_one_of1<
    mpllibs::metaparse::alphanum,
    mpllibs::metaparse::lit_c<'_'>,
    mpllibs::metaparse::lit_c<'*'>
  >
>
token_name;

// parses only names
typedef mpllibs::metaparse::build_parser<
  token_name
> name_parser;

struct dummy_fsm
{
    typedef boost::msm::front::default_base_state BaseAllStates;
};

template <class Name, class ContainingFsm = boost::msm::front::euml2::dummy_fsm>
struct euml2_state: public msm::front::state<typename ContainingFsm::BaseAllStates>
{
    typedef euml2_state type;
    typedef Name name_type;
    typedef typename ContainingFsm::BaseAllStates base_type;

    // returns name of this object (as given in transition table)
    std::string name()const
    {
        return boost::mpl::c_str<typename vector_c_to_string<Name>::type>::value;
    }

    template <class Event, class Fsm>
    void on_entry(Event const& evt, Fsm& fsm)
    {
        ContainingFsm::BaseAllStates::on_entry(evt,fsm);
    }
    template <class Event, class Fsm>
    void on_exit(Event const& evt, Fsm& fsm)
    {
        ContainingFsm::BaseAllStates::on_exit(evt,fsm);
    }
};
template <class Name, class ContainingFsm = boost::msm::front::euml2::dummy_fsm>
struct euml2_event
{
    typedef Name name_type;
    typedef euml2_event type;
    // returns name of this object (as given in transition table)
    std::string name()const
    {
        return boost::mpl::c_str<typename vector_c_to_string<Name>::type>::value;
    }

};
template <class Name, class ContainingFsm = boost::msm::front::euml2::dummy_fsm>
struct euml2_action
{
    typedef Name name_type;
    typedef euml2_action type;
    // returns name of this object (as given in transition table)
    std::string name()const
    {
        return boost::mpl::c_str<typename vector_c_to_string<Name>::type>::value;
    }

    template <class EVT,class FSM,class SourceState,class TargetState>
    void operator()(EVT const&, FSM&,SourceState& ,TargetState& )
    {
    }
};
template <class Name, class ContainingFsm = boost::msm::front::euml2::dummy_fsm>
struct euml2_guard
{
    typedef Name name_type;
    typedef euml2_guard type;
    // returns name of this object (as given in transition table)
    std::string name()const
    {
        return boost::mpl::c_str<typename vector_c_to_string<Name>::type>::value;
    }

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
    typedef boost::msm::front::euml2::kleene type;
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
template <class T1>
struct make_euml2_action<boost::msm::front::ActionSequence_<T1>>
{
    typedef boost::msm::front::ActionSequence_<T1> type;
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
template <class T1,class T2>
struct make_euml2_guard<boost::msm::front::euml::Or_<T1,T2>>
{
    typedef boost::msm::front::euml::Or_<T1,T2> type;
};
template <class T1,class T2>
struct make_euml2_guard<boost::msm::front::euml::And_<T1,T2>>
{
    typedef boost::msm::front::euml::And_<T1,T2> type;
};
template <class T1,class T2>
struct make_euml2_guard<boost::msm::front::euml::EqualTo_<T1,T2>>
{
    typedef boost::msm::front::euml::EqualTo_<T1,T2> type;
};
template <class T1,class T2>
struct make_euml2_guard<boost::msm::front::euml::NotEqualTo_<T1,T2>>
{
    typedef boost::msm::front::euml::NotEqualTo_<T1,T2> type;
};
template <class T1>
struct make_euml2_guard<boost::msm::front::euml::Not_<T1>>
{
    typedef boost::msm::front::euml::Not_<T1> type;
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
typedef  mpllibs::metaparse::token<
            mpllibs::metaparse::sequence<
                mpllibs::metaparse::lit_c<'|'>,
                mpllibs::metaparse::lit_c<'|'>
         >
> or_token;
typedef  mpllibs::metaparse::token<
            mpllibs::metaparse::sequence<
                mpllibs::metaparse::lit_c<'&'>,
                mpllibs::metaparse::lit_c<'&'>
         >
> and_token;

typedef mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'('> > open_paren_token;
typedef mpllibs::metaparse::token<mpllibs::metaparse::lit_c<')'> > close_paren_token;
typedef mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'!'> > not_token;
typedef  mpllibs::metaparse::token<
            mpllibs::metaparse::sequence<
                mpllibs::metaparse::lit_c<'='>,
                mpllibs::metaparse::lit_c<'='>
         >
> equal_to_token;
typedef  mpllibs::metaparse::token<
            mpllibs::metaparse::sequence<
                mpllibs::metaparse::lit_c<'!'>,
                mpllibs::metaparse::lit_c<'='>
         >
> not_equal_to_token;

struct eval_or
{
  template <class C, class State>
  struct apply :
    boost::msm::front::euml::Or_<
          typename boost::msm::front::euml2::make_euml2_guard<typename State::type>::type,
          typename boost::msm::front::euml2::make_euml2_guard<typename boost::mpl::back<C>::type>::type
    >
  {};
};
struct eval_and
{
  template <class C, class State>
  struct apply :
    boost::msm::front::euml::And_<
          typename boost::msm::front::euml2::make_euml2_guard<typename State::type>::type,
          typename boost::msm::front::euml2::make_euml2_guard<typename boost::mpl::back<C>::type>::type
    >
  {};
};
struct eval_equal_to
{
  template <class C, class State>
  struct apply :
    boost::msm::front::euml::EqualTo_<
          typename boost::msm::front::euml2::make_euml2_guard<typename State::type>::type,
          typename boost::msm::front::euml2::make_euml2_guard<typename boost::mpl::back<C>::type>::type
    >
  {};
};
struct eval_not_equal_to
{
  template <class C, class State>
  struct apply :
    boost::msm::front::euml::NotEqualTo_<
          typename boost::msm::front::euml2::make_euml2_guard<typename State::type>::type,
          typename boost::msm::front::euml2::make_euml2_guard<typename boost::mpl::back<C>::type>::type
    >
  {};
};
struct eval_not
{
  template <class C, class State>
  struct apply :
    boost::msm::front::euml::Not_<
          typename boost::msm::front::euml2::make_euml2_guard<typename State::type>::type
    >
  {};
};


struct or_exp;

typedef mpllibs::metaparse::middle_of<open_paren_token, or_exp, close_paren_token> paren_guard_exp;
typedef mpllibs::metaparse::one_of<token_name, paren_guard_exp> simple_exp;

typedef
    mpllibs::metaparse::foldrp<
        not_token,
        simple_exp,
        eval_not
    >
not_exp;

typedef
    mpllibs::metaparse::foldlp<
        mpllibs::metaparse::sequence<equal_to_token,not_exp>,
        not_exp,
        eval_equal_to
    >
equal_to_exp;

typedef
    mpllibs::metaparse::foldlp<
        mpllibs::metaparse::sequence<not_equal_to_token,equal_to_exp>,
        equal_to_exp,
        eval_not_equal_to
    >
not_equal_to_exp;

typedef
    mpllibs::metaparse::foldlp<
        mpllibs::metaparse::sequence<and_token,not_equal_to_exp>,
        not_equal_to_exp,
        eval_and
    >
and_exp;

struct or_exp :
    mpllibs::metaparse::foldlp<
        mpllibs::metaparse::sequence<or_token,and_exp>,
        and_exp,
        eval_or
    >
{};


// action grammar
struct action_exp;
typedef mpllibs::metaparse::token<mpllibs::metaparse::lit_c<','> > comma_token;
typedef mpllibs::metaparse::middle_of<open_paren_token, action_exp, close_paren_token> paren_comma_exp;
typedef mpllibs::metaparse::one_of<token_name, paren_comma_exp> simple_action_exp;

struct eval_action
{
  template <class C, class State>
  struct apply :
    boost::msm::front::ActionSequence_<boost::mpl::vector<
          typename boost::msm::front::euml2::make_euml2_action<typename State::type>::type,
          typename boost::msm::front::euml2::make_euml2_action<typename boost::mpl::back<C>::type>::type>
    >
  {};
};

struct action_exp :
    mpllibs::metaparse::foldlp<
        mpllibs::metaparse::sequence<comma_token,simple_action_exp>,
        simple_action_exp,
        eval_action
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
    struct transition_parser :
      mpllibs::metaparse::transform<
        mpllibs::metaparse::sequence<
          // metaparse::token is used to consume whitespaces after token_name
          token_name,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::last_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'+'> >,
                  token_name
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::middle_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'['> >,
                  or_exp,
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<']'> >
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::last_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'/'> >,
                  action_exp
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >,
          mpllibs::metaparse::one_of<
              mpllibs::metaparse::last_of<
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'-'> >,
                  mpllibs::metaparse::token<mpllibs::metaparse::lit_c<'>'> >,
                  token_name
              >,
              mpllibs::metaparse::return_<boost::msm::front::none>
          >
        >,
        transition_transform
      >
      {};

typedef mpllibs::metaparse::build_parser<transition_parser> row_parser;

template <class Fsm,class T>
struct make_source_from_row
{
    typedef euml2_state<typename T::Source::name_type,Fsm > type;
};
template <class T>
struct get_source_from_row
{
    typedef typename T::Source type;
};
template <class T>
struct get_event_from_row
{
    typedef typename T::Evt type;
};

template <class T>
struct get_target_from_row
{
    typedef typename T::Target type;
};
template <class T>
struct get_action_from_row
{
    typedef typename T::Action type;
};
template <class T>
struct get_guard_from_row
{
    typedef typename T::Guard type;
};

template <class Fsm,class T>
struct make_event_from_row
{
    typedef euml2_event<typename T::Evt::name_type,Fsm> type;
};
template <class Fsm,class T>
struct make_target_from_row
{
    typedef euml2_state<typename T::Target::name_type,Fsm> type;
};
template <class Fsm,class T>
struct make_action_from_row
{
    typedef euml2_action<typename T::Action::name_type,Fsm> type;
};
template <class Fsm,class T>
struct make_guard_from_row
{
    typedef euml2_guard<typename T::Guard::name_type,Fsm> type;
};

template <class T>
struct get_name_type
{
    typedef typename T::name_type type;
};

template <class T>
struct eval_name_type
{
    typedef typename boost::mpl::eval_if<
        has_name_type<T>,
        get_name_type<T>,
        boost::mpl::identity<void>
    >::type type;
};
template <typename Fsm,typename Cfg,typename Stt>
struct stt_helper
{
    typedef typename ::boost::mpl::fold<
        Stt,
        ::boost::mpl::vector<>,
        ::boost::mpl::push_back<
            ::boost::mpl::placeholders::_1,
            ::boost::msm::front::Row<
                boost::mpl::if_<
                    boost::mpl::has_key<Cfg,eval_name_type<get_source_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::at<Cfg,eval_name_type<get_source_from_row< ::boost::mpl::placeholders::_2>>>,
                    make_source_from_row< Fsm,::boost::mpl::placeholders::_2>
                >,
                boost::mpl::if_<
                    boost::mpl::has_key<Cfg,eval_name_type<get_event_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::at<Cfg,eval_name_type<get_event_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::eval_if<
                        has_name_type< get_event_from_row< ::boost::mpl::placeholders::_2>>,
                        make_event_from_row< Fsm, ::boost::mpl::placeholders::_2>,
                        get_event_from_row< ::boost::mpl::placeholders::_2>
                    >
                >,
                boost::mpl::if_<
                    boost::mpl::has_key<Cfg,eval_name_type<get_target_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::at<Cfg,eval_name_type<get_target_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::eval_if<
                        has_name_type< get_target_from_row< ::boost::mpl::placeholders::_2>>,
                        make_target_from_row< Fsm, ::boost::mpl::placeholders::_2>,
                        get_target_from_row< ::boost::mpl::placeholders::_2>
                    >
                >,
                boost::mpl::if_<
                    boost::mpl::has_key<Cfg,eval_name_type<get_action_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::at<Cfg,eval_name_type<get_action_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::eval_if<
                        has_name_type< get_action_from_row< ::boost::mpl::placeholders::_2>>,
                        make_action_from_row< Fsm, ::boost::mpl::placeholders::_2>,
                        get_action_from_row< ::boost::mpl::placeholders::_2>
                    >
                >,
                boost::mpl::if_<
                    boost::mpl::has_key<Cfg,eval_name_type<get_guard_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::at<Cfg,eval_name_type<get_guard_from_row< ::boost::mpl::placeholders::_2>>>,
                    boost::mpl::eval_if<
                        has_name_type< get_guard_from_row< ::boost::mpl::placeholders::_2>>,
                        make_guard_from_row< Fsm, ::boost::mpl::placeholders::_2>,
                        get_guard_from_row< ::boost::mpl::placeholders::_2>
                    >
                >
            >
        >
    >::type type;
};
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

#endif //BOOST_MSM_FRONT_EUML2_STT_GRAMMAR_H
