#include "Date.h"
#include <iomanip>

Date::Date() : day_(1), month_(1), year_(1970) {}

Date::Date(int day, int month, int year) {
  if (day < 1 || day > GetDaysCount(month, year)) {
    day_ = 1;
    month_ = 1;
    year_ = 1970;
    return;
  }
  day_ = day;
  month_ = month;
  year_ = year;
}

int Date::GetDay() const {
  return day_;
}

int Date::GetMonth() const {
  return month_;
}

int Date::GetYear() const {
  return year_;
}

int Date::DiffDays(const Date& other) const {
  return std::abs(ToDays() - other.ToDays());
}

Date Date::Add(int days) const {
  return FromDays(ToDays() + days);
}

Date Date::Subtract(int days) const {
  return FromDays(ToDays() - days);
}

Date Date::Replace(int day, int month, int year) const {
  return {day == 0 ? day_ : day, month == 0 ? month_ : month,
          year == 0 ? year_ : year};
}

int Date::Week() const {
  Date start_of_year(1, 1, year_);
  int days = DiffDays(start_of_year);
  return days / 7;
}

int Date::TotalWeeks() const {
  Date start_of_year(1, 1, 1970);
  int days = DiffDays(start_of_year);
  return days / 7;
}

std::string Date::IsoFormat() const {
  std::ostringstream oss;
  oss << std::setw(4) << std::setfill('0') << year_ << "-"
      << std::setw(2) << std::setfill('0') << month_ << "-"
      << std::setw(2) << std::setfill('0') << day_;
  return oss.str();
}

int Date::GetDaysCount(int month, int year) {
  static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
                                      30, 31};
  if (month < 1 || month > 12 || year < 1) {
    return 0;
  }
  if (month == 2 && IsLeapYear(year)) {
    return 29;
  }
  return days_in_month[month - 1];
}

bool Date::IsLeapYear(int year) {
  if (year % 4 == 0) {
    if (year % 100 == 0) {
      return year % 400 == 0;
    }
    return true;
  }
  return false;
}

int Date::ToDays() const {
  int days = 0;
  for (int y = 1; y < year_; ++y) {
    days += IsLeapYear(y) ? 366 : 365;
  }
  for (int m = 1; m < month_; ++m) {
    days += GetDaysCount(m, year_);
  }
  days += day_;
  return days;
}

Date Date::FromDays(int total_days) {
  int y = 1;
  while (total_days > (IsLeapYear(y) ? 366 : 365)) {
    total_days -= IsLeapYear(y) ? 366 : 365;
    y++;
  }

  int m = 1;
  while (total_days > GetDaysCount(m, y)) {
    total_days -= GetDaysCount(m, y);
    m++;
  }

  return {total_days, m, y};
}