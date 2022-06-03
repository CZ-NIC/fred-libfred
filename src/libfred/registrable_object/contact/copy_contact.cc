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

#include "libfred/registrable_object/contact/copy_contact.hh"
#include "libfred/object_state/get_blocking_status_desc_list.hh"
#include "libfred/object_state/get_object_state_id_map.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/contact/create_contact.hh"
#include "libfred/object/object.hh"
#include "libfred/opcontext.hh"

#include "util/db/nullable.hh"
#include "util/log/log.hh"
#include "util/optional_value.hh"

namespace LibFred {

CopyContact::CopyContact(
        const std::string& _src_contact_handle,
        const std::string& _dst_contact_handle,
        RequestId _request_id)
    : src_contact_handle_(_src_contact_handle),
      dst_contact_handle_(_dst_contact_handle),
      request_id_(_request_id)
{}

CopyContact::CopyContact(
        const std::string& _src_contact_handle,
        const std::string& _dst_contact_handle,
        const Optional<std::string>& _dst_registrar_handle,
        RequestId _request_id)
    : src_contact_handle_(_src_contact_handle),
      dst_contact_handle_(_dst_contact_handle),
      dst_registrar_handle_(_dst_registrar_handle),
      request_id_(_request_id)
{}

namespace {

template <class T>
Optional<T> to_optional(const Nullable<T> &_n)
{
    return _n.isnull() ? Optional<T>() : Optional<T>(_n.get_value());
}

Optional<unsigned long long> to_optional(unsigned long long _n)
{
    return _n <= 0 ? Optional<unsigned long long>() : Optional<unsigned long long>(_n);
}

Optional<ContactAddressList> to_optional(ContactAddressList _l)
{
    return _l.empty() ? Optional< ContactAddressList >() : Optional< ContactAddressList >(_l);
}

}//namespace LibFred::{anonymous}

ObjectId CopyContact::exec(OperationContext &_ctx)
{
    const Database::Result check_args_res = _ctx.get_conn().exec_params(
        "SELECT "
               "(SELECT 1 " // src_contact_handle exist ? 1 : NULL
                "FROM object_registry "
                "WHERE type=$1::integer AND name=UPPER($2::text) AND erdate IS NULL),"
               "(SELECT 1 " // dst_contact_handle exist ? 1 : NULL
                "FROM object_registry "
                "WHERE type=$1::integer AND name=UPPER($3::text) AND erdate IS NULL),"
               "(SELECT 1 " // dst_registrar_handle exist ? 1 : NULL
                "FROM registrar "
                "WHERE handle=UPPER($4::text))",
        Database::query_param_list
            (OBJECT_TYPE_ID_CONTACT)(src_contact_handle_)(dst_contact_handle_)(dst_registrar_handle_.get_value_or_default()));
    if (check_args_res.size() == 1)
    {
        Exception ex;
        if (check_args_res[0][0].isnull())
        {
            ex.set_src_contact_handle_not_found(src_contact_handle_);
        }
        if (!check_args_res[0][1].isnull())
        {
            ex.set_dst_contact_handle_already_exist(dst_contact_handle_);
        }
        if (check_args_res[0][2].isnull())
        {
            ex.set_create_contact_failed(std::string("dst_registrar_handle ") + dst_registrar_handle_.print_quoted() + " doesn't exist");
        }
        if (ex.throw_me())
        {
            BOOST_THROW_EXCEPTION(ex);
        }
    }

    LibFred::InfoContactByHandle info_contact(src_contact_handle_);
    LibFred::InfoContactOutput old_contact = info_contact.exec(_ctx);
    LibFred::CreateContact create_contact(
            dst_contact_handle_,
            dst_registrar_handle_.get_value(),
            Optional<std::string>(),
            to_optional(old_contact.info_contact_data.name),
            to_optional(old_contact.info_contact_data.organization),
            to_optional(old_contact.info_contact_data.place),
            to_optional(old_contact.info_contact_data.telephone),
            to_optional(old_contact.info_contact_data.fax),
            to_optional(old_contact.info_contact_data.email),
            to_optional(old_contact.info_contact_data.notifyemail),
            to_optional(old_contact.info_contact_data.vat),
            to_optional(old_contact.info_contact_data.ssntype),
            to_optional(old_contact.info_contact_data.ssn),
            to_optional(old_contact.info_contact_data.addresses),
            to_optional(old_contact.info_contact_data.disclosename),
            to_optional(old_contact.info_contact_data.discloseorganization),
            to_optional(old_contact.info_contact_data.discloseaddress),
            to_optional(old_contact.info_contact_data.disclosetelephone),
            to_optional(old_contact.info_contact_data.disclosefax),
            to_optional(old_contact.info_contact_data.discloseemail),
            to_optional(old_contact.info_contact_data.disclosevat),
            to_optional(old_contact.info_contact_data.discloseident),
            to_optional(old_contact.info_contact_data.disclosenotifyemail),
            to_optional(old_contact.info_contact_data.warning_letter),
            to_optional(request_id_));
    try
    {
        create_contact.exec(_ctx);
    }
    catch (const LibFred::CreateContact::Exception& e)
    {
        /* XXX Exception method naming is incorrect. In fact it signalizes that contact with such handle already exists. */
        if (e.is_set_invalid_contact_handle())
        {
            BOOST_THROW_EXCEPTION(Exception().set_dst_contact_handle_already_exist(dst_contact_handle_));
        }
        throw;
    }
    const Database::Result dbres = _ctx.get_conn().exec_params(
            "WITH dst_c AS "
            "("
                "SELECT id "
                  "FROM object_registry "
                 "WHERE type = $1::integer AND "
                       "name = UPPER($3::text) AND "
                       "erdate IS NULL"
            "), "
            "authinfo AS "
            "("
                "INSERT INTO object_authinfo (object_id, registrar_id, password, created_at, expires_at) "
                    "SELECT dst_c.id, oa.registrar_id, oa.password, oa.created_at, oa.expires_at "
                      "FROM dst_c, "
                           "object_registry src_c "
                      "JOIN object_authinfo oa ON oa.object_id = src_c.id "
                     "WHERE src_c.type = $1::integer AND "
                           "src_c.name = UPPER($2::text) AND "
                           "src_c.erdate IS NULL AND "
                           "NOW() < oa.expires_at AND "
                           "oa.canceled_at IS NULL AND "
                           "oa.password <> '' "
                "RETURNING id"
            ") "
            "SELECT id, (SELECT COUNT(*) FROM authinfo) "
              "FROM dst_c",
            Database::QueryParams{ OBJECT_TYPE_ID_CONTACT, src_contact_handle_, dst_contact_handle_ });
    if (dbres.size() != 1)
    {
        BOOST_THROW_EXCEPTION(Exception().set_create_contact_failed("dst_contact " + dst_contact_handle_ + " not found"));
    }
    LOGGER.debug(static_cast<std::string>(dbres[0][1]) + " authinfo records copied");
    return static_cast<ObjectId>(dbres[0][0]);
}

}//namespace LibFred
