// Copyright 2017 Henrik Steffen Gaﬂmann
//
// Licensed under the MIT License.
// See LICENSE file in the project root for full license information.
//
#pragma once

#include <iostream>

#include <sexpr-cpp/data.hpp>

namespace sexpr
{

template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline std::ostream& operator<<(std::ostream& os,
        const basic_node<TString, TList, TStringTraits, TListTraits> &n)
{
    os << "{node:";
    if (n.is_list())
    {
        const auto &l = n.get_list();
        os << '(';
        for (auto it = l.cbegin(), end = l.cend(); it != end;)
        {
            os << *it;
            if (++it != end)
            {
                os << ' ';
            }
        }
        os << ')';
    }
    else
    {
        os << n.get_string();
    }
    return os << '}';
}

inline std::ostream& operator<<(std::ostream& os,
    const node_type &nt)
{
    return os << "{node_type::"
        << (nt == sexpr::node_type::list ? "list" : "string")
        << '}';
}

}

inline std::ostream& operator<<(std::ostream& os,
    const std::type_info &ti)
{
    return os << "{type_info:" << ti.name() << '}';
}

