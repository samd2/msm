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

#include <boost/msm/front/euml2/detail/common.hpp>


namespace boost { namespace msm { namespace front { namespace euml2
{
// transform a row
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
                boost::mpl::eval_if<
                    has_name_type< get_action_from_row< ::boost::mpl::placeholders::_2>>,
                    replace_from_cfg_for_guard<Cfg,make_tag_for_action< Fsm, get_action_from_row< ::boost::mpl::placeholders::_2>>>,
                    get_action_from_row< ::boost::mpl::placeholders::_2>
                >,
                boost::mpl::eval_if<
                    has_name_type< get_guard_from_row< ::boost::mpl::placeholders::_2>>,
                    replace_from_cfg_for_guard<Cfg,make_tag_for_guard< Fsm, get_guard_from_row< ::boost::mpl::placeholders::_2>>>,
                    get_guard_from_row< ::boost::mpl::placeholders::_2>
                >
            >
        >
    >::type type;
};
}}}}


#endif //BOOST_MSM_FRONT_EUML2_STT_GRAMMAR_H
