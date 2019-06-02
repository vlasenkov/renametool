#include <fstream>

#include "JpgParser.hpp"
#include "ByteSwap.hpp"


using namespace std;
namespace bs = byteswap;


// JPEG marker: start of image
const uint16_t SOI  = 0xD8FF;
// JPEG marker: APP1
const uint16_t APP1 = 0xE1FF;
// JPEG marker: start of stream
const uint16_t SOS  = 0xDAFF;
// Motorola format indicator (aka big endian)
const uint16_t MM   = 0x4D4D;
// EXIF IDF offset tag
const uint16_t EXIF = 0x8769;
// DateTimeOriginal field tag
const uint16_t DTO  = 0x9003;


JpgParser::JpgParser()
    : m_exif_header("^Exif\\0\\0(?:MM|II)\\0\*.*", std::regex_constants::ECMAScript | regex::optimize)
{}


void JpgParser::read_datetime(const filesystem::path& filename, DateTime& dt) {
    ifstream input(filename, ios::binary);
    uint32_t size;
    uint16_t marker;

    input.read((char*) &marker, 2);
    assert(marker == SOI);

    while (input){
        input.read((char*) &marker, 2);
        switch (marker) {
        case APP1:
            input.read((char*) &marker, 2);
            marker = bs::u16(marker);
            if (marker >= 14) {
                char buf[20];
                input.read(buf, 14);
                assert(regex_match(buf, buf + 14, m_exif_header));
                uint64_t hdr_pos = input.tellg();
                hdr_pos -= 8;
                bool use_big = *(uint16_t*)(buf + 6) == MM;
                input.seekg(hdr_pos + bs::u32c(*(uint32_t*)(buf + 10), use_big));

                // read ifd
                uint16_t* fc = (uint16_t*) (buf + 12);
                input.read((char*) fc, 2);
                *fc = bs::u16c(*fc, use_big);
                for (auto i = 0; i < *fc; i++) {
                    input.read(buf, 12);
                    if (*(uint16_t*)buf == bs::u16c(EXIF, use_big))
                        goto exif_found;
                }
                assert(false);
            exif_found:
                assert(bs::u16c(*(uint16_t*)(buf + 2), use_big) == 4);
                assert(bs::u32c(*(uint32_t*)(buf + 4), use_big) == 1);

                input.seekg(hdr_pos + bs::u32c(*(uint32_t*)(buf + 8), use_big));

                input.read((char*)fc, 2);
                *fc = bs::u16c(*fc, use_big);
                for (auto i = 0; i < *fc; i++) {
                    input.read(buf, 12);
                    if (*(uint16_t*)buf == bs::u16c(DTO, use_big))
                        goto dto_found;
                }
                assert(false);
            dto_found:
                assert(bs::u16c(*(uint16_t*)(buf + 2), use_big) == 2);
                assert(bs::u32c(*(uint32_t*)(buf + 4), use_big) == 20);

                input.seekg(hdr_pos + bs::u32c(*(uint32_t*)(buf + 8), use_big));
                input.read(buf, 20);
                dt.read(buf, "%d:%d:%d %d:%d:%d");
                return;
            }
            else {
                input.seekg(marker - 2, ios_base::cur);
            }
            break;
        case SOS:
            assert(false);
        default:
            input.read((char*)& marker, 2);
            input.seekg(bs::u16(marker) - 2, ios_base::cur);
        }
    }
    assert(false);
}
