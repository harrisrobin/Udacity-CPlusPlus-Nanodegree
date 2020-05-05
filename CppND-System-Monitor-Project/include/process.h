#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
public:
    Process(int pid, float cpuUtilization) : pid_(pid), cpuUtilization(cpuUtilization) {};

    int Pid();                               // DONE: See src/process.cpp
    std::string User();                      // DONE: See src/process.cpp
    std::string Command();                   // DONE: See src/process.cpp
    float &CpuUtilization();                  // DONE: See src/process.cpp
    std::string Ram();                       // DONE: See src/process.cpp
    long int UpTime();                       // DONE: See src/process.cpp
    bool operator<(Process &a);  // DONE: See src/process.cpp

    // TODO: Declare any necessary private members
private:
    int pid_ = 0;
    float cpuUtilization = 0.0;

};

#endif