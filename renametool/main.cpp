#include <fstream>
#include <iterator>
#include <iostream>
#include <cassert>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <cstring>
#include <regex>
#include <cassert>

#include "JpgParser.hpp"
#include "Mp4Parser.hpp"


using namespace std;
namespace fs = filesystem;



enum FileType {
    UNK = 0,
    JPG,
    MP4,
    MOV,
};


class FileTypeDetector {
    regex m_mp4;
    regex m_jpg;
    regex m_mov;
public:

    FileTypeDetector()
        : m_mp4("[.](?:mp4|mpeg4)", regex::ECMAScript | regex::icase | regex::optimize)
        , m_jpg("[.](?:jpg|jpeg)", regex::ECMAScript | regex::icase | regex::optimize)
        , m_mov("[.]mov", regex::ECMAScript | regex::icase | regex::optimize)
    {}

    FileType detect(const fs::path& filename) {
        auto extension = filename.extension().string();
        if (regex_match(extension, m_jpg)) {
            return FileType::JPG;
        }
        else if (regex_match(extension, m_mp4)) {
            return FileType::MP4;
        }
        else if (regex_match(extension, m_mov)) {
            return FileType::MOV;
        }
        else {
            return FileType::UNK;
        }
    }
};


int main(int argc, char *argv[])
{
    assert(argc > 1);

    DateTime dt;
    FileTypeDetector ftype;
    JpgParser jpg;
    Mp4Parser mp4;

    for (auto& p : fs::directory_iterator(argv[1])) {
        auto path = p.path();
        cout << path.filename();
        switch (ftype.detect(path)) {
        case FileType::JPG:
            jpg.read_datetime(path, dt);
            break;
        case FileType::MP4:
            mp4.read_datetime(path, dt);
            break;
        default:
            assert(false);
        }
        cout << '\t' << dt << '\n';
    }
}
