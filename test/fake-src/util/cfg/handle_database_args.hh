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
 *  @handle_database_args.hh
 *  database dependent args handling
 */

#ifndef HANDLE_DATABASE_ARGS_HH_932E1219F77B4A3F9E7B1DB0E25CB8E1
#define HANDLE_DATABASE_ARGS_HH_932E1219F77B4A3F9E7B1DB0E25CB8E1

#include "test/fake-src/util/cfg/faked_args.hh"
#include "test/fake-src/util/cfg/handle_args.hh"
#include "libfred/db_settings.hh"

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <exception>
#include <string>
#include <vector>

/**
 * \class HandleDatabaseArgs
 * \brief database options handler
 */
class HandleDatabaseArgs : public HandleArgs
{
protected:
    std::string conn_info;
public:

    std::shared_ptr<boost::program_options::options_description>
        get_options_description()
    {
        std::shared_ptr<boost::program_options::options_description> db_opts(
                new boost::program_options::options_description(
                        std::string("Database connection configuration")));
        db_opts->add_options()
                ("database.name", boost::program_options
                            ::value<std::string>()->default_value(std::string("fred"))
                        , "database name")
                ("database.user", boost::program_options
                            ::value<std::string>()->default_value(std::string("fred"))
                        , "database user name")
                ("database.password", boost::program_options
                            ::value<std::string>(), "database password")
                ("database.host", boost::program_options
                            ::value<std::string>()->default_value(std::string("localhost"))
                        , "database hostname")
                ("database.port", boost::program_options
                            ::value<unsigned int>(), "database port number")
                ("database.timeout", boost::program_options
                            ::value<unsigned int>(), "database timeout");

        return db_opts;
    }//get_options_description

    void handle( int argc, char* argv[],  FakedArgs &fa)
    {
        boost::program_options::variables_map vm;
        handler_parse_args()(get_options_description(), vm, argc, argv, fa);

        /* construct connection string */
        conn_info += "host=";
        conn_info += vm["database.host"].as<std::string>();
        conn_info += " ";

        if (vm.count("database.port") == 1)
        {
            conn_info += "port=";
            conn_info += boost::lexical_cast<std::string>(vm["database.port"].as<unsigned>());
            conn_info += " ";
        }

        conn_info += "dbname=";
        conn_info += vm["database.name"].as<std::string>();
        conn_info += " ";

        conn_info += "user=";
        conn_info += vm["database.user"].as<std::string>();
        conn_info += " ";

        if (vm.count("database.password") == 1)
        {
            conn_info += "password=";
            conn_info += vm["database.password"].as<std::string>();
            conn_info += " ";
        }

        if (vm.count("database.timeout") == 1)
        {
            conn_info += "connect_timeout=";
            conn_info += boost::lexical_cast<std::string>(vm["database.timeout"].as<unsigned>());
            conn_info += " ";
        }

        Database::emplace_default_manager<Database::StandaloneManager>(conn_info);
    }//handle

    std::string get_conn_info() {
        if (conn_info.empty()) {
            throw std::runtime_error("Wrong usage: Connection info not initialized yet");
        }
        return conn_info;
    }
};//class HandleDatabaseArgs

/**
 * \class HandleLoggingArgsGrp
 * \brief database options handler with option groups
 */

class HandleDatabaseArgsGrp : public HandleGrpArgs
                            , private HandleDatabaseArgs
{
public:
    std::shared_ptr<boost::program_options::options_description>
        get_options_description()
    {
        return HandleDatabaseArgs::get_options_description();
    }//get_options_description
    std::size_t handle( int argc, char* argv[],  FakedArgs &fa
            , std::size_t option_group_index)
    {
        HandleDatabaseArgs::handle(argc, argv, fa);
        return option_group_index;
    }//handle

    const std::string& get_conn_info(){return HandleDatabaseArgs::conn_info;}
};//class HandleDatabaseArgsGrp


#endif
