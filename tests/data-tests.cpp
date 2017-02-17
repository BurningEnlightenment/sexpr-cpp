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


struct accessor_fixture
{
    static constexpr char str_raw[] = "foobar";
    const std::string str = str_raw;
    node str_node;
    const node &str_cnode;

    const std::vector<node> node_vector = { "first", "second", "third" };
    node list_node;
    const node &list_cnode;

    accessor_fixture()
        : str_node(str_raw)
        , str_cnode(str_node)
        , list_node()
        , list_cnode(list_node)
    {
        list_node = node(node_vector.begin(), node_vector.end());
    }
};

constexpr char accessor_fixture::str_raw[];

BOOST_FIXTURE_TEST_SUITE(accessor_tests, accessor_fixture)

using namespace std::string_literals;


BOOST_AUTO_TEST_CASE(string_getters)
{
    BOOST_TEST(str_node.is_string());
    BOOST_TEST(!str_node.get_string().compare(str));
    BOOST_CHECK_THROW(str_node.get_list(), std::domain_error);
    BOOST_TEST_REQUIRE(str_node.try_get_string());
    BOOST_TEST(!str_node.try_get_string()->compare(str));
    BOOST_TEST(!str_node.try_get_list());

    BOOST_TEST(str_cnode.is_string());
    BOOST_TEST(!str_cnode.get_string().compare(str));
    BOOST_CHECK_THROW(str_cnode.get_list(), std::domain_error);
    BOOST_TEST_REQUIRE(str_cnode.try_get_string());
    BOOST_TEST(!str_cnode.try_get_string()->compare(str));
    BOOST_TEST(!str_cnode.try_get_list());
}

BOOST_AUTO_TEST_CASE(list_getters)
{
    BOOST_TEST(list_node.is_list());
    BOOST_TEST(list_node.get_list() == node_vector);
    BOOST_CHECK_THROW(list_node.get_string(), std::domain_error);
    BOOST_TEST_REQUIRE(list_node.try_get_list());
    BOOST_TEST(*list_node.try_get_list() == node_vector);
    BOOST_TEST(!list_node.try_get_string());

    BOOST_TEST(list_cnode.is_list());
    BOOST_TEST(list_cnode.get_list() == node_vector);
    BOOST_CHECK_THROW(list_cnode.get_string(), std::domain_error);
    BOOST_TEST_REQUIRE(list_cnode.try_get_list());
    BOOST_TEST(*list_cnode.try_get_list() == node_vector);
    BOOST_TEST(!list_cnode.try_get_string());
}

BOOST_AUTO_TEST_CASE(list_container_emulation)
{
    BOOST_TEST(list_node.get_list() == node_vector);

    BOOST_TEST(list_node.at(0) == node_vector.at(0));
    BOOST_TEST(list_node.at(1) == node_vector.at(1));
    BOOST_TEST(list_node.at(2) == node_vector.at(2));
    BOOST_TEST(list_cnode.at(0) == node_vector.at(0));
    BOOST_TEST(list_cnode.at(1) == node_vector.at(1));
    BOOST_TEST(list_cnode.at(2) == node_vector.at(2));

    BOOST_CHECK_THROW(list_node.at(3), std::exception);
    BOOST_TEST(list_node.at(0) == list_node[0]);
    BOOST_TEST(list_node.at(1) == list_node[1]);
    BOOST_TEST(list_node.at(2) == list_node[2]);
    BOOST_TEST(list_node.at(0) == list_cnode[0]);
    BOOST_TEST(list_node.at(1) == list_cnode[1]);
    BOOST_TEST(list_node.at(2) == list_cnode[2]);

    BOOST_TEST(list_node.front() == node_vector.front());
    BOOST_TEST(list_cnode.front() == node_vector.front());

    BOOST_TEST(list_node.back() == node_vector.back());
    BOOST_TEST(list_cnode.back() == node_vector.back());

    BOOST_TEST(list_node.empty() == node_vector.empty());
    BOOST_TEST(list_cnode.empty() == node_vector.empty());

    BOOST_TEST(list_node.size() == node_vector.size());
    BOOST_TEST(list_cnode.size() == node_vector.size());

    BOOST_TEST(list_node.max_size() >= list_node.size());
    BOOST_TEST(list_cnode.max_size() >= list_cnode.size());
    BOOST_TEST(list_cnode.max_size() >= list_node.max_size());
}

BOOST_AUTO_TEST_CASE(string_container_emulation)
{
    BOOST_CHECK_THROW(str_node.at(0), std::domain_error);
    BOOST_CHECK_THROW(str_node.at(1), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.at(0), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.at(1), std::domain_error);

    BOOST_CHECK_THROW(str_node[0], std::domain_error);
    BOOST_CHECK_THROW(str_node[1], std::domain_error);
    BOOST_CHECK_THROW(str_cnode[0], std::domain_error);
    BOOST_CHECK_THROW(str_cnode[1], std::domain_error);

    BOOST_CHECK_THROW(str_node.front(), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.front(), std::domain_error);

    BOOST_CHECK_THROW(str_node.back(), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.back(), std::domain_error);

    BOOST_TEST(!str_node.empty());
    BOOST_TEST(!str_cnode.empty());

    BOOST_TEST(str_node.size() == 1);
    BOOST_TEST(str_cnode.size() == 1);

    BOOST_TEST(str_node.max_size() == 1);
    BOOST_TEST(str_cnode.max_size() == 1);
}

BOOST_AUTO_TEST_CASE(explicit_string_conversion)
{
    node::string s(str_cnode);
    BOOST_TEST(s == str_cnode.get_string());
    BOOST_TEST(static_cast<node::string>(str_cnode) == str_cnode.get_string());

    BOOST_CHECK_THROW(static_cast<node::string>(list_cnode), std::domain_error);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
