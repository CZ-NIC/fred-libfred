/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
/**
 *  @file factory_check.h
 *  Comparison of registered implementations with requirements.
 */

#ifndef FACTORY_CHECK_HH_32E8EF58E9B844A89ABA5459C1AA8D6A
#define FACTORY_CHECK_HH_32E8EF58E9B844A89ABA5459C1AA8D6A

#include <stdexcept>
#include <string>
#include <algorithm>

#include "util/factory.hh"
#include "util/util.hh"

///check if required keys includes all keys from FACTORY
/// which would be instance of Util::Factory from factory.h
/// example: FactoryHaveSubsetOfKeysChecker<SomeFactory>(required_keys).check();

template <class FACTORY> class FactoryHaveSubsetOfKeysChecker
{
public:
    typedef  std::vector< typename FACTORY::key_type >
                KeyVector;
private:
    KeyVector required_keys_;
public:
    explicit FactoryHaveSubsetOfKeysChecker(KeyVector _required_keys)
    : required_keys_(_required_keys)
    {}
    void check()
    {
        bool throw_not_found_keys = false;
        KeyVector not_found_keys_vector;

        KeyVector key_vector = FACTORY::instance_ref().get_keys();
        for (typename KeyVector::iterator i = key_vector.begin()
                        ; i != key_vector.end(); ++i)
        {
            //look for FACTORY keys in required_keys
            if (std::find(required_keys_.begin(), required_keys_.end(), *i)
                == required_keys_.end())
            {
                throw_not_found_keys = true;
                not_found_keys_vector.push_back(*i);
            }
        }//for

        //throw if not found
        if (throw_not_found_keys)
        {
            throw std::runtime_error(
                "FactoryHaveSubsetOfKeysChecker: unable to find this keys from FACTORY in required_keys: "
                + Util::container2comma_list(not_found_keys_vector));
        }
    }//check()
};

///check if FACTORY which would be instance of Util::Factory from factory.h
/// includes required keys
/// example: FactoryHaveSupersetOfKeysChecker<SomeFactory>(required_keys).check();

template <class FACTORY> class FactoryHaveSupersetOfKeysChecker
{
public:
    typedef  std::vector< typename FACTORY::key_type >
                KeyVector;
private:
    KeyVector required_keys_;
public:
    explicit FactoryHaveSupersetOfKeysChecker(KeyVector _required_keys)
    : required_keys_(_required_keys)
    {}
    void check()
    {
        bool throw_not_found_keys = false;
        KeyVector not_found_keys_vector;

        KeyVector key_vector = FACTORY::instance_ref().get_keys();
        for (typename KeyVector::iterator i = required_keys_.begin()
                        ; i != required_keys_.end(); ++i)
        {
            //look for required keys in FACTORY
            if (std::find(key_vector.begin(), key_vector.end(), *i)
                == key_vector.end())
            {
                throw_not_found_keys = true;
                not_found_keys_vector.push_back(*i);
            }
        }//for

        //throw if not found
        if (throw_not_found_keys)
        {
            throw std::runtime_error(
                "FactoryHaveSupersetOfKeysChecker: unable to find this required keys in FACTORY: "
                + Util::container2comma_list(not_found_keys_vector));
        }
    }//check()
};

//get implementation names begining with prefix from factory
template <class FACTORY> std::vector<std::string> get_names_begining_with_prefix_from_factory (const std::string& prefix)
{
    typedef  std::vector< typename FACTORY::key_type > KeyVector;
    std::vector<std::string> names;
    KeyVector key_vector = FACTORY::instance_ref().get_keys();
    for (typename KeyVector::const_iterator i = key_vector.begin()
                    ; i != key_vector.end(); ++i)
    {
        if (i->substr(0, prefix.length()).compare(prefix) == 0)
        {
            names.push_back(*i);
        }
    }//for
    return names;

}

#endif
