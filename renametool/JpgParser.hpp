#ifndef JPG_HPP_
#define JPG_HPP_

#include <regex>

#include "IParser.hpp"

class JpgParser : public IParser {
    std::regex m_exif_header;
public:
    JpgParser();
	void read_datetime(const std::filesystem::path& filename, DateTime& dt);
};

#endif