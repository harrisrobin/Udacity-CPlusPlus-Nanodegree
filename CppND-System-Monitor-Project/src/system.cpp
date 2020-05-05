#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <linux_parser.h>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
    cpu_ = Processor();
}

// DONE: Return the system's CPU
Processor &System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process> &System::Processes() {
    // get the list of PIDS.
    vector<int> pids = LinuxParser::Pids();
    // clear the vector of processes.
    processes_ = {};

    // loop over the list of Pids
    for (int &pid : pids) {
        long elapsedTime = LinuxParser::UpTime() - LinuxParser::UpTime(pid);
        float cpuUtlization = (float) LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK) / elapsedTime;
        Process process(pid, cpuUtlization);
        processes_.push_back(process);
    }

    std::sort(processes_.begin(), processes_.end(), sortProcesses);

    return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }


bool System::sortProcesses(Process p, Process q) { return q < p; }
