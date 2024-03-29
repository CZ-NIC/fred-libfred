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
#ifndef GET_OBJECT_STATES_BY_HISTORY_ID_HH_BD592E4538FA457F808161FCB173E937
#define GET_OBJECT_STATES_BY_HISTORY_ID_HH_BD592E4538FA457F808161FCB173E937

#include "libfred/object_state/get_object_states.hh"
#include "libfred/opcontext.hh"

namespace LibFred {

class GetObjectStatesByHistoryId
{
public:
    GetObjectStatesByHistoryId(unsigned long long _history_id);
    typedef std::vector<ObjectStateData> ObjectState;
    struct Result
    {
        ObjectState object_state_at_begin;
        ObjectState object_state_at_end;
    };
    Result exec(const OperationContext& _ctx);
private:
    const unsigned long long history_id_;
};

}//namespace LibFred

#endif//GET_OBJECT_STATES_BY_HISTORY_ID_HH_BD592E4538FA457F808161FCB173E937
