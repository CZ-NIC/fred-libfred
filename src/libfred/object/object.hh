/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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

#ifndef OBJECT_HH_656D57B205F348B8A9C6540F6FAA1625
#define OBJECT_HH_656D57B205F348B8A9C6540F6FAA1625

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <chrono>
#include <string>

namespace LibFred {

/**
 * Creates common part of registry object.
 */
class CreateObject : public Util::Printable<CreateObject>
{
public:
    DECLARE_EXCEPTION_DATA(invalid_object_handle, std::string);
    DECLARE_EXCEPTION_DATA(unknown_registrar_handle, std::string);
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_invalid_object_handle<Exception>
    , ExceptionData_unknown_registrar_handle<Exception>
    {};

    CreateObject(const std::string& object_type
            , const std::string& handle
            , const std::string& registrar);
    CreateObject(const std::string& object_type
            , const std::string& handle
        , const std::string& registrar
        , const Nullable<unsigned long long>& logd_request_id);
    CreateObject& set_logd_request_id(const Nullable<unsigned long long>& logd_request_id);

    struct Result
    {
        enum { INVALID_ID = 0 };
        Result()
        :   object_id(INVALID_ID),
            history_id(INVALID_ID)
        {}
        unsigned long long object_id;
        unsigned long long history_id;
    };
    Result exec(OperationContext& ctx);

    std::string to_string()const;
private:
    const std::string object_type_;//object type name
    const std::string handle_;//object identifier
    const std::string registrar_;//set registrar
    Nullable<unsigned long long> logd_request_id_;//logger request_id
};

/**
 * Updates some common parts of registry object.
 */
class UpdateObject : public Util::Printable<UpdateObject>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_handle, std::string);
    DECLARE_EXCEPTION_DATA(unknown_registrar_handle, std::string);
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_handle<Exception>
    , ExceptionData_unknown_registrar_handle<Exception>
    {};

    UpdateObject(const std::string& handle
            , const std::string& obj_type
            , const std::string& registrar);
    UpdateObject(const std::string& handle
        , const std::string& obj_type
        , const std::string& registrar
        , const Optional<std::string>& authinfo
        , const Nullable<unsigned long long>& logd_request_id
    );
    UpdateObject& set_authinfo(const std::string& authinfo);
    UpdateObject& set_logd_request_id(const Nullable<unsigned long long>& logd_request_id);

    unsigned long long exec(OperationContext& ctx);//return history_id

    std::string to_string()const;

    static std::chrono::seconds set_authinfo_ttl(std::chrono::seconds value) noexcept;
private:
    const std::string handle_;//object identifier
    const std::string obj_type_;//object type name
    const std::string registrar_;//set registrar performing the update
    Optional<std::string> authinfo_;//set authinfo
    Nullable<unsigned long long> logd_request_id_;//logger request_id
};

class InsertHistory : public Util::Printable<InsertHistory>
{
public:
    InsertHistory(const Nullable<unsigned long long>& logd_request_id, unsigned long long object_id);
    unsigned long long exec(OperationContext& ctx);

    std::string to_string()const;
private:
    Nullable<unsigned long long> logd_request_id_; //id of the new entry in log_entry database table, id is used in other calls to logging within current request
    unsigned long long object_id_;
};

class DeleteObjectByHandle : public Util::Printable<DeleteObjectByHandle>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_handle, std::string);
    struct Exception
        : virtual LibFred::OperationException
        , ExceptionData_unknown_object_handle<Exception>
        {};

    DeleteObjectByHandle(const std::string& handle
            , const std::string& obj_type);

    void exec(OperationContext& ctx);

    std::string to_string()const;
private:
    const std::string handle_;      //object handle
    const std::string obj_type_;    //object type name
};

class DeleteObjectById : public Util::Printable<DeleteObjectById>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_id, unsigned long long);
    struct Exception
        : virtual LibFred::OperationException
        , ExceptionData_unknown_object_id<Exception>
        {};

    DeleteObjectById(unsigned long long id);

    void exec(OperationContext& ctx);

    std::string to_string()const;
private:
    const unsigned long long id_;      //object id
};

}//namespace LibFred

#endif//OBJECT_HH_656D57B205F348B8A9C6540F6FAA1625
