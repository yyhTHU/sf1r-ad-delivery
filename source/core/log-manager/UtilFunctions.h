#ifndef _LOG_MANAGER_UTIL_FUNCTIONS_H_
#define _LOG_MANAGER_UTIL_FUNCTIONS_H_

#include <util/ustring/UString.h>

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace sf1r {

template <typename T>
std::string toBytes(const T& val)
{
    return std::string(reinterpret_cast<const char *>(&val), sizeof(T));
}

template <typename T>
T fromBytes(const std::string& str)
{
    return *(reinterpret_cast<const T *>(str.c_str()));
}

time_t createTimeStamp();

time_t createTimeStamp(boost::posix_time::ptime pt);

time_t createTimeStamp(const izenelib::util::UString& text);

time_t createTimeStamp(const std::string& text);

}

#endif