#include <iostream>
#include "Date.h"

int main() {
  try {
    int day1, month1, year1;
    int day2, month2, year2;
    std::cout <<
        "Enter the day, month, and year of the first date separated by spaces"
        << '\n';
    std::cin >> day1 >> month1 >> year1;
    std::cout <<
        "Enter the day, month, and year of the second date separated by spaces"
        << '\n';
    std::cin >> day2 >> month2 >> year2;
    Date date1(day1, month1, year1);
    Date date2(day2, month2, year2);
    std::cout << "You entered: " << date1.IsoFormat() << " and " << date2.
        IsoFormat() << '\n';

    std::cout << "Difference between dates in days: " << date1.DiffDays(date2)
        << '\n';

    Date new_date = date1.Add(30);
    std::cout << "First date + 30 days: " << new_date.IsoFormat() << '\n';

    Date subtracted_date = date1.Subtract(15);
    std::cout << "First date - 15 days: " << subtracted_date.IsoFormat() <<
        '\n';

    Date replaced_date = date1.Replace(15, 2, 2021);
    std::cout << "Change 1st date to 2021--02--15: " << replaced_date.
        IsoFormat() << '\n';

    std::cout << "Weeks in the 1st date from the beginning of the year: " <<
        date1.Week() << '\n';
    std::cout << "Weeks in first date since 1970: " << date1.TotalWeeks() <<
        '\n';
  } catch (const std::exception& e) {
    std::cout << e.what() << '\n';
  }

  return 0;
}