/*
 * Copyright (C) 2007  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file db_settings.hh
 *  Usage setting for database:
 *
 *  There should be defined user database settings:
 *    Manager and used Connection factory, and following types
 *    defined as internal and depends on used connection manager type
 *    (Manager + Connection factory) - Connection, Result and Row types.
 */

#ifndef DB_SETTINGS_HH_496DFB2A05584145B21F5808014C6545
#define DB_SETTINGS_HH_496DFB2A05584145B21F5808014C6545

#include "src/util/db/database.hh"

namespace Database {

using StandaloneConnectionFactory = Factory::Simple<PSQLConnection>;
using StandaloneManager = Manager_<StandaloneConnectionFactory>;

using StandaloneConnection = StandaloneManager::connection_type;
using Result = StandaloneManager::result_type;
using Row = StandaloneManager::row_type;

}//namespace Database

#endif//DB_SETTINGS_HH_496DFB2A05584145B21F5808014C6545
