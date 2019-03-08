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
#ifndef MAILER_HH_20C1B607DE314B78AF5A8EE057FCAA07
#define MAILER_HH_20C1B607DE314B78AF5A8EE057FCAA07

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include "libfred/types.hh"

namespace LibFred
{
  namespace Mailer
  {
    typedef std::map<std::string, std::string> Parameters;
    typedef std::vector<std::string> Handles;
    typedef std::vector<TID> Attachments;
    // Exception thrown when mail cannot be send
    struct NOT_SEND : public std::runtime_error
    {
        NOT_SEND()
                : std::runtime_error("Mailer NOT_SEND")
        {}
    };
    class Manager
    {
     public:
      virtual ~Manager() {}
      virtual TID sendEmail(
        const std::string& from,
        const std::string& to,
        const std::string& subject,
        const std::string& mailTemplate,
        const Parameters &params,
        const Handles &handles,
        const Attachments &attach,
        const std::string& reply_to = std::string("")
      ) = 0;
      virtual bool checkEmailList(std::string &_email_list) const = 0;
    }; // Manager
  }; // Mailer
}; // Fred

#endif
