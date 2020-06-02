#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    std::vector<int> pids = LinuxParser::Pids();
    for (int i = 0; i < (int) pids.size(); i++){
        this->processes_.push_back(Process(pids[i]));
    }
    //std::sort(this->processes_.begin(), this->processes_.end());
    std::reverse(this->processes_.begin(), this->processes_.end());
    return this->processes_; 
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
    std::string kernel = LinuxParser::Kernel();
    return kernel;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() {
    float memUtilization = LinuxParser::MemoryUtilization();
    return memUtilization;
}

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
    std::string os = LinuxParser::OperatingSystem();
    return os;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { 
    int runningProcesses = LinuxParser::RunningProcesses();
    return runningProcesses;
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
    int totalProcesses = LinuxParser::TotalProcesses();
    return totalProcesses;
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {
    long int uptime = LinuxParser::UpTime();
    return uptime; 
}