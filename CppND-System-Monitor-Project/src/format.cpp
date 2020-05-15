#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    long h = seconds / 3600;
    long m = (seconds % 3600) / 60;
    long s = (seconds % 3600 % 60);

    string hour = (h > 10 ? "" : "0") + std::to_string(h);
    string minute = (m > 10 ? "" : "0") + std::to_string(m);
    string second = (s > 10 ? "" : "0") + std::to_string(s);

    return hour + ":" + minute + ":" + second;
}