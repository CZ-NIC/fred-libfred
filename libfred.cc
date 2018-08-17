#include "src/libfred/registrable_object/contact/check_contact.hh"
#include "src/libfred/registrable_object/contact/copy_contact.hh"
#include "src/libfred/registrable_object/contact/create_contact.hh"
#include "src/libfred/registrable_object/contact/delete_contact.hh"
#include "src/libfred/registrable_object/contact/find_contact_duplicates.hh"
#include "src/libfred/registrable_object/contact/info_contact.hh"
#include "src/libfred/registrable_object/contact/transfer_contact.hh"
#include "src/libfred/registrable_object/contact/undisclose_address.hh"
#include "src/libfred/registrable_object/contact/update_contact.hh"

#include "src/util/log/logger.hh"

#include <exception>
#include <iostream>
#include <sstream>
#include <cstdlib>

int main(int argc, char* argv[])
{
    const char* const db_conn_str = argc < 2 ? "host=localhost port=11112 dbname=fred user=fred"
                                             : argv[1];
    try
    {
        {
            Logging::Manager::instance_ref().get(PACKAGE).setLevel(Logging::Log::Level::LL_DEBUG);
            boost::any param = std::string("libfred.log");
            Logging::Manager::instance_ref().get(PACKAGE).addHandler(Logging::Log::LT_FILE, param);
        }
        {
            Database::emplace_default_manager<Database::StandaloneManager>(db_conn_str);
        }
        const std::string registrar_handle = "REG-FRED_A";
        const std::string new_registrar_handle = "REG-FRED_B";
        const std::string existing_contact_handle = "KONTAKT";
        const std::string contact_handle = "KONTAKT-LIBFRED-TESTOVACI";
        const std::string copy_contact_handle = "KONTAKT-LIBFRED-TESTOVA-KOPIE";
        LibFred::OperationContextCreator ctx;

        LibFred::CreateContact create_contact_op(contact_handle, registrar_handle);
        create_contact_op.set_name("Líba Fredová");
        LibFred::Contact::PlaceAddress place;
        place.street1 = "Libá 1";
        place.postalcode = "12345";
        place.city = "Fredín";
        place.country = "CZ";
        create_contact_op.set_place(place);
        create_contact_op.set_telephone("420.600654321");
        create_contact_op.set_email("libuse@fredova.cz");
        create_contact_op.set_notifyemail("admin@libfreda.cz");
        create_contact_op.set_ssntype("BIRTHDAY");
        create_contact_op.set_ssn("1.1.2000");
        create_contact_op.set_disclosename(true);
        create_contact_op.set_discloseorganization(true);
        create_contact_op.set_discloseaddress(true);
        create_contact_op.set_discloseemail(false);
        create_contact_op.set_disclosetelephone(false);
        create_contact_op.set_disclosefax(false);
        create_contact_op.set_discloseident(false);
        create_contact_op.set_disclosevat(false);
        create_contact_op.set_disclosenotifyemail(false);
        const auto create_contact_result = create_contact_op.exec(ctx);
        ctx.get_log().info(boost::format("contact id = %1%") % create_contact_result.create_object_result.object_id);

        LibFred::InfoContactByHandle info_contact_op(contact_handle);
        const auto info = info_contact_op.exec(ctx);
        std::ostringstream out;
        out << info;
        std::cout << out.str() << std::endl;
        ctx.get_log().info(out.str());

        LibFred::TransferContact transfer_contact_op(info.info_contact_data.id,
                                                     new_registrar_handle,
                                                     info.info_contact_data.authinfopw);
        const auto transfer_contact_result = transfer_contact_op.exec(ctx);
        ctx.get_log().info(boost::format("contact history id = %1%") % transfer_contact_result);

        LibFred::UpdateContactByHandle update_contact_op(contact_handle, registrar_handle);
        place.street1 = "Libá 10";
        update_contact_op.set_place(place);
        const auto update_contact_result = update_contact_op.exec(ctx);
        ctx.get_log().info(boost::format("contact history id = %1%") % update_contact_result);
        ctx.get_log().info(boost::format("%1%") % info_contact_op.exec(ctx));

        LibFred::CopyContact copy_contact_op(
                contact_handle,
                copy_contact_handle,
                registrar_handle,
                0);
        const auto copy_contact_id = copy_contact_op.exec(ctx);
        ctx.get_log().info(boost::format("copy contact id = %1%") % copy_contact_id);
        ctx.get_log().info(boost::format("%1%") % info_contact_op.exec(ctx));
        ctx.get_log().info(boost::format("%1%") % LibFred::InfoContactById(copy_contact_id).exec(ctx));

        const auto non_unique_contacts = LibFred::Contact::FindContactDuplicates().exec(ctx);
        ctx.get_log().debug(boost::format("number of non-unique contacts: %1%") % non_unique_contacts.size());
        for (const auto& contact : non_unique_contacts)
        {
            ctx.get_log().debug("non-unique contact: " + contact);
        }

        LibFred::Contact::undisclose_address(ctx, info.info_contact_data.id, new_registrar_handle);
        LibFred::Contact::undisclose_address_async(
                LibFred::InfoContactByHandle(existing_contact_handle).exec(ctx).info_contact_data.id, registrar_handle);

        LibFred::DeleteContactByHandle delete_contact_op(contact_handle);
        delete_contact_op.exec(ctx);
        try
        {
            info_contact_op.exec(ctx);
        }
        catch (const std::exception& e)
        {
            ctx.get_log().info("contact " + contact_handle + " successfully deleted");
        }

        switch (LibFred::Contact::get_handle_syntax_validity(ctx, copy_contact_handle))
        {
            case LibFred::ContactHandleState::SyntaxValidity::valid:
                ctx.get_log().info(copy_contact_handle + " is valid contact handle");
                break;
            case LibFred::ContactHandleState::SyntaxValidity::invalid:
                ctx.get_log().info(copy_contact_handle + " is not valid contact handle");
                break;
        }

        switch (LibFred::Contact::get_handle_registrability(ctx, contact_handle))
        {
            case LibFred::ContactHandleState::Registrability::registered:
                ctx.get_log().info(contact_handle + " is registered contact handle");
                break;
            case LibFred::ContactHandleState::Registrability::in_protection_period:
                ctx.get_log().info(contact_handle + " is contact handle in protection period");
                break;
            case LibFred::ContactHandleState::Registrability::available:
                ctx.get_log().info(contact_handle + " is contact handle available for registration");
                break;
        }
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
