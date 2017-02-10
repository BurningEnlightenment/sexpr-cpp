// Copyright 2015 Henrik Steffen Gaßmann
//
// Licensed under the MIT License.
// See LICENSE file in the project root for full license information.
//
#pragma once

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
