/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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

#include <boost/optional.hpp>
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
public:
    std::shared_ptr<boost::program_options::options_description>
    get_options_description()
    {
        auto db_opts = std::make_shared<boost::program_options::options_description>(
                "Database connection configuration");
        db_opts->add_options()
                ("database.name",
                        boost::program_options::value<std::string>()->default_value("fred"),
                        "database name")
                ("database.user",
                        boost::program_options::value<std::string>()->default_value("fred"),
                        "database user name")
                ("database.password",
                        boost::program_options::value<std::string>(),
                        "database password")
                ("database.host",
                        boost::program_options::value<std::string>()->default_value("localhost"),
                        "database hostname")
                ("database.port",
                        boost::program_options::value<unsigned int>(),
                        "database port number")
                ("database.timeout",
                        boost::program_options::value<unsigned int>(),
                        "database timeout");
        return db_opts;
    }

    void handle(int argc, char* argv[], FakedArgs& fa)
    {
        boost::program_options::variables_map vm;
        handler_parse_args()(get_options_description(), vm, argc, argv, fa);

        /* construct connection string */
        host_ = vm["database.host"].as<std::string>();
        conn_info_ += "host=" + host_ + " ";

        if (vm.count("database.port") == 1)
        {
            port_ = vm["database.port"].as<unsigned>();
            conn_info_ += "port=" + boost::lexical_cast<std::string>(*port_) + " ";
        }

        db_name_ = vm["database.name"].as<std::string>();
        conn_info_ += "dbname=" + db_name_ + " ";

        user_ = vm["database.user"].as<std::string>();
        conn_info_ += "user=" + user_ + " ";

        if (vm.count("database.password") == 1)
        {
            password_ = vm["database.password"].as<std::string>();
            conn_info_ += "password=" + *password_ + " ";
        }

        if (vm.count("database.timeout") == 1)
        {
            connect_timeout_sec_ = vm["database.timeout"].as<unsigned>();
            conn_info_ += "connect_timeout=" + boost::lexical_cast<std::string>(*connect_timeout_sec_) + " ";
        }

        Database::emplace_default_manager<Database::StandaloneManager>(conn_info_);
    }

    const std::string& get_conn_info() const
    {
        if (conn_info_.empty())
        {
            throw std::runtime_error("Wrong usage: Connection info not initialized yet");
        }
        return conn_info_;
    }
 
    const std::string& get_host() const
    {
        return host_;
    }
 
    bool has_port() const noexcept
    {
        return port_ != boost::none;
    }
    unsigned get_port() const
    {
        return *port_;
    }
 
    const std::string& get_db_name() const
    {
        return db_name_;
    }
 
    const std::string& get_user() const
    {
        return user_;
    }
 
    bool has_password() const noexcept
    {
        return password_ != boost::none;
    }
    const std::string& get_password() const
    {
        return *password_;
    }
 
    bool has_connect_timeout() const noexcept
    {
        return connect_timeout_sec_ != boost::none;
    }
    unsigned get_connect_timeout_sec() const
    {
        return *connect_timeout_sec_;
    }
private:
    std::string conn_info_;
    std::string host_;
    boost::optional<unsigned> port_;
    std::string db_name_;
    std::string user_;
    boost::optional<std::string> password_;
    boost::optional<unsigned> connect_timeout_sec_;
};

/**
 * \class HandleLoggingArgsGrp
 * \brief database options handler with option groups
 */
class HandleDatabaseArgsGrp : public HandleGrpArgs,
                              private HandleDatabaseArgs
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

    const std::string& get_conn_info() const { return this->HandleDatabaseArgs::get_conn_info(); }
};

#endif
