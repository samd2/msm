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
#include <cxxabi.h>
// eUML2 stt grammar
#define BOOST_MPL_LIMIT_STRING_SIZE 64
#define MPLLIBS_LIMIT_STRING_SIZE BOOST_MPL_LIMIT_STRING_SIZE
#include <boost/msm/front/euml2/stt_grammar.hpp>
#include <typeinfo>
#include <boost/test/unit_test.hpp>

namespace
{
    struct s1{};
    struct s2{};
}

BOOST_AUTO_TEST_CASE( test_stt_cfg )
{
    using boost::msm::front::euml2::dummy_fsm;
    using boost::mpl::pair;

    BOOST_CHECK_MESSAGE(typeid(EUML2_STT_CFG())== typeid(boost::mpl::map<>),"should be empty map");

    BOOST_CHECK_MESSAGE(typeid(EUML2_STT_CFG(EUML2_STT_USE("s1",s1)))==
                                             typeid(boost::mpl::map<pair<BOOST_MSM_EUML2_NAME("s1"),s1>>),"wrong map with 1 element");

    BOOST_CHECK_MESSAGE(typeid(EUML2_STT_CFG(EUML2_STT_USE("s1",s1),
                                             EUML2_STT_USE("s2",s2)))==
                                             typeid(boost::mpl::map<pair<BOOST_MSM_EUML2_NAME("s1"),s1>,
                                                                    pair<BOOST_MSM_EUML2_NAME("s2"),s2>>),"wrong map with 2 elements");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa || cc] / bb -> foo")::Source::name_type)==
                        typeid(BOOST_MSM_EUML2_NAME("foo")),"wrong source for row");

}



