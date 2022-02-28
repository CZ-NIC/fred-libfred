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
/**
 *  @merge_contact_selection.h
 *  selection of contact for merging
 */

#ifndef MERGE_CONTACT_SELECTION_HH_D8C066D7D30C4937904530A4B6A69E22
#define MERGE_CONTACT_SELECTION_HH_D8C066D7D30C4937904530A4B6A69E22

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"

#include "util/log/log.hh"
#include "util/factory.hh"
#include "util/factory_check.hh"
#include "util/printable.hh"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace LibFred {

static const std::string MCS_FILTER_IDENTIFIED_CONTACT = "mcs_filter_identified_contact";
static const std::string MCS_FILTER_IDENTITY_ATTACHED = "mcs_filter_identity_attached";
static const std::string MCS_FILTER_CONDITIONALLY_IDENTIFIED_CONTACT = "mcs_filter_conditionally_identified_contact";
static const std::string MCS_FILTER_HANDLE_MOJEID_SYNTAX = "mcs_filter_handle_mojeid_syntax";
static const std::string MCS_FILTER_MAX_DOMAINS_BOUND = "mcs_filter_max_domains_bound";
static const std::string MCS_FILTER_MAX_OBJECTS_BOUND = "mcs_filter_max_objects_bound";
static const std::string MCS_FILTER_RECENTLY_UPDATED = "mcs_filter_recently_updated";
static const std::string MCS_FILTER_NOT_REGCZNIC  = "mcs_filter_not_regcznic";
static const std::string MCS_FILTER_RECENTLY_CREATED = "mcs_filter_recently_created";

struct ContactSelectionFilter
{
    virtual ~ContactSelectionFilter() {}
    virtual std::vector<std::string> operator()(const OperationContext& ctx, const std::vector<std::string>& contact_handle) = 0;
};

struct MergeContactSelectionOutput : Util::Printable<MergeContactSelectionOutput>
{
    explicit MergeContactSelectionOutput(std::string _handle, std::string _filter);
    std::string to_string() const;

    std::string handle;
    std::string filter;
};

class MergeContactSelection : public Util::Printable<MergeContactSelection>
{
public:
    explicit MergeContactSelection(
            std::vector<std::string> contact_handles,
            std::vector<std::string> filters);
    MergeContactSelectionOutput exec(const OperationContext& ctx);
    std::string to_string()const;
    struct NoContactHandles : LibFred::OperationException
    {
        const char* what() const noexcept override;
    };
    struct NoContactHandlesLeft : LibFred::OperationException
    {
        const char* what() const noexcept override;
    };
    struct TooManyContactHandlesLeft : LibFred::OperationException
    {
        const char* what() const noexcept override;
    };
private:
    std::vector<std::string> contact_handles_;
    std::vector<std::string> filters_;
};

using ContactSelectionFilterFactory = Util::Factory<ContactSelectionFilter, std::string>;

const ContactSelectionFilterFactory& get_default_contact_selection_filter_factory();

}//namespace LibFred

#endif//MERGE_CONTACT_SELECTION_HH_D8C066D7D30C4937904530A4B6A69E22
