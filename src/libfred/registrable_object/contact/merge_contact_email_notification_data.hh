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
 *  @file merge_contact_email_notification_data.h
 *  contact merge email notification data
 */

#ifndef MERGE_CONTACT_EMAIL_NOTIFICATION_DATA_HH_4CD8CF4F78674DAAA4FEE4D2A05BF30D
#define MERGE_CONTACT_EMAIL_NOTIFICATION_DATA_HH_4CD8CF4F78674DAAA4FEE4D2A05BF30D

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "util/printable.hh"
#include "libfred/registrable_object/contact/merge_contact.hh"

#include <string>
#include <vector>

namespace LibFred {

struct MergeContactEmailNotificationInput : Util::Printable<MergeContactEmailNotificationInput>
{
    MergeContactEmailNotificationInput(){}
    MergeContactEmailNotificationInput(const std::string& _src_contact_handle
            , const std::string& _dst_contact_handle
            , const MergeContactOutput& _merge_output)
    : src_contact_handle(_src_contact_handle)
    , dst_contact_handle(_dst_contact_handle)
    , merge_output(_merge_output)
    {}

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const
    {
        return Util::format_data_structure(
                "MergeContactEmailNotificationInput",
                Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("src_contact_handle", src_contact_handle))
                    (std::make_pair("dst_contact_handle", dst_contact_handle))
                    (std::make_pair("merge_output", merge_output.to_string())));
    }

    std::string src_contact_handle;//source contact identifier
    std::string dst_contact_handle;//destination contact identifier
    MergeContactOutput merge_output;//result of merge operation
};

struct MergeContactNotificationEmail : Util::Printable<MergeContactNotificationEmail>
{
    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const
    {
        return Util::format_data_structure(
                "MergeContactNotificationEmail",
                Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("dst_contact_handle", dst_contact_handle))
                    (std::make_pair("dst_contact_roid", dst_contact_roid))
                    (std::make_pair("domain_registrant_list", Util::format_container(domain_registrant_list)))
                    (std::make_pair("domain_admin_list", Util::format_container(domain_admin_list)))
                    (std::make_pair("nsset_tech_list", Util::format_container(nsset_tech_list)))
                    (std::make_pair("keyset_tech_list", Util::format_container(keyset_tech_list)))
                    (std::make_pair("removed_list", Util::format_container(removed_list)))
                    (std::make_pair("removed_roid_list", Util::format_container(removed_roid_list))));
    }
    std::string dst_contact_handle;
    std::string dst_contact_roid;
    std::vector<std::string> domain_registrant_list;
    std::vector<std::string> domain_admin_list;
    std::vector<std::string> nsset_tech_list;
    std::vector<std::string> keyset_tech_list;
    std::vector<std::string> removed_list;
    std::vector<std::string> removed_roid_list;
};

struct SortedContactNotificationEmail
{
    std::string dst_contact_handle;
    std::set<std::string> domain_registrant_list;
    std::set<std::string> domain_admin_list;
    std::set<std::string> nsset_tech_list;
    std::set<std::string> keyset_tech_list;
    std::set<std::string> removed_list;
    std::set<std::string> removed_roid_list;
};

class MergeContactEmailNotificationData : public Util::Printable<MergeContactEmailNotificationData>
{
public:
    DECLARE_EXCEPTION_DATA(invalid_contact_handle, std::string);
    DECLARE_EXCEPTION_DATA(invalid_registry_object_identifier, std::string);
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_invalid_contact_handle<Exception>
    , ExceptionData_invalid_registry_object_identifier<Exception>
    {};

    MergeContactEmailNotificationData(const std::vector<MergeContactEmailNotificationInput>& merge_contact_data_);
    std::vector<MergeContactNotificationEmail> exec(OperationContext& ctx);

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    void update_email(
            std::vector<MergeContactEmailNotificationInput>::iterator i,
            SortedContactNotificationEmail& email);
    std::vector<MergeContactEmailNotificationInput> merge_contact_data_;
};

struct MergeContactNotificationEmailWithAddr
{
    std::string notification_email_addr;
    MergeContactNotificationEmail email_data;
};

class MergeContactNotificationEmailAddr : public Util::Printable<MergeContactNotificationEmailAddr>
{
public:
    DECLARE_EXCEPTION_DATA(invalid_registry_object_identifier, std::string);
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_invalid_registry_object_identifier<Exception>
    {};

    MergeContactNotificationEmailAddr(const std::vector<MergeContactNotificationEmail>& email_data);
    std::vector<MergeContactNotificationEmailWithAddr> exec(OperationContext& ctx);

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::vector<MergeContactNotificationEmail> email_data_;
};

}//namespace LibFred

#endif//MERGE_CONTACT_EMAIL_NOTIFICATION_DATA_HH_4CD8CF4F78674DAAA4FEE4D2A05BF30D
