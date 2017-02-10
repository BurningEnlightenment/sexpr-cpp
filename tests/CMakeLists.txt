# Written in 2017 by Henrik Steffen Gaßmann <henrik@gassmann.onl>
#
# To the extent possible under law, the author(s) have dedicated all
# copyright and related and neighboring rights to this software to the
# public domain worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication
# along with this software. If not, see
#
#     http://creativecommons.org/publicdomain/zero/1.0/
#
########################################################################

set(_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/sexpr-cpp")

add_executable(sexpr-cpp-tests
    sexpr-cpp-tests.cpp
    boost-unit-test.hpp


    # project headers for IDE support
    "${_INCLUDE_DIR}/data.hpp"
)
target_link_libraries(sexpr-cpp-tests
    PUBLIC
        sexpr-cpp
        Boost::unit_test_framework
)

add_test(NAME sexpr-cpp-tests
    COMMAND sexpr-cpp-tests
)
