#ifndef DATETIME_HPP_
#define DATETIME_HPP_

#include <ctime>
#include <cstring>
#include <cassert>
#include <iostream>

class DateTime {
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_minute;
    int m_second;
public:
    void read(const std::tm* dt);
    void read(const char* dt, const char* format);
    std::ostream& display(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, DateTime const& dt);

#endif