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
 *  @file
 *  IDN related utils
 */

#include "util/idn_utils.hh"

#include <idna.h>

namespace Util {

std::string::size_type get_utf8_char_len(const std::string& utf8str)
{
    std::string::size_type len = 0;
    for (std::string::size_type i = 0; i < utf8str.size(); ++i)
    {
        //count UTF-8 single byte characters and leading bytes of multiple byte characters,
        //i.e. bytes different from continuation bytes with 10xxxxxx bit pattern
        const unsigned char UTF8_CONTINUATION_BYTE_MASK = 0xc0;//mask of bits which differentiates single byte characters, leading bytes and continuation bytes
        const unsigned char UTF8_CONTINUATION_BYTE_PREFIX = 0x80;//result of bitwise ANDing continuation byte with mask
        len += (utf8str.at(i) & UTF8_CONTINUATION_BYTE_MASK) != UTF8_CONTINUATION_BYTE_PREFIX;
    }
    return len;
}

Optional<std::string> convert_utf8_to_punnycode(const std::string& utf8str)
{
    class UTF8ToPunnycode
    {
    public:
        explicit UTF8ToPunnycode(const std::string& utf8_str)
            : out_p(nullptr)
        {
            if (idna_to_ascii_8z(utf8_str.c_str(), &out_p, 0) != IDNA_SUCCESS)
            {
                free(out_p);
                out_p = nullptr;
            }
        }

        UTF8ToPunnycode() = delete;
        UTF8ToPunnycode(const UTF8ToPunnycode&) = delete;
        UTF8ToPunnycode& operator=(const UTF8ToPunnycode&) = delete;

        ~UTF8ToPunnycode()
        {
            free(out_p);
        }

        Optional<std::string> get()
        {
            if (out_p != nullptr)
            {
                return Optional<std::string>(std::string(out_p));
            }
            return Optional<std::string>();
        }
    private:
        char *out_p;
    };

    return UTF8ToPunnycode(utf8str).get();
}

}//namespace Util
