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
    string version;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
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
    long value = 0, memTotal = 0, memFree = 0;
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
            }
        }
    }
    float memFreeGB = memFree / (1024 * 1024);
    float memTotalGB = memTotal / (1024 * 1024);

    return (memTotalGB - memFreeGB) / memTotalGB;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {

    long uptime = 0;
    string line;
    std::ifstream stream(kProcDirectory + kUptimeFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime;
    }

    return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    string line;
    string key;
    long jiffies = 0;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::stringstream linestream(line);
        int value;

        for (int i = 0; i < 9; ++i) {
            if (i == 0) {
                linestream >> key;
            } else {
                linestream >> value;
                jiffies += value;
            }
        }
    }
    return jiffies;
}


// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
    string line;
    string placeholder;
    long jiffies = 0;
    long process_jiffies = 0;
    string kPidDirectory = '/' + std::to_string(pid);
    std::ifstream filestream(kProcDirectory + kPidDirectory + kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        for (int token_id = 1; token_id <= 17; ++token_id) {
            if (token_id == CPUStates::kCstime ||
                token_id == CPUStates::kCutime ||
                token_id == CPUStates::kStime ||
                token_id == CPUStates::kUtime) {
                linestream >> jiffies;
                process_jiffies += jiffies;
            } else {
                linestream >> placeholder;
            }
        }
    }
    return process_jiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }


// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    string line;
    string key;
    long idleJiffies = 0;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::stringstream linestream(line);
        int value;

        for (int i = 0; i < 6; ++i) {
            if (i == 0) {
                linestream >> key;
            } else if (i > 3) {
                linestream >> value;
                idleJiffies += value;
            } else {
                linestream >> value;
            }
        }
    }
    return idleJiffies;
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
string LinuxParser::Command(int pid) {
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
string LinuxParser::Ram(int pid) {

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
string LinuxParser::Uid(int pid) {

    string line, key, value;
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatusFilename;
    std::ifstream stream(filename.str());

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key) {
                if (key == "Uid:") {
                    linestream >> value;
                    break;
                }
            }
        }
    }

    return value;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {

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
    long uptime{0};

    string line;
    string value;
    string placeholder;
    std::stringstream filename;
    filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
    std::ifstream filestream(filename.str());

    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);

        for (int i = 0; i <= 22; ++i) {
            if (i == CPUStates::kStarttime) {
                linestream >> uptime;
            } else {
                linestream >> placeholder;
            }
        }
    }
    return uptime / sysconf(_SC_CLK_TCK);
}