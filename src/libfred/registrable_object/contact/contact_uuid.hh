#ifndef CONTACT_UUID_HH_866B23F359411A87BF70804743F13EDC//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CONTACT_UUID_HH_866B23F359411A87BF70804743F13EDC

#include "libfred/registrable_object/uuid.hh"
#include "util/db/value.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

using ContactUuid = UuidOf<Object_Type::contact>;
using ContactHistoryUuid = HistoryUuidOf<Object_Type::contact>;

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

namespace Database {

template <>
LibFred::RegistrableObject::Contact::ContactUuid Value::as()const;

template <>
LibFred::RegistrableObject::Contact::ContactHistoryUuid Value::as()const;

}//namespace Database

#endif//CONTACT_UUID_HH_866B23F359411A87BF70804743F13EDC
