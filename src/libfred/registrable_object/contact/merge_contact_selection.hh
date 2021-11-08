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

FACTORY_MODULE_INIT_DECL(merge_contact_selection)

typedef std::string ContactSelectionFilterType;

static const ContactSelectionFilterType MCS_FILTER_IDENTIFIED_CONTACT = "mcs_filter_identified_contact";
static const ContactSelectionFilterType MCS_FILTER_IDENTITY_ATTACHED = "mcs_filter_identity_attached";
static const ContactSelectionFilterType MCS_FILTER_CONDITIONALLY_IDENTIFIED_CONTACT = "mcs_filter_conditionally_identified_contact";
static const ContactSelectionFilterType MCS_FILTER_HANDLE_MOJEID_SYNTAX = "mcs_filter_handle_mojeid_syntax";
static const ContactSelectionFilterType MCS_FILTER_MAX_DOMAINS_BOUND = "mcs_filter_max_domains_bound";
static const ContactSelectionFilterType MCS_FILTER_MAX_OBJECTS_BOUND = "mcs_filter_max_objects_bound";
static const ContactSelectionFilterType MCS_FILTER_RECENTLY_UPDATED = "mcs_filter_recently_updated";
static const ContactSelectionFilterType MCS_FILTER_NOT_REGCZNIC  = "mcs_filter_not_regcznic";
static const ContactSelectionFilterType MCS_FILTER_RECENTLY_CREATED = "mcs_filter_recently_created";

struct ContactSelectionFilterBase
{
  virtual ~ContactSelectionFilterBase() {}
  virtual std::vector<std::string> operator()(const OperationContext& ctx, const std::vector<std::string>& contact_handle) = 0;
};

struct MergeContactSelectionOutput : Util::Printable<MergeContactSelectionOutput>
{
    MergeContactSelectionOutput(
            const std::string &_handle,
            const ContactSelectionFilterType &_filter)
        : handle(_handle),
          filter(_filter)
    {}

    std::string to_string() const
    {
        return Util::format_data_structure(
                "MergeContactSelectionOutput",
                Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("handle", handle))
                    (std::make_pair("filter", filter)));
    }

    std::string handle;
    ContactSelectionFilterType filter;
};

class MergeContactSelection : public Util::Printable<MergeContactSelection>
{
public:
    struct NoContactHandles : LibFred::OperationException
    {
        const char* what() const noexcept { return "no contact handles, nothing to process"; }
    };
    struct NoContactHandlesLeft : LibFred::OperationException
    {
        const char* what() const noexcept { return "no contact handles left, selection of contact with given rules failed"; }
    };
    struct TooManyContactHandlesLeft : LibFred::OperationException
    {
        const char* what() const noexcept { return "too many contact handles left, selection of contact with given rules failed"; }
    };

    MergeContactSelection(const std::vector<std::string>& contact_handle,
                          const std::vector<ContactSelectionFilterType>& filter);
    MergeContactSelectionOutput exec(const OperationContext& ctx);

    std::string to_string()const;
private:
    std::vector<std::string> contact_handle_;//contact handle vector
    std::vector<std::pair<std::string, std::shared_ptr<ContactSelectionFilterBase>>> ff_;//filter functor ptr vector
};

typedef Util::Factory<ContactSelectionFilterBase, Util::ClassCreator<ContactSelectionFilterBase> > ContactSelectionFilterFactory;

}//namespace LibFred

#endif//MERGE_CONTACT_SELECTION_HH_D8C066D7D30C4937904530A4B6A69E22
