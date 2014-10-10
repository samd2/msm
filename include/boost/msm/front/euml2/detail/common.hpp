// Copyright 2008 Christophe Henry
// henry UNDERSCORE christophe AT hotmail DOT com
// This is an extended version of the state machine available in the boost::mpl library
// Distributed under the same license as the original.
// Copyright for the original version:
// Copyright 2005 David Abrahams and Aleksey Gurtovoy. Distributed
// under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MSM_FRONT_EUML2_COMMON_H
#define BOOST_MSM_FRONT_EUML2_COMMON_H

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
#include <boost/mpl/list.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/string.hpp>

#include <boost/any.hpp>

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
    typedef euml2_action this_type;
    template <class T>
    struct replace
    {
        typedef T type;
    };
    template <class Fsm>
    struct convert_name
    {
        typedef euml2_action<Name,Fsm> type;
    };
    template <class Op,class Param>
    struct recurse
    {
        typedef typename Op::template apply<this_type>::type type;
    };
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
    typedef euml2_guard this_type;
    template <class T>
    struct replace
    {
        typedef T type;
    };
    template <class Fsm>
    struct convert_name
    {
        typedef euml2_guard<Name,Fsm> type;
    };
    template <class Op,class Param>
    struct recurse
    {
        typedef typename Op::template apply<this_type>::type type;
    };
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
    typedef typename T::OrgTarget type;
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
    typedef euml2_state<typename T::OrgTarget::name_type,Fsm> type;
};

// use the given tag (Fsm) on the action to avoid ODL problems
template <class Fsm,class T>
struct make_tag_for_action
{
    struct replace_name
    {
        template <class U>
        struct apply
        {
            typedef typename U::template convert_name<Fsm>::type type;
        };
    };

    //recursion for sequence
    typedef typename T::template recurse<replace_name,Fsm>::type type;
};
// use the given tag (Fsm) on the guard to avoid ODL problems
template <class Fsm,class T>
struct make_tag_for_guard
{
    struct replace_name
    {
        template <class U>
        struct apply
        {
            typedef typename U::template convert_name<Fsm>::type type;
        };
    };
    //recursion for operators
    typedef typename T::template recurse<replace_name,Fsm>::type type;
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

// replaces for a given guard/action the names by the types provided in the config
template <class Cfg,class T>
struct replace_from_cfg_for_guard
{
    struct find_and_replace_if_found
    {
        template <class U>
        struct apply
        {
            typedef typename boost::mpl::if_<
                boost::mpl::has_key<Cfg,typename eval_name_type<U>::type>,
                typename U::template replace<typename boost::mpl::at<Cfg,typename eval_name_type<U>::type>::type>::type,
                U
            >::type type;
        };
    };
    //recursion for operators
    typedef typename T::template recurse<find_and_replace_if_found,Cfg>::type type;
};
template <class Cfg>
struct replace_from_cfg_for_guard<Cfg,boost::msm::front::none>
{
    typedef boost::msm::front::none type;
};

}}}}
#endif // BOOST_MSM_FRONT_EUML2_COMMON_H
