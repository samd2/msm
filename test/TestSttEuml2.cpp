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


BOOST_AUTO_TEST_CASE( test_stt )
{
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb -> foobar")::Target)== typeid(BOOST_MSM_EUML2_STATE("foobar")),"target not foobar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("bla + bar [aa] / bb -> foo")::Source)== typeid(BOOST_MSM_EUML2_STATE("bla")),"src not bla");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb -> foo")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb -> foo")::Action)== typeid(BOOST_MSM_EUML2_ACTION("bb")),"action not bb");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb -> foo")::Guard)== typeid(BOOST_MSM_EUML2_GUARD("aa")),"guard not aa");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar -> tgt")::Target)== typeid(BOOST_MSM_EUML2_STATE("tgt")),"target not tgt");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar -> tgt")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar -> tgt")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar -> tgt")::Action)== typeid(boost::msm::front::none),"action not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar -> tgt")::Guard)== typeid(boost::msm::front::none),"guard not none");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb -> tgt")::Target)== typeid(BOOST_MSM_EUML2_STATE("tgt")),"target not tgt");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb -> tgt")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb -> tgt")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb -> tgt")::Action)== typeid(BOOST_MSM_EUML2_ACTION("bb")),"action not bb");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb -> tgt")::Guard)== typeid(boost::msm::front::none),"guard not none");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] -> tgt")::Target)== typeid(BOOST_MSM_EUML2_STATE("tgt")),"target not tgt");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] -> tgt")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] -> tgt")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] -> tgt")::Action)== typeid(boost::msm::front::none),"action not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] -> tgt")::Guard)== typeid(BOOST_MSM_EUML2_GUARD("aa")),"guard not aa");

    // internal transitions
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar")::Target)== typeid(BOOST_MSM_EUML2_STATE("foo")),"target not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar")::Action)== typeid(boost::msm::front::none),"action not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar")::Guard)== typeid(boost::msm::front::none),"guard not none");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb")::Target)== typeid(BOOST_MSM_EUML2_STATE("foo")),"target not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb")::Action)== typeid(BOOST_MSM_EUML2_ACTION("bb")),"action not bb");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar / bb")::Guard)== typeid(boost::msm::front::none),"guard not none");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa]")::Target)== typeid(BOOST_MSM_EUML2_STATE("foo")),"target not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa]")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa]")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa]")::Action)== typeid(boost::msm::front::none),"action not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa]")::Guard)== typeid(BOOST_MSM_EUML2_GUARD("aa")),"guard not aa");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb")::Target)== typeid(BOOST_MSM_EUML2_STATE("foo")),"target not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb")::Evt)== typeid(BOOST_MSM_EUML2_EVENT("bar")),"evt not bar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb")::Action)== typeid(BOOST_MSM_EUML2_ACTION("bb")),"action not bb");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + bar [aa] / bb")::Guard)== typeid(BOOST_MSM_EUML2_GUARD("aa")),"guard not aa");

    // anonymous transitions
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] / bb -> foobar")::Target)== typeid(BOOST_MSM_EUML2_STATE("foobar")),"target not foobar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("bla [aa] / bb -> foo")::Source)== typeid(BOOST_MSM_EUML2_STATE("bla")),"src not bla");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] / bb -> foo")::Evt)== typeid(boost::msm::front::none),"evt not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] / bb -> foo")::Action)== typeid(BOOST_MSM_EUML2_ACTION("bb")),"action not bb");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] / bb -> foo")::Guard)== typeid(BOOST_MSM_EUML2_GUARD("aa")),"guard not aa");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo / bb -> tgt")::Target)== typeid(BOOST_MSM_EUML2_STATE("tgt")),"target not tgt");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo / bb -> tgt")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo / bb -> tgt")::Evt)== typeid(boost::msm::front::none),"evt not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo / bb -> tgt")::Action)== typeid(BOOST_MSM_EUML2_ACTION("bb")),"action not bb");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo / bb -> tgt")::Guard)== typeid(boost::msm::front::none),"guard not none");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] -> tgt")::Target)== typeid(BOOST_MSM_EUML2_STATE("tgt")),"target not tgt");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] -> tgt")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] -> tgt")::Evt)== typeid(boost::msm::front::none),"evt not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] -> tgt")::Action)== typeid(boost::msm::front::none),"action not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo [aa] -> tgt")::Guard)== typeid(BOOST_MSM_EUML2_GUARD("aa")),"guard not aa");

    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo -> tgt")::Target)== typeid(BOOST_MSM_EUML2_STATE("tgt")),"target not tgt");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo -> tgt")::Source)== typeid(BOOST_MSM_EUML2_STATE("foo")),"src not foo");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo -> tgt")::Evt)== typeid(boost::msm::front::none),"evt not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo -> tgt")::Action)== typeid(boost::msm::front::none),"action not none");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo -> tgt")::Guard)== typeid(boost::msm::front::none),"guard not none");

    //kleene event
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + * [aa] / bb -> foobar")::Target)== typeid(BOOST_MSM_EUML2_STATE("foobar")),"target not foobar");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("bla + * [aa] / bb -> foo")::Source)== typeid(BOOST_MSM_EUML2_STATE("bla")),"src not bla");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + * [aa] / bb -> foo")::Evt)== typeid(boost::any),"evt not kleene");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + * [aa] / bb -> foo")::Action)== typeid(BOOST_MSM_EUML2_ACTION("bb")),"action not bb");
    BOOST_CHECK_MESSAGE(typeid(EUML2_ROW("foo + * [aa] / bb -> foo")::Guard)== typeid(BOOST_MSM_EUML2_GUARD("aa")),"guard not aa");

    using boost::msm::front::euml::Or_;
    using boost::msm::front::euml::And_;
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
//    char   *realname;

//    realname = abi::__cxa_demangle(typeid(EUML2_ROW("foo + bar [aa && (cc || dd)] / bb -> foo")).name(), 0, 0, 0);
//    std::cout << "raw: " << realname << std::endl;
}


