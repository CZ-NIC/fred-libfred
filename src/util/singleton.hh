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
 *  @file singleton.h
 *  \class Singleton
 *  \brief Template for simple implementing singleton pattern
 */

#ifndef SINGLETON_HH_C5F4DE21F764431C9B613A3D019D91A5
#define SINGLETON_HH_C5F4DE21F764431C9B613A3D019D91A5

template <class T>
class Singleton
{
public:
    /**
     * Getter
     * @return  stored instance pointer
     */
    static T* instance_ptr()
    {
        static T instance;
        return &instance;
    }

    /**
     * Getter
     * @return  stored instance reference
     */
    static T& instance_ref()
    {
        return *instance_ptr();
    }

	Singleton() = delete;

	Singleton(const Singleton&) = delete;

	~Singleton() = delete;

	Singleton& operator=(const Singleton&) = delete;
};

#endif//SINGLETON_HH_C5F4DE21F764431C9B613A3D019D91A5
