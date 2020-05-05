#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "PRETTY_NAME") {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
    string os, kernel;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> kernel;
    }
    return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
    vector<int> pids;
    DIR *directory = opendir(kProcDirectory.c_str());
    struct dirent *file;
    while ((file = readdir(directory)) != nullptr) {
        // Is this a directory?
        if (file->d_type == DT_DIR) {
            // Is every character of the name a digit?
            string filename(file->d_name);
            if (std::all_of(filename.begin(), filename.end(), isdigit)) {
                int pid = stoi(filename);
                pids.push_back(pid);
            }
        }
    }
    closedir(directory);
    return pids;
}


// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    string line;
    string key;
    long value = 0, memTotal = 0, memFree = 0, memAvailable = 0;
    std::ifstream filestream(kProcDirectory + kMeminfoFilename);

    if (filestream.is_open()) {
        int countToRead = 0;

        while (countToRead < 4 && std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "MemTotal:") {
                    memTotal = value;
                    countToRead++;
                    continue;
                }
                if (key == "MemFree:") {
                    memFree = value;
                    countToRead++;
                    continue;
                }
                if (key == "MemAvailable:") {
                    memAvailable = value;
                    countToRead++;
                    continue;
                }
            }
        }
    }
    float memFreeGB = memFree / (1024 * 1024);
    float memTotalGB = memTotal / (1024 * 1024);

    return (memTotalGB - memFreeGB) / memTotalGB;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {

    long idletime = 0, uptime = 0;
    string line;
    std::ifstream stream(kProcDirectory + kUptimeFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime >> idletime;
    }

    return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    std::ifstream filestream(kProcDirectory + kStatFilename);

    if (filestream.is_open()) {
        std::string line;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        std::string cpu;
        long user;
        long nice;
        long system;
        long idle;
        long iowait;
        long irq;
        long softirq;
        long steal;
        long guest;
        long guest_nice;
        linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
        long userTime = user - guest;
        long niceTime = nice - guest_nice;
        long idleAllTime = idle + iowait;
        long systemAllTime = system + irq + softirq;
        long virtualTime = guest + guest_nice;
        long totalTime = userTime + niceTime + systemAllTime + idleAllTime + steal + virtualTime;
        return totalTime;
    }

    return 0;
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
    std::ifstream filestream(filename.str());

    if (filestream.is_open()) {
        std::string line;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        std::string ignore;
        long utime;
        long stime;
        long cutime;
        long cstime;
        long starttime;

        for (int i = 0; i < 13; i++) linestream >> ignore;
        linestream >> utime >> stime >> cutime >> cstime;
        for (int i = 0; i < 4; i++) linestream >> ignore;
        linestream >> starttime;
        return utime + stime + cutime + cstime + starttime;

    }
    return 0;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    std::ifstream filestream(kProcDirectory + kUptimeFilename);
    long upTime = 0;
    long idleTime;

    if (filestream.is_open()) {
        std::string line;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> upTime >> idleTime;
    }
    return 0;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    string os, kernel;
    string line;
    std::ifstream filestream(kProcDirectory + kStatFilename);


    if (filestream.is_open()) {
        std::string line;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        std::string cpu;
        long user;
        long nice;
        long system;
        long idle;
        long iowait;
        long irq;
        long softirq;
        long steal;
        long guest;
        long guest_nice;
        linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
        long userTime = user - guest;
        long niceTime = nice - guest_nice;
        long idleAllTime = idle + iowait;
        long systemAllTime = system + irq + softirq;
        long virtualTime = guest + guest_nice;
        return idleAllTime;
    }

    return 0;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {

    vector<string> cpuVect;
    string value, line;

    std::ifstream filestream(kProcDirectory + kStatFilename);

    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);

        while (linestream >> value) {
            if (value != "cpu") {
                cpuVect.push_back(value);
            }
        }
    }

    return cpuVect;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);

    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "processes") {
                    return stoi(value);
                }
            }
        }
    }
    return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);

    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "procs_running") {
                    return stoi(value);
                }
            }
        }
    }
    return 0;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) {
    string line;
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kCmdlineFilename;
    std::ifstream stream(filename.str());

    if (stream.is_open()) {
        std::getline(stream, line);
    }

    return line;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) {

    string line, key, value;
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatusFilename;
    std::ifstream stream(filename.str());

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "VmSize:") {
                    return to_string(stoi(value) / 1024);
                }
            }
        }
    }

    return string();
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) {

    string line, key, value;
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatusFilename;
    std::ifstream stream(filename.str());

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "Uid:") {
                    return to_string(stoi(value) / 1024);
                }
            }
        }
    }

    return string();
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) {

    string line;
    string user, x, suid;
    string uid = LinuxParser::Uid(pid);
    std::ifstream filestream(kPasswordPath);

    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream stringstream(line);
            stringstream >> user >> x >> suid;
            if (suid.compare(uid) == 0) {
                return user;
            }
        }
    }

    return string();
}


// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
    long int uptime{0};
    string value;
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
    std::ifstream stream(filename.str());

    if (stream.is_open()) {
        for (int i = 0; stream >> value; ++i)
            if (i == 13) {
                uptime = stol(value);
                return uptime;
            }
    }
    return 0;
}