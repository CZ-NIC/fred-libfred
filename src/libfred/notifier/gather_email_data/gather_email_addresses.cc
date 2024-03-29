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
#include "libfred/notifier/gather_email_data/gather_email_addresses.hh"
#include "libfred/notifier/util/get_previous_object_historyid.hh"

#include "libfred/notifier/gather_email_data/objecttype_specific_impl/contact.hh"
#include "libfred/notifier/gather_email_data/objecttype_specific_impl/domain.hh"
#include "libfred/notifier/gather_email_data/objecttype_specific_impl/keyset.hh"
#include "libfred/notifier/gather_email_data/objecttype_specific_impl/nsset.hh"
#include "libfred/notifier/gather_email_data/objecttype_specific_impl/util.hh"

#include "libfred/notifier/exception.hh"

#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/domain/info_domain.hh"
#include "libfred/registrable_object/keyset/info_keyset.hh"
#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/domain/info_domain_diff.hh"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

namespace Notification {


/**
 * @return unique non-empty notifyemail addresses of given _contact_ids
 */
static std::set<std::string> get_email_addresses(
    const LibFred::OperationContext& _ctx,
    const std::set<unsigned long long>& _contact_ids,
    const boost::posix_time::ptime& _time_of_validity
) {

    std::set<std::string> contact_ids_as_strings;
    BOOST_FOREACH(unsigned long long id, _contact_ids) {
        contact_ids_as_strings.insert( boost::lexical_cast<std::string>(id) );
    }

    const Database::Result email_addresses = _ctx.get_conn().exec_params(
        "SELECT "
            "notifyemail "
            "FROM contact_history c_h "
                "JOIN history h ON c_h.historyid = h.id "
            "WHERE c_h.id = ANY($1::INT[]) "
                /* this is closed interval inclusive test - inclusive "newer end" is mandatory for deleted contact, "older end" is included just in case */
                "AND ( h.valid_from <= $2::TIMESTAMP AND $2::TIMESTAMP < COALESCE( h.valid_to, 'infinity'::TIMESTAMP )) "
                "AND c_h.notifyemail IS NOT NULL ",
        Database::query_param_list
            ( std::string("{") + boost::algorithm::join(contact_ids_as_strings, ", ") + "}" )
            ( _time_of_validity )
    );

    std::set<std::string> result;

    for (unsigned int i = 0; i < email_addresses.size(); ++i) {
        const std::string email_addr = boost::algorithm::trim_copy( static_cast<std::string>( email_addresses[i]["notifyemail"] ) );
        if (!email_addr.empty() ) {
            result.insert(email_addr);
        }
    }

    return result;
}

std::set<std::string> gather_email_addresses(
    const LibFred::OperationContext& _ctx,
    const EventOnObject& _event_on_object,
    unsigned long long _last_history_id /* XXX always post change but delete ... */
) {
    if (_event_on_object.get_type() == LibFred::contact ) {
        return get_emails_to_notify_contact_event(_ctx, _event_on_object.get_event(), _last_history_id);
    }

    std::set<unsigned long long> contact_ids_to_notify;

    if (_event_on_object.get_type() == LibFred::domain ) {
        contact_ids_to_notify = gather_contact_ids_to_notify_domain_event(_ctx, _event_on_object.get_event(), _last_history_id);

    } else if( _event_on_object.get_type() == LibFred::keyset ) {
        contact_ids_to_notify = gather_contact_ids_to_notify_keyset_event(_ctx, _event_on_object.get_event(), _last_history_id);

    } else if( _event_on_object.get_type() == LibFred::nsset ) {
        contact_ids_to_notify = gather_contact_ids_to_notify_nsset_event(_ctx, _event_on_object.get_event(), _last_history_id);
    } else {
        throw ExceptionObjectTypeNotImplemented();
    }

    return get_email_addresses(
        _ctx,
        contact_ids_to_notify,
        get_utc_time_of_event(_ctx, _event_on_object.get_event(), _last_history_id)
    );
}

}
