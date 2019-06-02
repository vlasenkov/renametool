#include <fstream>

#include "Mp4Parser.hpp"
#include "ByteSwap.hpp"


using namespace std;
namespace bs = byteswap;


// this constant is produced by:
//
// #include <ctime>
//
// std::tm time1904;
// time1904.tm_mday = 1;
// time1904.tm_year = 4; 
// std::time_t t1904 = std::mktime(&time1904);
//
const time_t EPOCH_1904 = -2082853817;

const uint32_t FTYP = 0x70797466;
const uint32_t MVHD = 0x6468766d;
const uint32_t MOOV = 0x766f6f6d;


Mp4Parser::Mp4Parser() {
}


void Mp4Parser::read_datetime(const filesystem::path& filename, DateTime& dt) {
    ifstream input(filename, ios::binary);
    uint32_t size;
    uint32_t buf;
    time_t datetime;

    while (input) {
        input.read((char*) &size, 4);
        input.read((char*) &buf, 4);
        size = bs::u32(size);
        //cerr << "atom: " << buf << " size: " << size << endl;
        // if (buf == FTYP) {
        //  input.read(buf, 4);
        //  input.seekg(size - 12, ios_base::cur);
        //  cerr << "    file type: " << buf << endl;
        // }
        if (buf == MVHD) {
            input.seekg(4, ios_base::cur); // skip fields
            input.read((char*) &buf, 4);
            datetime = EPOCH_1904 + bs::u32(buf);
            dt.read(gmtime(&datetime));
            return;
        }
        else if (buf != MOOV) {
            input.seekg(size - 8, ios_base::cur);
        }
    }
    return;
}