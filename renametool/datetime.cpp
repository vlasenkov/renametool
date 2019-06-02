#include <iomanip>

#include "DateTime.hpp"

using namespace std;

void DateTime::read(const tm* dt) {
    m_year = 1900 + dt->tm_year;
    m_month = dt->tm_mon + 1;
    m_day = dt->tm_mday;
    m_hour = dt->tm_hour;
    m_minute = dt->tm_min;
    m_second = dt->tm_sec;
}

void DateTime::read(const char* dt, const char* format) {
    assert(sscanf(dt, format, &m_year, &m_month, &m_day, &m_hour, &m_minute, &m_second) != EOF);
}

ostream& DateTime::display(ostream& os) const {
    return os <<
        m_year << '/' <<
        setfill('0') << setw(2) << m_month << '/' <<
        setfill('0') << setw(2) << m_day << ' ' <<
        setfill('0') << setw(2) << m_hour << ':' <<
        setfill('0') << setw(2) << m_minute << ':' <<
        setfill('0') << setw(2) << m_second;
}


ostream& operator<<(ostream& os, DateTime const& dt) {
    return dt.display(os);
}
