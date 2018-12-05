#ifndef CONTEXT_HH_1A3983AA309849AAA28B99D050A00D21
#define CONTEXT_HH_1A3983AA309849AAA28B99D050A00D21

#include <string>

namespace Logging {

class Context
{
public:
    Context();

    explicit Context(const std::string& name);

    ~Context();

    Context(const Context&) = delete;

    Context& operator=(const Context&) = delete;

    /**
     * stacked interface implementation
     */
    static void push(const std::string& name);

    static void pop();

    static std::string top();

    static std::string getNDC();

    /**
     * mapped interface implementation
     */
    static void add(const std::string& attr, const std::string& val);

    static void rem(const std::string& attr);

    static std::string getMDC();

    /**
     *  get both contexts - stacked and mapped
     */
    static std::string get();

    static void clear();
};

}//namespace Logging

#endif//CONTEXT_HH_1A3983AA309849AAA28B99D050A00D21
