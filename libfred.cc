#include "src/libfred/registrable_object/contact/info_contact.hh"

#include <exception>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
    const char* const db_conn_str = argc < 2 ? "host=localhost port=11112 dbname=fred user=fred"
                                             : argv[1];
    try
    {
        Database::Manager::init(new Database::ConnectionFactory(db_conn_str));
        LibFred::OperationContextCreator ctx;
        LibFred::InfoContactByHandle info_contact_op("KONTAKT");
        info_contact_op.exec(ctx);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
