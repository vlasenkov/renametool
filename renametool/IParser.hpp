#ifndef IPARSER_HPP_
#define IPARSER_HPP_

#include <filesystem>

#include "DateTime.hpp"

class IParser {
public:
    virtual ~IParser() {};
    virtual void read_datetime(const std::filesystem::path& filename, DateTime& dt) = 0;
};

#endif