// Copyright 2017 Henrik Steffen Gaßmann
//
// Licensed under the MIT License.
// See LICENSE file in the project root for full license information.
//
#pragma once

#include <cstddef>
#include <cstdint>

#include <string>
#include <vector>
#include <iterator>
#include <typeinfo>
#include <stdexcept>
#include <type_traits>

#include <boost/variant.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/reverse_iterator.hpp>


namespace sexpr
{

template< class T >
struct std_string_traits
{
    static int compare(const T &lhs, const T &rhs)
    {
        return lhs.compare(rhs);
    }
};

template< template<class, class...> class T >
struct std_list_traits
{
    template< typename TComparator, class... TListTArgs >
    static int compare(const T<TListTArgs...> &lhs, const T<TListTArgs...> &rhs, TComparator comp)
    {
        auto lit = lhs.cbegin(),
            lend = lhs.cend(),
            rit = rhs.cbegin(),
            rend = rhs.cend();

        for (; lit != lend && rit != rend; ++lit, ++rit)
        {
            if (auto i = comp(*lit, *lend))
            {
                return i;
            }
        }
        return (lit != lend) - (rit != rend);
    }
};


template< class TString,
    template<class, class...> class TList,
    class TStringTraits = std_string_traits<TString>,
    class TListTraits = std_list_traits<TList> >
class basic_node;

enum class node_type
{
    list = 0,
    string = 1,
};


template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
class basic_node
{
public:
    using string = TString;
    using string_traits = TStringTraits;
    using list = TList<basic_node>;
    using list_traits = TListTraits;
    struct combined_traits
        : string_traits
        , list_traits
    {
    };

private:
    using content = boost::variant<list, string>;

    template< class T >
    class basic_iterator
        : public boost::iterator_adaptor< basic_iterator<T>,
             std::conditional_t< std::is_const<T>::value, typename list::const_iterator, typename list::iterator > >
    {
        friend class boost::iterator_core_access;

        using base_iterator_t = std::conditional_t< std::is_const<T>::value, typename list::const_iterator, typename list::iterator >;
    public:
        basic_iterator() = default;

        explicit basic_iterator(base_iterator_t v)
            : basic_iterator::iterator_adaptor_(v)
        {
        }

        template<class OtherValue,
            std::enable_if_t<std::is_convertible<OtherValue*, T*>::value, int> = 0>
        basic_iterator(const basic_iterator<OtherValue> &other)
            : basic_iterator::iterator_adaptor_(other.base())
        {
        }
    };

public:
    using type = node_type;

    using value_type = basic_node;
    using size_type = typename list::size_type;
    using difference_type = typename list::difference_type;
    using reference = basic_node &;
    using const_reference = const basic_node &;

    using iterator = basic_iterator<basic_node>;
    using const_iterator = basic_iterator<const basic_node>;
    using reverse_iterator = boost::reverse_iterator<iterator>;
    using const_reverse_iterator = boost::reverse_iterator<const_iterator>;

    basic_node() = default;
    basic_node(string s)
        : mContent(s)
    {
    }
    template< std::size_t n >
    basic_node(const char (&str)[n], bool remove_trailing_null = true)
        : mContent( std::string{str, n - (remove_trailing_null && !str[n-1]) } )
    {
        static_assert(n, "you tried to initialize a node with a zero sized char array (which itself is illegal C++ anyway)");
    }
    template< class TInputIterator,
        std::enable_if_t<
            std::is_base_of<
                std::forward_iterator_tag,
                typename std::iterator_traits<TInputIterator>::iterator_category
            >::value, int
        > = 0
    >
    basic_node(TInputIterator first, TInputIterator last)
        : mContent( list(first, last) )
    {
    }
    basic_node(std::initializer_list<basic_node> il)
        : mContent( list(il) )
    {
    }

    explicit operator string() const
    {
        if (auto ps = try_get_as<string>())
        {
            return *ps;
        }
        throw std::domain_error("basic_node::operator string() can only be used with strings");
    }

    type which() const noexcept
    {
        return static_cast<type>(mContent.which());
    }
    bool is_list() const noexcept
    {
        return which() == type::list;
    }
    bool is_string() const noexcept
    {
        return which() == type::string;
    }
    const std::type_info & type_info() const noexcept
    {
        return mContent.type();
    }

    string & get_string()
    {
        if (auto ps = try_get_as<string>())
        {
            return *ps;
        }
        throw std::domain_error("basic_node::get_string() can only be used with strings");
    }
    string * try_get_string() noexcept
    {
        return try_get_as<string>();
    }
    const string & get_string() const
    {
        if (auto ps = try_get_as<string>())
        {
            return *ps;
        }
        throw std::domain_error("basic_node::get_string() can only be used with strings");
    }
    const string * try_get_string() const noexcept
    {
        return try_get_as<string>();
    }

    list & get_list()
    {
        if (auto pl = try_get_as<list>())
        {
            return *pl;
        }
        throw std::domain_error("basic_node::get_list() can only be used with lists");
    }
    list * try_get_list() noexcept
    {
        return try_get_as<list>();
    }
    const list & get_list() const
    {
        if (auto pl = try_get_as<list>())
        {
            return *pl;
        }
        throw std::domain_error("basic_node::get_list() can only be used with lists");
    }
    const list * try_get_list() const noexcept
    {
        return try_get_as<list>();
    }


    #pragma region iterator methods [begin, end)

    iterator begin()
    {
        if (auto pl = try_get_as<list>())
        {
            return iterator (pl->begin());
        }
        throw std::domain_error("basic_node::begin can only be used with lists");
    }
    const_iterator begin() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_iterator(pl->begin());
        }
        throw std::domain_error("basic_node::begin can only be used with lists");
    }
    const_iterator cbegin() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_iterator(pl->begin());
        }
        throw std::domain_error("basic_node::cbegin can only be used with lists");
    }
    iterator end()
    {
        if (auto pl = try_get_as<list>())
        {
            return iterator(pl->end());
        }
        throw std::domain_error("basic_node::end can only be used with lists");
    }
    const_iterator end() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_iterator(pl->end());
        }
        throw std::domain_error("basic_node::end can only be used with lists");
    }
    const_iterator cend() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_iterator(pl->end());
        }
        throw std::domain_error("basic_node::cend can only be used with lists");
    }

    reverse_iterator rbegin()
    {
        if (auto pl = try_get_as<list>())
        {
            return reverse_iterator(iterator(pl->end()));
        }
        throw std::domain_error("basic_node::begin can only be used with lists");
    }
    const_reverse_iterator rbegin() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_reverse_iterator(const_iterator(pl->end()));
        }
        throw std::domain_error("basic_node::begin can only be used with lists");
    }
    const_reverse_iterator crbegin() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_reverse_iterator(const_iterator(pl->end()));
        }
        throw std::domain_error("basic_node::cbegin can only be used with lists");
    }
    reverse_iterator rend()
    {
        if (auto pl = try_get_as<list>())
        {
            return reverse_iterator(iterator(pl->begin()));
        }
        throw std::domain_error("basic_node::end can only be used with lists");
    }
    const_reverse_iterator rend() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_reverse_iterator(const_iterator(pl->begin()));
        }
        throw std::domain_error("basic_node::end can only be used with lists");
    }
    const_reverse_iterator crend() const
    {
        if (auto pl = try_get_as<list>())
        {
            return const_reverse_iterator(const_iterator(pl->begin()));
        }
        throw std::domain_error("basic_node::cend can only be used with lists");
    }

    #pragma endregion


    reference at(size_type idx)
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->at(idx);
        }
        throw std::domain_error("basic_node::at() can only be used with lists");
    }
    const_reference at(size_type idx) const
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->at(idx);
        }
        throw std::domain_error("basic_node::at() can only be used with lists");
    }

    reference operator[](size_type idx)
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->operator[](idx);
        }
        throw std::domain_error("basic_node::operator[] can only be used with lists");
    }
    const_reference operator[](size_type idx) const
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->operator[](idx);
        }
        throw std::domain_error("basic_node::operator[] can only be used with lists");
    }

    reference front()
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->front();
        }
        throw std::domain_error("basic_node::front() can only be used with lists");
    }
    const_reference front() const
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->front();
        }
        throw std::domain_error("basic_node::front() can only be used with lists");
    }

    reference back()
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->back();
        }
        throw std::domain_error("basic_node::back() can only be used with lists");
    }
    const_reference back() const
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->back();
        }
        throw std::domain_error("basic_node::back() can only be used with lists");
    }


    bool empty() const
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->empty();
        }
        return false;
    }

    size_type size() const
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->size();
        }
        return 1;
    }

    size_type max_size() const
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->max_size();
        }
        return 1;
    }


    void clear()
    {
        boost::apply_visitor([](auto &e) { e.clear(); }, mContent);
    }

    iterator insert(const_iterator pos, const basic_node &value)
    {
        if (auto pl = try_get_as<list>())
        {
            return iterator(pl->insert(pos.base(), value));
        }
        throw std::domain_error("basic_node::insert() can only be used with lists");
    }
    iterator insert(const_iterator pos, basic_node &&value)
    {
        if (auto pl = try_get_as<list>())
        {
            return iterator(pl->insert(pos.base(), std::forward<basic_node>(value)));
        }
        throw std::domain_error("basic_node::insert() can only be used with lists");
    }
    iterator insert(const_iterator pos, size_type count, const basic_node &value)
    {
        if (auto pl = try_get_as<list>())
        {
            return iterator(pl->insert(pos.base(), count, value));
        }
        throw std::domain_error("basic_node::insert() can only be used with lists");
    }
    template< class TInputIterator >
    iterator insert(const_iterator pos, TInputIterator first, TInputIterator last)
    {
        if (auto pl = try_get_as<list>())
        {
            return iterator(pl->insert(pos.base(), first, last));
        }
        throw std::domain_error("basic_node::insert() can only be used with lists");
    }
    iterator insert(const_iterator pos, std::initializer_list<basic_node> il)
    {
        if (auto pl = try_get_as<list>())
        {
            return iterator(pl->insert(pos.base(), std::move(il)));
        }
        throw std::domain_error("basic_node::insert() can only be used with lists");
    }

    void erase(const_iterator pos)
    {
        if (auto pl = try_get_as<list>())
        {
            pl->erase(pos.base());
        }
        else
        {
            throw std::domain_error("basic_node::erase() can only be used with lists");
        }
    }
    void erase(const_iterator first, const_iterator last)
    {
        if (auto pl = try_get_as<list>())
        {
            pl->erase(first.base(), last.base());
        }
        else
        {
            throw std::domain_error("basic_node::erase() can only be used with lists");
        }
    }

    void push_back(const basic_node &value)
    {
        if (auto pl = try_get_as<list>())
        {
            pl->push_back(value);
        }
        else
        {
            throw std::domain_error("basic_node::erase() can only be used with lists");
        }
    }
    void push_back(basic_node &&value)
    {
        if (auto pl = try_get_as<list>())
        {
            pl->push_back(std::forward<basic_node>(value));
        }
        else
        {
            throw std::domain_error("basic_node::erase() can only be used with lists");
        }
    }

    template< typename... TArgs >
    auto emplace_back(TArgs&&... args)
    {
        if (auto pl = try_get_as<list>())
        {
            return pl->emplace_back(std::forward<TArgs>(args)...);
        }
        throw std::domain_error("basic_node::emplace_back() can only be used with lists");
    }

    void pop_back()
    {
        if (auto pl = try_get_as<list>())
        {
            pl->pop_back();
        }
        else
        {
            throw std::domain_error("basic_node::erase() can only be used with lists");
        }
    }

    void resize(size_type count)
    {
        if (auto pl = try_get_as<list>())
        {
            pl->resize(count);
        }
        else
        {
            throw std::domain_error("basic_node::erase() can only be used with lists");
        }
    }
    void resize(size_type count, const basic_node &value)
    {
        if (auto pl = try_get_as<list>())
        {
            pl->resize(count, value);
        }
        else
        {
            throw std::domain_error("basic_node::erase() can only be used with lists");
        }
    }

    void swap(basic_node &other)
        noexcept(noexcept(std::declval<content>().swap(other.mContent)))
    {
        mContent.swap(other.mContent);
    }


private:
    template< class T >
    T * try_get_as() noexcept
    {
        return boost::get<T>(&mContent);
    }
    template< class T >
    const T * try_get_as() const noexcept
    {
        return boost::get<const T>(&mContent);
    }

    content mContent;
};


template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline bool operator==(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
        const basic_node<TString, TList, TStringTraits, TListTraits> &rhs)
{
    auto lt = lhs.which();
    if (lt == rhs.which())
    {
        return lt == node_type::list
            ? lhs.get_list() == rhs.get_list()
            : lhs.get_string() == rhs.get_string();
    }
    return false;
}

template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline bool operator!=(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
        const basic_node<TString, TList, TStringTraits, TListTraits> &rhs)
{
    return !(operator==(lhs, rhs));
}


template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline bool operator==(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
        const typename basic_node<TString, TList, TStringTraits, TListTraits>::string &rhs)
{
    return lhs.is_string() && lhs.get_string() == rhs;
}

template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
    inline bool operator!=(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
        const typename basic_node<TString, TList, TStringTraits, TListTraits>::string &rhs)
{
    return !(operator==(lhs, rhs));
}


namespace detail
{
template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline int compare(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
    const basic_node<TString, TList, TStringTraits, TListTraits> &rhs)
{
    //using node_t = basic_node<TString, TList, TStringTraits, TListTraits>;

    auto lt = lhs.which();
    auto rt = rhs.which();
    if (lt == rt)
    {
        if (lt == node_type::list)
        {
            return TListTraits::compare(lhs.get_list(), rhs.get_list(),
                [](const auto &le, const auto &re)
            {
                return compare(le, re);
            });
        }

        return TStringTraits::compare(lhs.get_string(), rhs.get_string());
    }

    return (lt == node_type::list) - (lt == node_type::string);
}
}

template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline bool operator<(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
    const basic_node<TString, TList, TStringTraits, TListTraits> &rhs)
{
    return detail::compare(lhs, rhs) < 0;
}

template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline bool operator>(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
        const basic_node<TString, TList, TStringTraits, TListTraits> &rhs)
{
    return detail::compare(lhs, rhs) > 0;
}

template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline bool operator<=(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
        const basic_node<TString, TList, TStringTraits, TListTraits> &rhs)
{
    return detail::compare(lhs, rhs) <= 0;
}

template< class TString,
    template<class, class...> class TList,
    class TStringTraits,
    class TListTraits >
inline bool operator>=(const basic_node<TString, TList, TStringTraits, TListTraits> &lhs,
        const basic_node<TString, TList, TStringTraits, TListTraits> &rhs)
{
    return detail::compare(lhs, rhs) >= 0;
}



template< class TString, template<class, class...> class TList >
inline void swap(basic_node<TString, TList> &lhs, basic_node<TString, TList> &rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

using node = basic_node<std::string, std::vector>;

}
