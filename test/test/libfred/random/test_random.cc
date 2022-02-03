/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "util/random/algorithm/boost_date.hh"
#include "util/random/algorithm/boost_time.hh"
#include "util/random/algorithm/floating_point.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"
#include "util/random/seeders/deterministic_seeder.hh"

#include "mock_engine.hh"
#include "mock_seeder.hh"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef> // std::size_t
#include <cstring> // std::strcmp
#include <future>
#include <iterator>
#include <limits>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept> // std::invalid_argument
#include <string>
#include <thread>
#include <vector>

BOOST_AUTO_TEST_SUITE(TestRandom)

// how many times a get<T>() operation should be checked
const int num_of_checks = 4;
// how long random strings/vectors should be generated
const int sequence_size = 100;
// lower bound for integer generation
const int int_min = std::numeric_limits<int>::min();
// upper bound for integer generation
const int int_max = std::numeric_limits<int>::max();


// * TEST UTILS *

template<typename T>
std::string vec_to_string(const std::vector<T>& v)
{
    std::ostringstream ss;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        if (i != 0)
        {
            ss << ", ";
        }
        ss << v[i];
    }
    return ss.str();
}

bool is_lower_alpha(char c)
{
    return c >= 'a' && c <= 'z';
}

bool is_upper_alpha(char c)
{
    return c >= 'A' && c <= 'Z';
}

bool is_num(char c)
{
    return c >= '0' && c <= '9';
}

bool is_alpha(char c)
{
    return is_lower_alpha(c) || is_upper_alpha(c);
}

bool is_alphanum(char c)
{
    return is_alpha(c) || is_num(c);
}


// * CREATING GENERATORS *

void reset_count()
{
    MockEngine::reset();
    MockSeeder::reset();
}

template<typename I, typename E, typename S>
void use_in_single_thread()
{
    for (int i = 0; i < 10; ++i)
    {
        Random::BasicGenerator<I, E, S>().template get(int_min, int_max);
    }
}

template<typename I, typename E, typename S>
void use_in_multiple_threads(std::size_t num_of_threads)
{
    const auto job = []()
    {
        Random::BasicGenerator<I, E, S>().template get(int_min, int_max);
    };

    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < num_of_threads; ++i)
    {
        std::thread t(job);
        threads.push_back(std::move(t));
    }
    for (std::thread& t : threads)
    {
        t.join();
    }
}

BOOST_AUTO_TEST_CASE(single_thread_global_instance_creation_test)
{
    reset_count();
    use_in_single_thread<Random::GlobalEngineInstance, MockEngine, MockSeeder>();

    BOOST_CHECK_EQUAL(MockEngine::get_number_created(), 1);
    BOOST_CHECK_EQUAL(MockSeeder::get_number_created(), 1);
}

BOOST_AUTO_TEST_CASE(single_thread_thread_local_creation_test)
{
    reset_count();
    use_in_single_thread<Random::ThreadLocalEngineInstance, MockEngine, MockSeeder>();

    BOOST_CHECK_EQUAL(MockEngine::get_number_created(), 1);
    BOOST_CHECK_EQUAL(MockSeeder::get_number_created(), 1);
}

// not feasible until Random::GlobalEngineInstance is thread-safe
//BOOST_AUTO_TEST_CASE(multi_thread_global_generator_creation_test)
//{
//    reset_count();
//    const std::size_t num_of_threads = 3;
//
//    use_in_multiple_threads<Random::GlobalEngineInstance, MockEngine, MockSeeder>(num_of_threads);
//
//    BOOST_CHECK_EQUAL(MockEngine::get_number_created(), 1);
//    BOOST_CHECK_EQUAL(MockSeeder::get_number_created(), 1);
//}

BOOST_AUTO_TEST_CASE(multi_thread_thread_local_generator_creation_test)
{
    reset_count();
    const std::size_t num_of_threads = 3;

    use_in_multiple_threads<Random::ThreadLocalEngineInstance, MockEngine, MockSeeder>(num_of_threads);

    BOOST_CHECK_EQUAL(MockEngine::get_number_created(), num_of_threads);
    BOOST_CHECK_EQUAL(MockSeeder::get_number_created(), num_of_threads);
}


// * SEEDING *

BOOST_AUTO_TEST_CASE(multi_thread_generator_test)
{
    const auto job = []() -> int
    {
       return Random::Generator().get(int_min, int_max);
    };

    const int num_of_threads = 3;
    std::vector<int> results;
    for (int i = 0; i < num_of_threads; ++i)
    {
        auto future = std::async(std::launch::async, job);
        results.push_back(future.get());
    }
    std::sort(results.begin(), results.end());
    for (std::size_t i = 1; i < results.size(); ++i)
    {
        if (results[i] == results[i - 1])
        {
            // for num_of_threads=3, 1-p ~ 7e-10
            BOOST_ERROR("Generator was most probably seeded with "
                        "the same seed in 2 different threads: "
                        << vec_to_string(results));
        }
    }
}

BOOST_AUTO_TEST_CASE(multi_thread_const_seed_generator_test)
{
    const auto job = []() -> int
    {
       return Random::BasicGenerator<
           Random::ThreadLocalEngineInstance,
           Random::DefaultEngine,
           Random::Seeders::DeterministicSeeder>()
               .get(int_min, int_max);
    };

    const int num_of_threads = 2;
    std::vector<int> results;
    for (int i = 0; i < num_of_threads; ++i)
    {
        auto future = std::async(std::launch::async, job);
        results.push_back(future.get());
    }
    // check if some values are different
    if (!std::equal(results.begin() + 1, results.end(), results.begin()))
    {
        BOOST_ERROR("Generator with DeterministicSeeder produced different outputs in different threads: "
                << vec_to_string(results));
    }
}

BOOST_AUTO_TEST_CASE(multi_thread_custom_seed_generator_const_seed_test)
{
    const unsigned int const_seed = 42;
    const auto job = []() -> int
    {
       return Random::BasicGenerator<
           Random::ThreadLocalEngineInstance,
           Random::DefaultEngine,
           Random::Seeders::CustomDeterministicSeeder<42>>()
               .get(int_min, int_max);
    };

    const int num_of_threads = 2;
    std::vector<int> results;
    for (int i = 0; i < num_of_threads; ++i)
    {
        auto future = std::async(std::launch::async, job);
        results.push_back(future.get());
    }
    // check if some values are different
    if (!std::equal(results.begin() + 1, results.end(), results.begin()))
    {
        BOOST_ERROR("Generator with CustomDeterministicSeeder produced different outputs in "
                    "different threads, even though it was given the same seed (" << const_seed << "): "
                    << vec_to_string(results));
    }
}

BOOST_AUTO_TEST_CASE(multi_thread_custom_seed_generator_test)
{
    const auto job_A = []() -> int
    {
       return Random::BasicGenerator<
           Random::ThreadLocalEngineInstance,
           Random::DefaultEngine,
           Random::Seeders::CustomDeterministicSeeder<0>>()
               .get(int_min, int_max);
    };

    const auto job_B = []() -> int
    {
       return Random::BasicGenerator<
           Random::ThreadLocalEngineInstance,
           Random::DefaultEngine,
           Random::Seeders::CustomDeterministicSeeder<1>>()
               .get(int_min, int_max);
    };

    auto future_A = std::async(std::launch::async, job_A);
    auto future_B = std::async(std::launch::async, job_B);
    const int result_A = future_A.get();
    const int result_B = future_B.get();

    if (result_A == result_B)
    {
        BOOST_ERROR("Generators with CustomDeterministicSeeder produced the same outputs "
                    "in 2 different threads, even though they were initialized with different seeds. "
                    "Output: " << result_A);
    }
}


// * RANDOM NUMBERS *

BOOST_AUTO_TEST_CASE(random_integral_number_range_test)
{
    const int min = -100;
    const int max = 100;
    const int i = Random::Generator().get(min, max);
    BOOST_CHECK_MESSAGE(i >= min && i <= max, "i was " << i);
}

BOOST_AUTO_TEST_CASE(random_real_number_range_test)
{
    const double min = -100.0;
    const double max = 100.0;
    const double d = Random::Generator().get(min, max);
    BOOST_CHECK_MESSAGE(d >= min && d <= max, "d was " << d);
    BOOST_CHECK_MESSAGE(
            d != static_cast<int>(d),
            "d was probably generated as an integer: " << d);
}

BOOST_AUTO_TEST_CASE(random_integral_inclusiveness_test)
{
    const int boundary = 42;
    const int i = Random::Generator().get(boundary, boundary);
    BOOST_CHECK_MESSAGE(i == boundary, "i was " << i);
}

BOOST_AUTO_TEST_CASE(random_integral_incorrect_bounds_order_test)
{
    BOOST_CHECK_THROW(
            Random::Generator().get(1, 0),
            std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(random_real_number_incorrect_bounds_order_test)
{
    BOOST_CHECK_THROW(
            Random::Generator().get(1.0, 0.0),
            std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(random_real_number_empty_interval_test)
{
    BOOST_CHECK_THROW(
            Random::Generator().get(1.0, 1.0),
            std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(random_real_number_span_too_large_test)
{
    BOOST_CHECK_THROW(
            Random::Generator().get(
                std::numeric_limits<double>::lowest(),
                std::numeric_limits<double>::max()),
            std::invalid_argument);
}


// * RANDOM STRINGS *

BOOST_AUTO_TEST_CASE(string_custom_char_set_composition_test)
{
    const char char_set_arr[] = "abftgxyz";
    const std::set<char> char_set(std::cbegin(char_set_arr), std::cend(char_set_arr));
    const std::string s = Random::Generator().get_seq(char_set_arr, sequence_size);
    BOOST_CHECK_EQUAL(s.size(), sequence_size);
    const auto is_valid = [&char_set](char c)
    {
        return char_set.find(c) != char_set.end();
    };
    if (!std::all_of(s.begin(), s.end(), is_valid))
    {
        BOOST_ERROR("output from get_seq<char>(\"" << char_set_arr << "\", )"
                << sequence_size << " was: " << s);
    }
}

BOOST_AUTO_TEST_CASE(string_lower_alpha_composition_test)
{
    const std::string s = Random::Generator().get_seq(Random::CharSet::small_letters(), sequence_size);
    BOOST_CHECK_EQUAL(s.size(), sequence_size);
    if (!std::all_of(s.begin(), s.end(), is_lower_alpha))
    {
        BOOST_ERROR("output from get_seq(lower_alpha) was: " << s);
    }
}

BOOST_AUTO_TEST_CASE(string_upper_alpha_composition_test)
{
    const std::string s = Random::Generator().get_seq(Random::CharSet::capital_letters(), sequence_size);
    BOOST_CHECK_EQUAL(s.size(), sequence_size);
    if (!std::all_of(s.begin(), s.end(), is_upper_alpha))
    {
        BOOST_ERROR("output from get_seq(upper_alpha) was: " << s);
    }
}

BOOST_AUTO_TEST_CASE(string_alpha_composition_test)
{
    const std::string s = Random::Generator().get_seq(Random::CharSet::letters(), sequence_size);
    BOOST_CHECK_EQUAL(s.size(), sequence_size);
    if (!std::all_of(s.begin(), s.end(), is_alpha))
    {
        BOOST_ERROR("output from get_seq(alpha) was: " << s);
    }
}

BOOST_AUTO_TEST_CASE(string_alphanum_composition_test)
{
    const std::string s = Random::Generator().get_seq(Random::CharSet::letters_and_digits(), sequence_size);
    BOOST_CHECK_EQUAL(s.size(), sequence_size);
    if (!std::all_of(s.begin(), s.end(), is_alphanum))
    {
        BOOST_ERROR("output from get_seq(alphanum) was: " << s);
    }
}

BOOST_AUTO_TEST_CASE(string_num_composition_test)
{
    const std::string s = Random::Generator().get_seq(Random::CharSet::digits(), sequence_size);
    BOOST_CHECK_EQUAL(s.size(), sequence_size);
    if (!std::all_of(s.begin(), s.end(), is_num))
    {
        BOOST_ERROR("output from get_seq(num) was: " << s);
    }
}


// * RANDOM VECTORS *

BOOST_AUTO_TEST_CASE(random_vector_composition_test)
{
    const int value_set_arr[] = {1, 4, 6, 9, 11, 15, 23, 48};
    const std::set<char> value_set(std::cbegin(value_set_arr), std::cend(value_set_arr));

    const std::vector<int> random_vector = Random::Generator().get_seq(value_set_arr, sequence_size);

    BOOST_CHECK_EQUAL(random_vector.size(), sequence_size);
    const auto is_valid = [&value_set](int value) { return value_set.find(value) != value_set.end(); };
    if (!std::all_of(random_vector.begin(), random_vector.end(), is_valid))
    {
        BOOST_ERROR("random vector contained values outside the value set: ["
                << vec_to_string(random_vector)
                << "]");
    }
}


// * RANDOM CHARACTERS *

BOOST_AUTO_TEST_CASE(char_custom_char_set_test)
{
    const char char_set_arr[] = "abftgxyz";
    const std::set<char> char_set(std::cbegin(char_set_arr), std::cend(char_set_arr));
    for (int i = 0; i < num_of_checks; ++i)
    {
        const char rand_char = Random::Generator().get(char_set_arr);
        BOOST_REQUIRE_MESSAGE(
                char_set.find(rand_char) != char_set.end(),
                "get<char>(\"" << char_set_arr << "\") returned: " << rand_char);
    }
}

BOOST_AUTO_TEST_CASE(char_lower_alpha_test)
{
    for (int i = 0; i < num_of_checks; ++i)
    {
        const char rand_char = Random::Generator().get(Random::CharSet::small_letters());
        BOOST_REQUIRE_MESSAGE(is_lower_alpha(rand_char), "rand_char was " << rand_char);
    }
}

BOOST_AUTO_TEST_CASE(char_upper_alpha_test)
{
    for (int i = 0; i < num_of_checks; ++i)
    {
        const char rand_char = Random::Generator().get(Random::CharSet::capital_letters());
        BOOST_REQUIRE_MESSAGE(is_upper_alpha(rand_char), "rand_char was " << rand_char);
    }
}

BOOST_AUTO_TEST_CASE(char_alpha_test)
{
    for (int i = 0; i < num_of_checks; ++i)
    {
        const char rand_char = Random::Generator().get(Random::CharSet::letters());
        BOOST_REQUIRE_MESSAGE(is_alpha(rand_char), "rand_char was " << rand_char);
    }
}

BOOST_AUTO_TEST_CASE(char_alphanum_test)
{
    for (int i = 0; i < num_of_checks; ++i)
    {
        const char rand_char = Random::Generator().get(Random::CharSet::letters_and_digits());
        BOOST_REQUIRE_MESSAGE(is_alphanum(rand_char), "rand_char was " << rand_char);
    }
}

BOOST_AUTO_TEST_CASE(char_num_test)
{
    for (int i = 0; i < num_of_checks; ++i)
    {
        const char rand_char = Random::Generator().get(Random::CharSet::digits());
        BOOST_REQUIRE_MESSAGE(is_num(rand_char), "rand_char was " << rand_char);
    }
}


// * RANDOM DATE & TIME *

BOOST_AUTO_TEST_CASE(random_date_range_test)
{
    const auto from = boost::gregorian::date(2001, boost::gregorian::Mar, 14);
    const auto to = boost::gregorian::date(2003, boost::gregorian::Apr, 21);
    for (int i = 0; i < num_of_checks; ++i)
    {
        const auto rand_date = Random::Generator().get(from, to);
        const bool valid = rand_date >= from && rand_date <= to;
        BOOST_REQUIRE_MESSAGE(valid, "rand_date was " << rand_date);
    }
}

BOOST_AUTO_TEST_CASE(random_date_inclusiveness_test)
{
    const auto date = boost::gregorian::date(2000, boost::gregorian::Mar, 14);
    for (int i = 0; i < num_of_checks; ++i)
    {
        const auto rand_date = Random::Generator().get(date, date);
        const bool valid = rand_date == date;
        BOOST_REQUIRE_MESSAGE(valid, "rand_date was " << rand_date);
    }
}

BOOST_AUTO_TEST_CASE(date_incorrect_bounds_order_test)
{
    const auto lower = boost::gregorian::date(2000, boost::gregorian::Mar, 14);
    const auto upper = boost::gregorian::date(2003, boost::gregorian::Apr, 21);
    BOOST_CHECK_THROW(
            Random::Generator().get(upper, lower),
            std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(random_time_range_test)
{
    const auto from = boost::posix_time::ptime(
            boost::gregorian::date(2001, boost::gregorian::Mar, 14),
            boost::posix_time::time_duration(10, 30, 27, 0));
    const auto to = boost::posix_time::ptime(
            boost::gregorian::date(2020, boost::gregorian::Jul, 14),
            boost::posix_time::time_duration(20, 30, 27, 0));
    for (int i = 0; i < num_of_checks; ++i)
    {
        const auto rand_time = Random::Generator().get(from, to);
        const bool valid = rand_time >= from && rand_time <= to;
        BOOST_REQUIRE_MESSAGE(valid, "rand_time was " << rand_time);
    }
}

BOOST_AUTO_TEST_CASE(random_time_inclusiveness_test)
{
    const auto boundary = boost::posix_time::ptime(
            boost::gregorian::date(2001, boost::gregorian::Mar, 14),
            boost::posix_time::time_duration(10, 30, 27, 123456));
    for (int i = 0; i < num_of_checks; ++i)
    {
        const auto rand_time = Random::Generator().get(boundary, boundary);
        BOOST_REQUIRE_MESSAGE(rand_time == boundary, "rand_time was " << rand_time);
    }
}

BOOST_AUTO_TEST_CASE(time_incorrect_bounds_order_test)
{
    const auto lower = boost::posix_time::ptime(
            boost::gregorian::date(2000, boost::gregorian::Mar, 14),
            boost::posix_time::time_duration(10, 30, 27, 0));
    const auto upper = boost::posix_time::ptime(
            boost::gregorian::date(2020, boost::gregorian::Jul, 14),
            boost::posix_time::time_duration(20, 30, 27, 0));
    BOOST_CHECK_THROW(
            Random::Generator().get(upper, lower),
            std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestRandom
