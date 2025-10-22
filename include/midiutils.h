#pragma once

#include <libremidi/libremidi.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>

inline std::ostream& operator<<(std::ostream& s, const libremidi::message& message)
{
    auto nBytes = message.size();
    s << "[ ";
    for (auto i = 0U; i < nBytes; i++)
        s << std::hex << (int)message[i] << std::dec << " ";
    s << "]";
    if (nBytes > 0)
        s << " ; stamp = " << message.timestamp;
    return s;
}