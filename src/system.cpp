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

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
    std::vector<int> pids = LinuxParser::Pids();
    for (int i = 0; i < (int) pids.size(); i++){
        this->processes_.push_back(Process(pids[i]));
    }
    //std::sort(this->processes_.begin(), this->processes_.end());
    std::reverse(this->processes_.begin(), this->processes_.end());
    return this->processes_; 
}

std::string System::Kernel() {
    std::string kernel = LinuxParser::Kernel();
    return kernel;
}

float System::MemoryUtilization() {
    float memUtilization = LinuxParser::MemoryUtilization();
    return memUtilization;
}

std::string System::OperatingSystem() { 
    std::string os = LinuxParser::OperatingSystem();
    return os;
}

int System::RunningProcesses() { 
    int runningProcesses = LinuxParser::RunningProcesses();
    return runningProcesses;
}

int System::TotalProcesses() {
    int totalProcesses = LinuxParser::TotalProcesses();
    return totalProcesses;
}

long int System::UpTime() {
    long int uptime = LinuxParser::UpTime();
    return uptime; 
}