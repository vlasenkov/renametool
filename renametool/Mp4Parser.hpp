#ifndef MP4_HPP_
#define MP4_HPP_

#include <regex>

#include "IParser.hpp"

class Mp4Parser : public IParser {
public:
    Mp4Parser();
    void read_datetime(const std::filesystem::path& filename, DateTime& dt);
};

#endif