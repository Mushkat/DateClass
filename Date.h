#ifndef DATE_H
#define DATE_H

#include <string>

class Date {
 public:
  Date();

  Date(int day, int month, int year);

  int GetDay() const;

  int GetMonth() const;

  int GetYear() const;

  int DiffDays(const Date& other) const;

  Date Add(int days) const;

  Date Subtract(int days) const;

  Date Replace(int day = 0, int month = 0, int year = 0) const;

  int Week() const;

  int TotalWeeks() const;

  std::string IsoFormat() const;

 private:
  int day_, month_, year_;

  int ToDays() const;

  static int GetDaysCount(int month, int year);

  static bool IsLeapYear(int year);

  static Date FromDays(int total_days);
};

#endif // DATE_H