#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    this->pid = pid;
    this->utilization = this->CpuUtilization();
}
// TODO: Return this process's ID
int Process::Pid() { return this->pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    std::vector<std::string> utils = LinuxParser::ProcessCpuUtilization(this->pid);
    std::string::size_type sz;
    float totalTime = std::stol(utils[LinuxParser::ProcStat::kUtime_], &sz) + 
                    std::stol(utils[LinuxParser::ProcStat::kStime_], &sz) +
                    std::stol(utils[LinuxParser::ProcStat::kCUtime_], &sz) +
                    std::stol(utils[LinuxParser::ProcStat::kCStime_], &sz);
    float seconds = LinuxParser::UpTime() - (this->UpTime() / sysconf(_SC_CLK_TCK));
    // float utilization = 100 * ((totalTime / sysconf(_SC_CLK_TCK)) / seconds);
    float utilization = (totalTime / sysconf(_SC_CLK_TCK)) / seconds;
    return utilization;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(this->pid); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(this->pid); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->pid); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(this->pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a){
    //std::string::size_type sz;
    //return std::stod(this->ram, &sz) < std::stod(a.ram, &sz) ? true : false;
    return this->utilization < a.utilization ? true : false;
}