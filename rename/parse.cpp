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

#include "byteswap.hpp"


using namespace std;
namespace fs = filesystem;


class DateTime {
	int m_year;
	int m_month;
	int m_day;
	int m_hour;
	int m_minute;
	int m_second;
public:
	void read(const struct tm* dt) {
		m_year = 1900 + dt->tm_year;
		m_month = dt->tm_mon + 1;
		m_day = dt->tm_mday;
		m_hour = dt->tm_hour;
		m_minute = dt->tm_min;
		m_second = dt->tm_sec;
	}

	void read(const char* dt, const char* format) {
		assert(sscanf(dt, format, &m_year, &m_month, &m_day, &m_hour, &m_minute, &m_second) != EOF);
	}

	ostream& display(ostream& os) const {
		return os <<
			m_year << '/' <<
			setfill('0') << setw(2) << m_month << '/' <<
			setfill('0') << setw(2) << m_day << ' ' <<
			setfill('0') << setw(2) << m_hour << ':' <<
			setfill('0') << setw(2) << m_minute << ':' <<
			setfill('0') << setw(2) << m_second;
	}
};

ostream& operator<<(ostream& os, DateTime const& dt) {
	return dt.display(os);
}

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



void parse_mp4(const fs::path& filename, DateTime& dt) {
	ifstream input(filename, ios::binary);
	uint32_t size;
	char buf[5];
	time_t datetime;

	buf[4] = '\0';
	while (input) {
		input.read((char*) &size, 4);
		input.read(buf, 4);
		size = byteswap_u32(size);
		cerr << "atom: " << buf << " size: " << size << endl;
		if (strcmp(buf, "ftyp") == 0) {
			input.read(buf, 4);
			input.seekg(size - 12, ios_base::cur);
			cerr << "    file type: " << buf << endl;
		}
		else if (strcmp(buf, "mvhd") == 0) {
			input.seekg(4, ios_base::cur); // skip fields
			input.read(buf, 4);
			datetime = EPOCH_1904 + byteswap_u32(* (uint32_t*) buf);
			dt.read(gmtime(&datetime));
			return;
		}
		else if (strcmp(buf, "moov") != 0) {
			input.seekg(size - 8, ios_base::cur);
		}
	}
	return;
}

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


void parse_jpg(const fs::path& filename, DateTime& dt) {
	ifstream input(filename, ios::binary);
	uint32_t size;
	//char buf[5];
	uint16_t marker;


	regex exif_header("^Exif\\0\\0(?:MM|II)\\0\*.*", std::regex_constants::ECMAScript);

	input.read((char*) &marker, 2);
	assert(marker == SOI);

	while (input){
		input.read((char*) &marker, 2);
		switch (marker) {
		case APP1:
			input.read((char*) &marker, 2);
			marker = byteswap_u16(marker);
			if (marker >= 14) {
				char buf[20];
				input.read(buf, 14);
				assert(regex_match(buf, buf + 14, exif_header));
				uint64_t hdr_pos = input.tellg();
				hdr_pos -= 8;
				bool use_big = *(uint16_t*)(buf + 6) == MM;
				input.seekg(hdr_pos + cbyteswap_u32(*(uint32_t*)(buf + 10), use_big));

				// read ifd
				uint16_t* fc = (uint16_t*) (buf + 12);
				input.read((char*) fc, 2);
				*fc = cbyteswap_u16(*fc, use_big);
				for (auto i = 0; i < *fc; i++) {
					input.read(buf, 12);
					if (*(uint16_t*)buf == cbyteswap_u16(EXIF, use_big))
						goto exif_found;
				}
				assert(false);
			exif_found:
				assert(cbyteswap_u16(*(uint16_t*)(buf + 2), use_big) == 4);
				assert(cbyteswap_u32(*(uint32_t*)(buf + 4), use_big) == 1);

				input.seekg(hdr_pos + cbyteswap_u32(*(uint32_t*)(buf + 8), use_big));

				input.read((char*)fc, 2);
				*fc = cbyteswap_u16(*fc, use_big);
				for (auto i = 0; i < *fc; i++) {
					input.read(buf, 12);
					if (*(uint16_t*)buf == cbyteswap_u16(DTO, use_big))
						goto dto_found;
				}
				assert(false);
			dto_found:
				assert(cbyteswap_u16(*(uint16_t*)(buf + 2), use_big) == 2);
				assert(cbyteswap_u32(*(uint32_t*)(buf + 4), use_big) == 20);

				input.seekg(hdr_pos + cbyteswap_u32(*(uint32_t*)(buf + 8), use_big));
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
			input.seekg(byteswap_u16(marker) - 2, ios_base::cur);
		}
	}
	assert(false);
}


int main()
{
	//char path[] = "C:\\Users\\leovl\\Desktop\\New folder\\100GOPRO_conv";
	//char path[] = "C:\\Users\\leovl\\Desktop\\New folder\\100GOPRO";
	char path[] = "C:\\Users\\leovl\\Desktop\\New folder\\other";
	regex jpg(".*\.jpg", std::regex_constants::ECMAScript);
	DateTime dt;
	for (auto& p : fs::directory_iterator(path)) {
		if (regex_match(p.path().string(), jpg)) {
			cout << p.path() << endl;
			parse_jpg(p.path(), dt);
			cout << dt << endl;
		}
	}
		//parse_mp4(p.path()); */
		//std::cout << p.path() << '\n';
	/*for (auto& p : fs::directory_iterator(path))
		parse_mp4(p.path());*/
		//std::cout << p.path() << '\n';
}
