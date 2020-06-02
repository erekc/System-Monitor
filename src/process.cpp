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
    this->ram = this->Ram();
}

int Process::Pid() { return this->pid; }

float Process::CpuUtilization() {
    std::vector<std::string> utils = LinuxParser::ProcessCpuUtilization(this->pid);
    std::string::size_type sz;
    float totalTime = std::stol(utils[LinuxParser::ProcStat::kUtime_], &sz) + 
                    std::stol(utils[LinuxParser::ProcStat::kStime_], &sz);
    float seconds = LinuxParser::UpTime() - (this->UpTime() / sysconf(_SC_CLK_TCK));
    // float utilization = 100 * ((totalTime / sysconf(_SC_CLK_TCK)) / seconds);
    float utilization = (totalTime / sysconf(_SC_CLK_TCK)) / seconds;
    return utilization;
}

string Process::Command() { return LinuxParser::Command(this->pid); }

string Process::Ram() { return LinuxParser::Ram(this->pid); }

string Process::User() { return LinuxParser::User(this->pid); }

long int Process::UpTime() { return LinuxParser::UpTime(this->pid); }

bool Process::operator<(Process const& a){
    std::string::size_type sz;
    return std::stod(this->ram, &sz) < std::stod(a.ram, &sz);
    // return this->utilization < a.utilization;
}