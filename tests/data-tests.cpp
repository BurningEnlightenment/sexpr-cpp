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


BOOST_AUTO_TEST_SUITE(data_tests)

BOOST_AUTO_TEST_SUITE(ctor_tests)


BOOST_AUTO_TEST_CASE(default_ctor)
{
    node n;
    BOOST_TEST_REQUIRE(n.which() == node_type::list);
    BOOST_TEST(n.is_list());
    BOOST_TEST(n.type_info() == typeid(node::list));

    BOOST_TEST(n.empty());
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

BOOST_AUTO_TEST_CASE(init_ctor)
{
    using namespace std::string_literals;
    std::string s1 = "foo", s2 = "bar", s3 = "baz";

    node n0{};
    BOOST_TEST_REQUIRE(n0.is_list());
    BOOST_TEST(n0.empty());

    node n1{s1};
    BOOST_TEST_REQUIRE(n1.is_list());
    BOOST_TEST_REQUIRE(!n1.empty());
    BOOST_TEST(n1.size() == 1);
    BOOST_TEST(n1.at(0).is_string());
    BOOST_TEST(n1.at(0).get_string() == s1);

    node n2{s1,s2,s3};
    BOOST_TEST_REQUIRE(n2.is_list());
    BOOST_TEST_REQUIRE(!n2.empty());
    BOOST_TEST(n2.size() == 3);
    BOOST_TEST(n2.at(0).is_string());
    BOOST_TEST(n2.at(0).get_string() == s1);
    BOOST_TEST(n2.at(1).is_string());
    BOOST_TEST(n2.at(1).get_string() == s2);
    BOOST_TEST(n2.at(2).is_string());
    BOOST_CHECK_EQUAL(n2.at(2), s3);

    node n3{"foo"s,{"bar"s, "baz"s},{"foobar"s}, "oh my"s};
    BOOST_TEST_REQUIRE(n3.is_list());
    BOOST_TEST_REQUIRE(n3.size() == 4);
    BOOST_TEST(n3[0].is_string());
    BOOST_TEST(n3[1].is_list());
    BOOST_TEST(n3[1].size() == 2);
    BOOST_TEST(n3[1][0].is_string());
    BOOST_TEST(n3[2].is_list());
    BOOST_TEST(n3[2].size() == 1);
    BOOST_TEST(n3[3].is_string());
}

BOOST_AUTO_TEST_CASE(copy_ctor)
{
    using namespace std::string_literals;
    node n0{ "foo"s, { "bar"s, "baz"s }, {"foobar"s}, "oh my"s };
    node n1 = n0;

    BOOST_TEST(n0 == n1);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(tmp_case)
{
    using namespace std::string_literals;
    node n0;
    BOOST_TEST(n0.is_list());

    node n1{std::string(), std::string()};
    BOOST_TEST(n1.is_list());

    for (auto it = n1.cbegin(), end = n1.cend(); it != end; ++it)
    {
        BOOST_TEST(it->is_string());
    }
    n1.clear();
    n1.emplace_back(""s);
    n1.swap(n0);
    BOOST_TEST(n1 == n1);

    bool x = visit([](const auto &e) { return e.empty(); }, n1);

    n0 = ""s;
    BOOST_TEST(n0.is_string());

    //n0 = { std::string(), node()}
}


BOOST_AUTO_TEST_SUITE_END()
