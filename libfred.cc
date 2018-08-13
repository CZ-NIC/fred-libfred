#include "src/libfred/registrable_object/contact/info_contact.hh"
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
            Database::Manager::init(new Database::ConnectionFactory(db_conn_str));
        }
        LibFred::OperationContextCreator ctx;
        LibFred::InfoContactByHandle info_contact_op("KONTAKT");
        const auto info = info_contact_op.exec(ctx);
        std::ostringstream out;
        out << info;
        std::cout << out.str() << std::endl;
        ctx.get_log().info(out.str());
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
