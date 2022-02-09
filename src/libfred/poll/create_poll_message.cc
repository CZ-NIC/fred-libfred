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

#include "libfred/poll/create_poll_message.hh"
#include "libfred/object/object_type.hh"

namespace LibFred {
namespace Poll {

namespace {

template <MessageType::Enum message_type>
unsigned long long create_poll_eppaction_message(
        const LibFred::OperationContext& ctx,
        unsigned long long action_history_id,
        unsigned long long recipient_registrar_id)
{
    const Database::Result db_res = ctx.get_conn().exec_params(
            "WITH create_new_message AS ("
                "INSERT INTO message (clid,crdate,exdate,seen,msgtype) "
                "SELECT $2::BIGINT,NOW(),NOW()+'7DAY'::INTERVAL,false,id "
                "FROM messagetype "
                "WHERE name=$3::TEXT "
                "RETURNING id AS msgid) "
            "INSERT INTO poll_eppaction (msgid,objid) "
            "SELECT msgid,$1::BIGINT FROM create_new_message "
            "RETURNING msgid",
            Database::query_param_list(action_history_id)
                                      (recipient_registrar_id)
                                      (Conversion::Enums::to_db_handle(message_type)));
    if (db_res.size() == 1)
    {
        return static_cast<unsigned long long>(db_res[0][0]);
    }
    struct UnexpectedNumberOfRows:InternalError
    {
        UnexpectedNumberOfRows():InternalError("unexpected number of rows") { }
    };
    throw UnexpectedNumberOfRows();
}

struct SponsoringRegistrar
{
    enum Enum
    {
        at_action_end,
        at_action_start,
    };
};

typedef unsigned long long (*CreatePollMessageFunction)(
        const LibFred::OperationContext& ,
        unsigned long long,
        unsigned long long);

template <MessageType::Enum message_type>
struct MessageTypeTraits { };

template <>
struct MessageTypeTraits<MessageType::transfer_contact>
{
    static const MessageType::Enum message_type = MessageType::transfer_contact;
    static const Object_Type::Enum object_type = Object_Type::contact;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_start;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::transfer_contact>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::transfer_domain>
{
    static const MessageType::Enum message_type = MessageType::transfer_domain;
    static const Object_Type::Enum object_type = Object_Type::domain;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_start;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::transfer_domain>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::transfer_nsset>
{
    static const MessageType::Enum message_type = MessageType::transfer_nsset;
    static const Object_Type::Enum object_type = Object_Type::nsset;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_start;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::transfer_nsset>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::transfer_keyset>
{
    static const MessageType::Enum message_type = MessageType::transfer_keyset;
    static const Object_Type::Enum object_type = Object_Type::keyset;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_start;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::transfer_keyset>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::update_contact>
{
    static const MessageType::Enum message_type = MessageType::update_contact;
    static const Object_Type::Enum object_type = Object_Type::contact;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_end;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::update_contact>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::update_domain>
{
    static const MessageType::Enum message_type = MessageType::update_domain;
    static const Object_Type::Enum object_type = Object_Type::domain;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_end;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::update_domain>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::update_nsset>
{
    static const MessageType::Enum message_type = MessageType::update_nsset;
    static const Object_Type::Enum object_type = Object_Type::nsset;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_end;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::update_nsset>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::update_keyset>
{
    static const MessageType::Enum message_type = MessageType::update_keyset;
    static const Object_Type::Enum object_type = Object_Type::keyset;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_end;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::update_keyset>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::delete_contact>
{
    static const MessageType::Enum message_type = MessageType::delete_contact;
    static const Object_Type::Enum object_type = Object_Type::contact;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_end;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::delete_contact>::create_poll_message =
        create_poll_eppaction_message<message_type>;

template <>
struct MessageTypeTraits<MessageType::delete_domain>
{
    static const MessageType::Enum message_type = MessageType::delete_domain;
    static const Object_Type::Enum object_type = Object_Type::domain;
    static const SponsoringRegistrar::Enum recipient = SponsoringRegistrar::at_action_end;
    static const CreatePollMessageFunction create_poll_message;
};

const CreatePollMessageFunction MessageTypeTraits<MessageType::delete_domain>::create_poll_message =
        create_poll_eppaction_message<message_type>;


template <SponsoringRegistrar::Enum recipient>
struct GetPrimaryRecipientImpl { };

template <>
struct GetPrimaryRecipientImpl<SponsoringRegistrar::at_action_end>
{
    static constexpr char sql[] =
        "SELECT eot.id IS NOT NULL,oh.clid "
        "FROM object_history oh "
        "JOIN object_registry obr ON obr.id=oh.id "
        "LEFT JOIN enum_object_type eot ON eot.id=obr.type AND eot.name=$2::TEXT "
        "WHERE oh.historyid=$1::BIGINT";

};

constexpr char GetPrimaryRecipientImpl<SponsoringRegistrar::at_action_end>::sql[];

template <>
struct GetPrimaryRecipientImpl<SponsoringRegistrar::at_action_start>
{
    static constexpr char sql[] =
        "SELECT eot.id IS NOT NULL,oh.clid "
        "FROM object_history oh "
        "JOIN object_registry obr ON obr.id=oh.id "
        "LEFT JOIN enum_object_type eot ON eot.id=obr.type AND eot.name=$2::TEXT "
        "WHERE oh.historyid=(SELECT id FROM history WHERE next=$1::BIGINT)";
};

constexpr char GetPrimaryRecipientImpl<SponsoringRegistrar::at_action_start>::sql[];


template<MessageType::Enum message_type>
struct GetPrimaryRecipient
{
    unsigned long long exec(const LibFred::OperationContext& _ctx, unsigned long long _history_id) const
    {
        using MessageTraits = MessageTypeTraits<message_type>;
        const std::string requested_object_type_handle = Conversion::Enums::to_db_handle(MessageTraits::object_type);
        const Database::Result result =
                _ctx.get_conn().exec_params(
                        GetPrimaryRecipientImpl<MessageTraits::recipient>::sql,
                        Database::query_param_list(_history_id)(requested_object_type_handle));

        switch (result.size())
        {
            case 0:
                struct NotFound : OperationException
                {
                    const char* what() const noexcept override
                    {
                        return "object history not found";
                    }
                };
                throw NotFound();
            case 1:
                break;
            default:
                struct TooManyRows : InternalError
                {
                    TooManyRows() : InternalError("too many rows") { }
                };
                throw TooManyRows();
        }

        const bool object_type_corresponds_to_message_type = static_cast<bool>(result[0][0]);
        if (!object_type_corresponds_to_message_type)
        {
            struct NotCorrespondingObjectType : OperationException
            {
                const char* what() const noexcept override
                {
                    return "associated object is not of the type corresponding to the given message type";
                }
            };
            throw NotCorrespondingObjectType();
        }

        return static_cast<unsigned long long>(result[0][1]);
    }
};

} // namespace LibFred::Poll::{anonymous}


template <MessageType::Enum message_type>
unsigned long long CreatePollMessage<message_type>::exec(
        const LibFred::OperationContext& _ctx,
        unsigned long long _history_id) const
{
    using MessageTraits = MessageTypeTraits<message_type>;

    const auto primary_recipient = GetPrimaryRecipient<message_type>().exec(_ctx, _history_id);
    return MessageTraits::create_poll_message(_ctx, _history_id, primary_recipient);
}

template struct CreatePollMessage<MessageType::transfer_contact>;
template struct CreatePollMessage<MessageType::transfer_domain>;
template struct CreatePollMessage<MessageType::transfer_nsset>;
template struct CreatePollMessage<MessageType::transfer_keyset>;

template struct CreatePollMessage<MessageType::update_contact>;
template struct CreatePollMessage<MessageType::update_domain>;
template struct CreatePollMessage<MessageType::update_nsset>;
template struct CreatePollMessage<MessageType::update_keyset>;

template struct CreatePollMessage<MessageType::delete_contact>;
template struct CreatePollMessage<MessageType::delete_domain>;


namespace {

template<MessageType::Enum message_type>
struct GetAdditionalRecipients;


template<>
struct GetAdditionalRecipients<MessageType::update_contact>
{
    auto exec(const LibFred::OperationContext& _ctx, unsigned long long _history_id) const
    {
        const Database::Result result = _ctx.get_conn().exec_params(
            "WITH contact_ AS ( "
            "SELECT h.valid_from AS update_timestamp, oh.id AS contact_id, "
                    "oh.clid AS contact_clid "
            "FROM history h "
            "JOIN object_history oh ON oh.historyid = h.id "
            "WHERE h.id = $1::BIGINT "
            ") "
            "SELECT DISTINCT d_oh.clid "
            "FROM contact_ c "
            "JOIN domain_history dh ON dh.registrant = c.contact_id "
            "JOIN object_history d_oh ON d_oh.historyid = dh.historyid "
            "JOIN history d_h ON d_h.id = dh.historyid AND "
                                "d_h.valid_from <= c.update_timestamp AND c.update_timestamp < coalesce(d_h.valid_to, 'infinity') "
            "WHERE d_oh.clid != c.contact_clid "
            "UNION "
            "SELECT DISTINCT d_oh.clid "
            "FROM contact_ c "
            "JOIN domain_contact_map_history dcmh ON dcmh.contactid = c.contact_id "
            "JOIN object_history d_oh ON d_oh.historyid = dcmh.historyid "
            "JOIN history d_h ON d_h.id = dcmh.historyid AND "
                                "d_h.valid_from <= c.update_timestamp AND c.update_timestamp < coalesce(d_h.valid_to, 'infinity') "
            "WHERE d_oh.clid != c.contact_clid",
            Database::query_param_list(_history_id)
        );
        std::set<unsigned long long> registrars;
        for (auto i = 0u; i < result.size(); ++i)
        {
            registrars.insert(static_cast<unsigned long long>(result[i][0]));
        }
        return registrars;
    }
};

template <Object_Type::Enum object_type>
struct UpdateOperationMessageTypeTraits { };

template<>
struct UpdateOperationMessageTypeTraits<Object_Type::contact>
{
    using Traits = MessageTypeTraits<MessageType::update_contact>;
};

template<>
struct UpdateOperationMessageTypeTraits<Object_Type::domain>
{
    using Traits = MessageTypeTraits<MessageType::update_domain>;
};

template<>
struct UpdateOperationMessageTypeTraits<Object_Type::nsset>
{
    using Traits = MessageTypeTraits<MessageType::update_nsset>;
};

template<>
struct UpdateOperationMessageTypeTraits<Object_Type::keyset>
{
    using Traits = MessageTypeTraits<MessageType::update_keyset>;
};


bool was_update_done_by_sponsoring_registrar(const LibFred::OperationContext& _ctx, unsigned long long _history_id)
{
    const Database::Result result = _ctx.get_conn().exec_params(
        "SELECT oh_prev.clid = oh_act.upid "
        "FROM object_history oh_act "
        "LEFT JOIN history h_prev ON h_prev.next = oh_act.historyid "
        "LEFT JOIN object_history oh_prev ON oh_prev.historyid = h_prev.id AND "
                                            "oh_prev.clid = oh_act.clid "
        "WHERE oh_act.historyid = $1::BIGINT",
        Database::query_param_list(_history_id));

    switch (result.size())
    {
        case 0:
            struct NotFound : OperationException
            {
                const char* what() const noexcept override
                {
                    return "object history not found";
                }
            };
            throw NotFound();
        case 1:
            break;
        default:
            struct TooManyRows : InternalError
            {
                TooManyRows() : InternalError("too many rows") { }
            };
            throw TooManyRows();
    }
    if (result[0][0].isnull())
    {
        struct NotUpdated : OperationException
        {
            const char* what() const noexcept override
            {
                return "object has not been updated";
            }
        };
        throw NotUpdated{};
    }
    return static_cast<bool>(result[0][0]);
}

} // namespace LibFred::Poll::{anonymous}


template <Object_Type::Enum object_type>
typename CreateUpdateOperationPollMessage<object_type>::Result CreateUpdateOperationPollMessage<object_type>::exec(
        const LibFred::OperationContext& _ctx,
        unsigned long long _history_id) const
{
    CreateUpdateOperationPollMessage<object_type>::Result poll_messages;
    using MessageTraits = typename UpdateOperationMessageTypeTraits<object_type>::Traits;

    auto recipients = GetAdditionalRecipients<MessageTraits::message_type>().exec(_ctx, _history_id);

    const auto should_notify_sponsoring_registrar = !was_update_done_by_sponsoring_registrar(_ctx, _history_id);
    if (should_notify_sponsoring_registrar)
    {
        recipients.insert(GetPrimaryRecipient<MessageTraits::message_type>().exec(_ctx, _history_id));
    }

    for (const auto& registrar_id : recipients)
    {
        poll_messages.insert(MessageTraits::create_poll_message(_ctx, _history_id, registrar_id));
    }
    return poll_messages;
}

template struct CreateUpdateOperationPollMessage<Object_Type::contact>;

} // namespace LibFred::Poll
} // namespace LibFred
