#include "util/random.hh"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <atomic>

#include <sys/time.h>


namespace Random {

namespace {

boost::mt19937& get_engine()
{
    class RandomSequenceNumber
    {
    public:
        static RandomSequenceNumber& get_singleton()
        {
            static RandomSequenceNumber singleton;
            return singleton;
        }
        unsigned long next()
        {
            return static_cast<unsigned long>(++id_);
        }

    private:
        RandomSequenceNumber()
            : id_(pseudorandom_value())
        {
        }
        static unsigned long pseudorandom_value()
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return 1000 * 1000 * static_cast<unsigned long>(tv.tv_sec) + tv.tv_usec;
        }
        std::atomic<unsigned long> id_;
    };

    thread_local boost::mt19937 engine(RandomSequenceNumber::get_singleton().next());
    return engine;
}

} // namespace Random::{anonymous}

int integer(int min, int max)
{
    boost::uniform_int<> range(min, max);
    boost::variate_generator<decltype(get_engine()), boost::uniform_int<>> gen(get_engine(), range);
    return gen();
}

std::string string_from(unsigned len, const std::string& allowed)
{
    std::string out;
    out.reserve(len);
    for (unsigned int idx = 0; idx < len; ++idx)
    {
        out += allowed[integer(0, static_cast<int>(allowed.size() - 1))];
    }
    return out;
}

std::string string_lower_alpha(unsigned len)
{
    return string_from(len, "abcdefghijklmnopqrstuvwxyz");
}

std::string string_upper_alpha(unsigned len)
{
    return string_from(len, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

std::string string_alpha(unsigned len)
{
    return string_from(len, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

std::string string_alphanum(unsigned len)
{
    return string_from(len, "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "1234567890");
}

} // namespace Random
