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
namespace bdata = boost::unit_test::data;


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

BOOST_AUTO_TEST_CASE(string_container_iterators)
{
    BOOST_CHECK_THROW(str_node.begin(), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.begin(), std::domain_error);
    BOOST_CHECK_THROW(str_node.cbegin(), std::domain_error);

    BOOST_CHECK_THROW(str_node.end(), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.end(), std::domain_error);
    BOOST_CHECK_THROW(str_node.cend(), std::domain_error);

    BOOST_CHECK_THROW(str_node.rbegin(), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.rbegin(), std::domain_error);
    BOOST_CHECK_THROW(str_node.crbegin(), std::domain_error);

    BOOST_CHECK_THROW(str_node.rend(), std::domain_error);
    BOOST_CHECK_THROW(str_cnode.rend(), std::domain_error);
    BOOST_CHECK_THROW(str_node.crend(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(list_container_iterators)
{
    BOOST_CHECK_EQUAL_COLLECTIONS(list_node.begin(), list_node.end(),
        node_vector.begin(), node_vector.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(list_cnode.begin(), list_cnode.end(),
        node_vector.begin(), node_vector.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(list_cnode.cbegin(), list_cnode.cend(),
        node_vector.begin(), node_vector.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(list_node.rbegin(), list_node.rend(),
        node_vector.rbegin(), node_vector.rend());
    BOOST_CHECK_EQUAL_COLLECTIONS(list_cnode.rbegin(), list_cnode.rend(),
        node_vector.rbegin(), node_vector.rend());
    BOOST_CHECK_EQUAL_COLLECTIONS(list_cnode.crbegin(), list_cnode.crend(),
        node_vector.rbegin(), node_vector.rend());
}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_CASE(string_clear_test)
{
    node s_node("some-str content");
    BOOST_TEST_REQUIRE(s_node.is_string());
    BOOST_TEST_REQUIRE(!s_node.get_string().empty());
    s_node.clear();
    BOOST_TEST(s_node.get_string().empty());
}

BOOST_AUTO_TEST_CASE(string_container_ops_test)
{
    node dummy;
    node s_node("some-str content");
    std::list<node> node_list;

    BOOST_CHECK_THROW(s_node.insert(node::const_iterator(), dummy), std::domain_error);
    BOOST_CHECK_THROW(s_node.insert(node::const_iterator(), std::move(dummy)), std::domain_error);
    BOOST_CHECK_THROW(s_node.insert(node::const_iterator(), 3, dummy), std::domain_error);
    BOOST_CHECK_THROW(s_node.insert(node::const_iterator(), 3, std::move(dummy)), std::domain_error);
    BOOST_CHECK_THROW(s_node.insert(node::const_iterator(), node_list.begin(), node_list.end()), std::domain_error);
    BOOST_CHECK_THROW(s_node.insert(node::const_iterator(), {}), std::domain_error);

    BOOST_CHECK_THROW(s_node.erase(node::const_iterator()), std::domain_error);
    BOOST_CHECK_THROW(s_node.erase(node::const_iterator(), node::const_iterator()), std::domain_error);

    BOOST_CHECK_THROW(s_node.push_back(dummy), std::domain_error);
    BOOST_CHECK_THROW(s_node.push_back(std::move(dummy)), std::domain_error);

    BOOST_CHECK_THROW(s_node.emplace_back(), std::domain_error);
    BOOST_CHECK_THROW(s_node.emplace_back("blublbub"), std::domain_error);

    BOOST_CHECK_THROW(s_node.pop_back(), std::domain_error);

    BOOST_CHECK_THROW(s_node.resize(3), std::domain_error);
    BOOST_CHECK_THROW(s_node.resize(3, dummy), std::domain_error);
}

struct container_ops_fixture
{
    const std::vector<node> node_vector = { "first", "second", "third" };
    node list_node;
    const size_t init_size = 3;

    node elem;
    const node celem;

    const std::list<node> node_list = { "i-first", "i-second", "i-third" };

    container_ops_fixture()
        : list_node()
        , elem("elem")
        , celem("elem")
    {
        list_node = node(node_vector.begin(), node_vector.end());
    }
};

BOOST_FIXTURE_TEST_SUITE(container_mod_tests, container_ops_fixture)


BOOST_AUTO_TEST_CASE(list_clear)
{
    BOOST_TEST_REQUIRE(!list_node.empty());
    list_node.clear();
    BOOST_TEST_REQUIRE(list_node.empty());
}

BOOST_AUTO_TEST_CASE(list_insert_copy__begin)
{
    list_node.insert(list_node.begin(), celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.front() == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_move__begin)
{
    list_node.insert(list_node.begin(), std::move(elem));
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.front() == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_copy__mid)
{
    list_node.insert(++list_node.begin(), celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.at(1) == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_move__mid)
{
    list_node.insert(++list_node.begin(), std::move(elem));
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.at(1) == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_copy__end)
{
    list_node.insert(list_node.end(), celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.back() == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_move__end)
{
    list_node.insert(list_node.end(), std::move(elem));
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.back() == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_multi__begin)
{
    list_node.insert(list_node.begin(), 2, celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 2);
    BOOST_TEST(list_node.front() == celem);
    BOOST_TEST(list_node.at(1) == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_multi__mid)
{
    list_node.insert(++list_node.begin(), 2, celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 2);
    BOOST_TEST(list_node.at(1) == celem);
    BOOST_TEST(list_node.at(2) == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_multi__end)
{
    list_node.insert(list_node.end(), 2, celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 2);
    BOOST_TEST(list_node.at(list_node.size() - 2) == celem);
    BOOST_TEST(list_node.back() == celem);
}

BOOST_AUTO_TEST_CASE(list_insert_iterator__begin)
{
    list_node.insert(list_node.begin(), node_list.begin(), node_list.end());
    BOOST_TEST_REQUIRE(list_node.size() == init_size + node_list.size());

    auto nbegin = list_node.begin();
    auto nend = nbegin + node_list.size();
    BOOST_CHECK_EQUAL_COLLECTIONS(nbegin, nend, node_list.begin(), node_list.end());
}

BOOST_AUTO_TEST_CASE(list_insert_iterator__mid)
{
    list_node.insert(++list_node.begin(), node_list.begin(), node_list.end());
    BOOST_TEST_REQUIRE(list_node.size() == init_size + node_list.size());

    auto nbegin = ++list_node.begin();
    auto nend = nbegin + node_list.size();
    BOOST_CHECK_EQUAL_COLLECTIONS(nbegin, nend, node_list.begin(), node_list.end());
}

BOOST_AUTO_TEST_CASE(list_insert_iterator__end)
{
    list_node.insert(list_node.end(), node_list.begin(), node_list.end());
    BOOST_TEST_REQUIRE(list_node.size() == init_size + node_list.size());

    auto nend = list_node.end();
    auto nbegin = nend - node_list.size();
    BOOST_CHECK_EQUAL_COLLECTIONS(nbegin, nend, node_list.begin(), node_list.end());
}

BOOST_AUTO_TEST_CASE(list_insert_init_list__begin)
{
    list_node.insert(list_node.begin(), { "i-first", "i-second", "i-third" });
    BOOST_TEST_REQUIRE(list_node.size() == init_size + node_list.size());

    auto nbegin = list_node.begin();
    auto nend = nbegin + node_list.size();
    BOOST_CHECK_EQUAL_COLLECTIONS(nbegin, nend, node_list.begin(), node_list.end());
}

BOOST_AUTO_TEST_CASE(list_insert_init_list__mid)
{
    list_node.insert(++list_node.begin(), { "i-first", "i-second", "i-third" });
    BOOST_TEST_REQUIRE(list_node.size() == init_size + node_list.size());

    auto nbegin = ++list_node.begin();
    auto nend = nbegin + node_list.size();
    BOOST_CHECK_EQUAL_COLLECTIONS(nbegin, nend, node_list.begin(), node_list.end());
}

BOOST_AUTO_TEST_CASE(list_insert_init_list__end)
{
    list_node.insert(list_node.end(), { "i-first", "i-second", "i-third" });
    BOOST_TEST_REQUIRE(list_node.size() == init_size + node_list.size());

    auto nend = list_node.end();
    auto nbegin = nend - node_list.size();
    BOOST_CHECK_EQUAL_COLLECTIONS(nbegin, nend, node_list.begin(), node_list.end());
}

BOOST_AUTO_TEST_CASE(list_erase)
{
    list_node.erase(list_node.begin());
    BOOST_TEST_REQUIRE(list_node.size() == init_size - 1);
    BOOST_TEST(list_node.at(0) == "second");
}

BOOST_AUTO_TEST_CASE(list_erase_range)
{
    list_node.erase(list_node.begin(), --list_node.end());
    BOOST_TEST_REQUIRE(list_node.size() == 1);
    BOOST_TEST(list_node.at(0) == "third");
}

BOOST_AUTO_TEST_CASE(list_resize__grow)
{
    list_node.resize(init_size + 1);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.back() == node());
}

BOOST_AUTO_TEST_CASE(list_resize__shrink)
{
    list_node.resize(init_size - 1);
    BOOST_TEST_REQUIRE(list_node.size() == init_size - 1);
    BOOST_TEST(list_node.back() == node_vector.at(init_size - 2));
}

BOOST_AUTO_TEST_CASE(list_resize_with_value__grow)
{
    list_node.resize(init_size + 1, celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size + 1);
    BOOST_TEST(list_node.back() == celem);
}

BOOST_AUTO_TEST_CASE(list_resize_with_value__shrink)
{
    list_node.resize(init_size - 1, celem);
    BOOST_TEST_REQUIRE(list_node.size() == init_size - 1);
    BOOST_TEST(list_node.back() == node_vector.at(init_size - 2));
}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_CASE(swap__string)
{
    const node cnl("left"), cnr("right");
    node nl(cnl), nr(cnr);

    using std::swap;
    swap(nl, nr);

    BOOST_TEST(nl == cnr);
    BOOST_TEST(nr == cnl);
}

BOOST_AUTO_TEST_CASE(swap__list)
{
    const node cnl{ "left" }, cnr{ "right" };
    node nl(cnl), nr(cnr);

    using std::swap;
    swap(nl, nr);

    BOOST_TEST(nl == cnr);
    BOOST_TEST(nr == cnl);
}

BOOST_AUTO_TEST_CASE(swap__different)
{
    const node cnl("left"), cnr{ "right" };
    node nl(cnl), nr(cnr);

    using std::swap;
    swap(nl, nr);

    BOOST_TEST(nl == cnr);
    BOOST_TEST(nr == cnl);
}


BOOST_DATA_TEST_CASE(equality_case, bdata::make(std::initializer_list<node>{
    node(),
    "abcd",
    { "first", "second", "third" },
    { { "1first", "1second" }, "second",{ { "31first" }, "3second" } }
}))
{
    BOOST_TEST(sample == sample);
    BOOST_TEST(sample <= sample);
    BOOST_TEST(sample >= sample);
    BOOST_TEST(!(sample != sample));
    BOOST_TEST(!(sample < sample));
    BOOST_TEST(!(sample > sample));
}

std::vector<node> generate_comparison_nodes()
{
    static std::vector<node> data = {
        "",
        "abcd",
        "abcde",
        node(),
        { "first", "second", "third" },
        { "first", "second", "third", "fourth" },
        { "first", "zecond", "third" },
        { { "1first", "1second" }, "second",{ { "31first" }, "3second" } },
        { { "2first", "1second" }, "second",{ { "31first" }, "3second" } }
    };
    return data;
}

BOOST_DATA_TEST_CASE(comparison_test,
    (bdata::make(generate_comparison_nodes()) ^ bdata::xrange(generate_comparison_nodes().size()))
    * (bdata::make(generate_comparison_nodes()) ^ bdata::xrange(generate_comparison_nodes().size()))
    , val1, idx1, val2, idx2)
{
    BOOST_TEST((val1 == val2) == (idx1 == idx2));
    BOOST_TEST((val1 < val2) == (idx1 < idx2));
    BOOST_TEST((val1 <= val2) == (idx1 <= idx2));
    BOOST_TEST((val1 > val2) == (idx1 > idx2));
    BOOST_TEST((val1 >= val2) == (idx1 >= idx2));
}

BOOST_AUTO_TEST_SUITE_END()
