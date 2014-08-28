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


BOOST_AUTO_TEST_CASE( test_guards )
{

    using boost::msm::front::euml::Or_;
    using boost::msm::front::euml::And_;
    using boost::msm::front::euml::EqualTo_;
    using boost::msm::front::euml::NotEqualTo_;
    using boost::msm::front::euml::Bitwise_Xor_;
    using boost::msm::front::euml::Not_;
    using boost::msm::front::Row;
    // guard syntax
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa || cc] / bb -> foo")::Guard)==
                        typeid(Or_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("cc")>),"guard not aa || cc");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa && cc] / bb -> foo")::Guard)==
                        typeid(And_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("cc")>),"guard not aa && cc");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa && cc || dd] / bb -> foo")::Guard)==
                        typeid(Or_<And_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("cc")>,BOOST_MSM_EUML2_GUARD("dd")>),
                        "guard not (aa && cc) || dd");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa || cc || dd] / bb -> foo")::Guard)==
                        typeid(Or_<Or_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("cc")>,BOOST_MSM_EUML2_GUARD("dd")>),
                        "guard not (aa || cc) || dd");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa || (cc && dd)] / bb -> foo")::Guard)==
                        typeid(Or_<BOOST_MSM_EUML2_GUARD("aa"),And_<BOOST_MSM_EUML2_GUARD("cc"),BOOST_MSM_EUML2_GUARD("dd")>>),
                        "guard not (aa || (cc && dd)");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa && (cc || dd)] / bb -> foo")::Guard)==
                        typeid(And_<BOOST_MSM_EUML2_GUARD("aa"),Or_<BOOST_MSM_EUML2_GUARD("cc"),BOOST_MSM_EUML2_GUARD("dd")>>),
                        "guard not aa && (cc || dd)");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [(aa || bb) && (cc || dd)] / bb -> foo")::Guard)==
                        typeid(And_<Or_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("bb")>,
                                    Or_<BOOST_MSM_EUML2_GUARD("cc"),BOOST_MSM_EUML2_GUARD("dd")>>),
                        "guard not (aa || bb) && (cc || dd)");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [(aa && bb) || (cc && dd)] / bb -> foo")::Guard)==
                        typeid(Or_<And_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("bb")>,
                                   And_<BOOST_MSM_EUML2_GUARD("cc"),BOOST_MSM_EUML2_GUARD("dd")>>),
                        "guard not (aa || bb) && (cc || dd)");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa == cc] / bb -> foo")::Guard)==
                        typeid(EqualTo_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("cc")>),"guard not aa == cc");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [(aa && bb) == (cc && dd)] / bb -> foo")::Guard)==
                        typeid(EqualTo_<And_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("bb")>,
                                        And_<BOOST_MSM_EUML2_GUARD("cc"),BOOST_MSM_EUML2_GUARD("dd")>>),
                        "guard not (aa || bb) == (cc || dd)");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa != cc] / bb -> foo")::Guard)==
                        typeid(NotEqualTo_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("cc")>),"guard not aa != cc");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [(aa && bb) != (cc && dd)] / bb -> foo")::Guard)==
                        typeid(NotEqualTo_<And_<BOOST_MSM_EUML2_GUARD("aa"),BOOST_MSM_EUML2_GUARD("bb")>,
                                           And_<BOOST_MSM_EUML2_GUARD("cc"),BOOST_MSM_EUML2_GUARD("dd")>>),
                        "guard not (aa || bb) != (cc || dd)");


    char   *realname;

    realname = abi::__cxa_demangle(typeid(EUML2_ROW("foo + bar [!aa] / bb -> foo")::Guard).name(), 0, 0, 0);
    std::cout << "raw: " << realname << std::endl;
}



