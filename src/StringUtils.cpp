#include "StringUtils.h"

#include <sstream>

std::string asString(float val)
{
    std::ostringstream strm;
    strm << val;
    return strm.str();
}
