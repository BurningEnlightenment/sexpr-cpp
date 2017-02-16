// Copyright 2017 Henrik Steffen Gaßmann
//
// Licensed under the MIT License.
// See LICENSE file in the project root for full license information.
//
//#include "precompiled.hpp"
#include <sexpr-cpp/data.hpp>

#include "data-helpers.hpp"
#include "boost-unit-test.hpp"

BOOST_TEST_DONT_PRINT_LOG_VALUE(std::type_info)
using namespace sexpr;


BOOST_AUTO_TEST_SUITE(basic_node)

BOOST_AUTO_TEST_SUITE(ctor_tests)


BOOST_AUTO_TEST_CASE(default_ctor)
{
    node n;
    BOOST_TEST_REQUIRE(n.which() == node_type::list);
    BOOST_TEST(n.is_list());
    BOOST_TEST(n.type_info() == typeid(node::list));

    BOOST_TEST(n.empty());
}

BOOST_AUTO_TEST_CASE(char_array_ctor)
{
    static constexpr char raw_str[] = "foo\0bar";
    const std::string str_w_null(raw_str, sizeof(raw_str));
    const auto str = str_w_null.substr(0, str_w_null.size() - 1);

    node n1(raw_str);
    BOOST_TEST_REQUIRE(n1.is_string());
    BOOST_TEST(n1.get_string() == str);

    node n2(raw_str, false);
    BOOST_TEST_REQUIRE(n2.is_string());
    BOOST_TEST(n2.get_string() == str_w_null);
}

BOOST_AUTO_TEST_CASE(string_ctor)
{
    using namespace std::string_literals;
    auto refstr = "foobar\0baz"s;
    node n(refstr);

    BOOST_TEST_REQUIRE(n.is_string());
    BOOST_TEST(n.type_info() == typeid(node::string));
    auto &s = n.get_string();
    BOOST_TEST(s.size() == refstr.size());
    BOOST_TEST(s == refstr);
}

BOOST_AUTO_TEST_CASE(init_ctor__empty)
{
    node n{};
    BOOST_TEST_REQUIRE(n.is_list());
    BOOST_TEST(n.empty());
}

BOOST_AUTO_TEST_CASE(init_ctor__single)
{
    using namespace std::string_literals;
    const auto s1 = "foo"s;

    node n{ s1 };
    BOOST_TEST_REQUIRE(n.is_list());
    BOOST_TEST_REQUIRE(!n.empty());
    BOOST_TEST(n.size() == 1);
    BOOST_TEST(n.at(0).is_string());
    BOOST_TEST(n.at(0).get_string() == s1);
}

BOOST_AUTO_TEST_CASE(init_ctor__multi)
{
    using namespace std::string_literals;
    std::string s1 = "foo", s2 = "bar", s3 = "baz";

    node n{ s1,s2,s3 };
    BOOST_TEST_REQUIRE(n.is_list());
    BOOST_TEST_REQUIRE(!n.empty());
    BOOST_TEST(n.size() == 3);
    BOOST_TEST(n.at(0).is_string());
    BOOST_TEST(n.at(0).get_string() == s1);
    BOOST_TEST(n.at(1).is_string());
    BOOST_TEST(n.at(1).get_string() == s2);
    BOOST_TEST(n.at(2).is_string());
    BOOST_TEST(s3 == n.at(2).get_string());
}

BOOST_AUTO_TEST_CASE(init_ctor__complex)
{
    node n{ "foo",{ "bar", "baz" },{ "foobar" }, "oh my" };
    BOOST_TEST_REQUIRE(n.is_list());
    BOOST_TEST_REQUIRE(n.size() == 4);
    BOOST_TEST(n[0].is_string());
    BOOST_TEST(n[1].is_list());
    BOOST_TEST(n[1].size() == 2);
    BOOST_TEST(n[1][0].is_string());
    BOOST_TEST(n[2].is_list());
    BOOST_TEST(n[2].size() == 1);
    BOOST_TEST(n[3].is_string());
}

BOOST_AUTO_TEST_CASE(copy_ctor)
{
    using namespace std::string_literals;
    node n0{ "foo"s, { "bar"s, "baz"s }, {"foobar"s}, "oh my"s };
    node n1 = n0;

    BOOST_TEST(n0 == n1);
}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(accessor_tests)

using namespace std::string_literals;


BOOST_AUTO_TEST_CASE(string_getters)
{
    auto s = "foobar"s;
    node ns(s);

    BOOST_TEST(ns.is_string());
    BOOST_TEST(ns.get_string() == s);
    BOOST_CHECK_THROW(ns.get_list(), std::domain_error);
    BOOST_TEST_REQUIRE(ns.try_get_string());
    BOOST_TEST(*ns.try_get_string() == s);
    BOOST_TEST(!ns.try_get_list());

    
}

BOOST_AUTO_TEST_CASE(list_getters)
{
    node nl{};
    BOOST_TEST(nl.is_list());
    BOOST_TEST(nl.get_list() == node::list{});
    BOOST_CHECK_THROW(nl.get_string(), std::domain_error);
    BOOST_TEST_REQUIRE(nl.try_get_list());
    BOOST_TEST(*nl.try_get_list() == node::list{});
    BOOST_TEST(!nl.try_get_string());
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
