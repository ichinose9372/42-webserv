#include "Utils.hpp"

std::string Utils::my_to_string(size_t number)
{
    std::ostringstream os;
    os << number;
    return os.str();
}