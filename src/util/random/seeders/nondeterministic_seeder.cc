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

#include "nondeterministic_seeder.hh"

#include <chrono>
#include <cstddef> // std::size_t
#include <cstdint> // std::uint32_t, std::int32_t
#include <iterator> // std::back_inserter
#include <mutex>
#include <random>
#include <type_traits>
#include <utility> // std::forward
#include <vector>

namespace Random {
namespace Seeders {
namespace {

// This is needed to prevent the compiler from generating shift-count-overflow warning
// when shifting a value by a number greater than or equal to the number of bits in
// its representation.
template<std::size_t bits,
         typename T,
         std::enable_if_t<std::is_integral<T>::value && (bits < sizeof(T) * 8), int> = 0>
void right_shift(T& _value)
{
    _value >>= bits;
}

template<std::size_t bits,
         typename T,
         std::enable_if_t<std::is_integral<T>::value && (bits >= sizeof(T) * 8), int> = 0>
void right_shift(T& _value)
{
    _value = 0;
}

template<typename Inserter, typename T>
void normalize(Inserter&& _inserter, T _source_value)
{
    static_assert(std::is_unsigned<typename Inserter::container_type::value_type>::value,
            "expected unsigned result type");
    static_assert(std::is_integral<T>::value, "expected integral source type");

    static constexpr std::size_t result_size = sizeof(typename Inserter::container_type::value_type);
    static constexpr std::size_t source_size = sizeof(T);
    static constexpr std::size_t chunks = (source_size + result_size - 1) / result_size;
    static_assert(chunks > 0, "zero chunks");
    static_assert(chunks * result_size >= source_size, "too few chunks");
    static_assert((chunks - 1) * result_size < source_size, "too many chunks");

    for (std::size_t i = 0; i < chunks; ++i)
    {
        _inserter = static_cast<typename Inserter::container_type::value_type>(_source_value);
        right_shift<result_size * 8>(_source_value);
    }
}

// Decomposes given values into chunks of type S, storing these using the given
// inserter. This is necessary in order to preserve all entropy contained in the
// input values, since std::seed_seq only uses the lower 32 bits of each value.
template<typename Inserter, typename T0, typename ...Ts>
void normalize(Inserter&& _inserter, T0 _first_source_value, Ts... _other_source_values)
{
    normalize(std::forward<Inserter>(_inserter), _first_source_value);
    normalize(std::forward<Inserter>(_inserter), _other_source_values...);
}

template<typename T>
static auto get_ticks()
{
    return T::now().time_since_epoch().count();
}

// Provide high quality seed values using hardware entropy source.
auto get_high_quality_seeds()
{
    std::random_device hw_entropy_source;
    // Combine output from random_device with time measurements from (possibly) different sources.
    // Note that high_resolution_clock may be an alias for system_clock or steady_clock
    // (system_clock in gcc 5.3.0).
    std::vector<std::uint32_t> seeds;
    normalize(
        std::back_inserter(seeds),
        hw_entropy_source(),
        get_ticks<std::chrono::high_resolution_clock>(),
        hw_entropy_source(),
        get_ticks<std::chrono::steady_clock>(),
        hw_entropy_source(),
        get_ticks<std::chrono::system_clock>(),
        hw_entropy_source());
    return seeds;
}

// Global generator initialized with a high-quality seed.
// Provides entropy for the NondeterministicSeeder.
// This approach was chosen in order to only use the hardware entropy source a constant
// number of times, during initialization.
class GlobalRandomNumberGenerator
{
public:
    class Access
    {
    public:
        Access()
            : generator_(GlobalRandomNumberGenerator::singleton()),
              locker_(generator_.mutex_)
        { }

        template<typename C>
        C generate_seeds() const
        {
            return generator_.template generate_seeds<C>();
        }
    private:
        GlobalRandomNumberGenerator& generator_;
        std::lock_guard<std::mutex> locker_;
    };

    // Combines output from the global pseudorandom generator with precise
    // time measurements and a simple counter.
    // @tparam C container for the generated seed values
    template<typename C>
    C generate_seeds()
    {
        typename std::remove_const<C>::type seed_container;
        normalize(
            std::back_inserter(seed_container),
            generator_(engine_),
            get_ticks<std::chrono::high_resolution_clock>(),
            generator_(engine_),
            get_ticks<std::chrono::steady_clock>(),
            generator_(engine_),
            get_ticks<std::chrono::system_clock>(),
            generator_(engine_),
            counter_);
        ++counter_;
        return seed_container;
    }
private:
    static GlobalRandomNumberGenerator& singleton()
    {
        static GlobalRandomNumberGenerator instance;
        return instance;
    }

    GlobalRandomNumberGenerator()
        : intermediate_seeds_(get_high_quality_seeds()),
          seeder_(intermediate_seeds_.cbegin(), intermediate_seeds_.cend()),
          engine_(seeder_),
          // Cast from unsigned to signed is implementation-defined,
          // but that doesn't bother us.
          counter_(static_cast<std::int32_t>(generator_(engine_)))
    { }

    // auxiliary values only used in the constructor
    const decltype(get_high_quality_seeds()) intermediate_seeds_;
    std::seed_seq seeder_;

    std::mutex mutex_;
    std::mt19937_64 engine_;
    std::uniform_int_distribution<std::uint32_t> generator_;
    std::int32_t counter_;
};

} //namespace Random::{anonymous}

NondeterministicSeeder::NondeterministicSeeder()
    : intermediate_seeds_(
            GlobalRandomNumberGenerator::Access()
                .template generate_seeds<decltype(intermediate_seeds_)>()),
      seed_(intermediate_seeds_.cbegin(), intermediate_seeds_.cend())
{ }

std::seed_seq& NondeterministicSeeder::get_seed()
{
    return seed_;
}

} //namespace Random::Seeders
} //namespace Random
