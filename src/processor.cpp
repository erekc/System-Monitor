#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

float Processor::Utilization() {
    long totalJiffies = LinuxParser::Jiffies();
    long activeJiffies = LinuxParser::ActiveJiffies();
    long idleJiffies = LinuxParser::IdleJiffies();
    float cpuUtilization = (activeJiffies - idleJiffies) / static_cast<float>(totalJiffies);
    return cpuUtilization;
}