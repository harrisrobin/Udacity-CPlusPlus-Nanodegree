#include "ncurses_display.h"
#include "system.h"

//Debugging
#include "iostream"
#include "linux_parser.h"

using namespace std;

int main() {
    System system;
//  NCursesDisplay::Display(system);

    auto output = LinuxParser::RunningProcesses();

    cout << output << "\n";

}