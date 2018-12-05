#include "util/util.hh"

#include <boost/format.hpp>

namespace Util {

HeadSeparator::HeadSeparator(const std::string& head, const std::string& separator)
    : got_head_(false),
      head_(head),
      separator_(separator)
{ }

std::string HeadSeparator::get()
{
    if (got_head_)
    {
        return separator_;
    }
    got_head_ = true;
    return head_;
}

void HeadSeparator::reset()
{
    got_head_ = false;
}

std::string escape(std::string input, const std::string& what, const std::string& esc_by)
{
    std::size_t idx = 0;
    while ((idx = input.find_first_of(what, idx)) != input.npos)
    {
        input.insert(idx, esc_by);
        idx += esc_by.length() + 1;
    }
    return input;
}

std::string escape2(std::string input)
{
    return escape(input, "'\\", "\\");
}

std::string make_svtrid(unsigned long long request_id)
{
    return boost::str(boost::format("ReqID-%010d") % request_id);
}

}//namespace Util
