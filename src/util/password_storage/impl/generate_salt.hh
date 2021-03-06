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
#ifndef GENERATE_SALT_HH_0C3EAFE249E541CF875E953F4DB90745
#define GENERATE_SALT_HH_0C3EAFE249E541CF875E953F4DB90745

#include "util/password_storage/base64.hh"

#include <random>

namespace PasswordStorage {
namespace Impl {

inline BinaryData generate_salt(int bytes)
{
    std::random_device source_of_randomness;
    std::mt19937_64 generate_random_number(source_of_randomness());
    std::unique_ptr<unsigned char[]> salt_data(new unsigned char[bytes]);
    {
        unsigned char* data = salt_data.get();
        const unsigned char* const data_end = data + bytes;
        while (data != data_end)
        {
            auto random_value = generate_random_number();
            for (unsigned cnt = 0; cnt < sizeof(random_value); ++cnt)
            {
                *data = random_value & 0xff;
                ++data;
                if (data == data_end)
                {
                    break;
                }
                random_value >>= 8;
            }
        }
    }
    return BinaryData::from_raw_binary_data(std::move(salt_data), bytes);
}

}//namespace PasswordStorage::Impl
}//namespace PasswordStorage

#endif
