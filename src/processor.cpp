#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    long totalJiffies = LinuxParser::Jiffies();
    long activeJiffies = LinuxParser::ActiveJiffies();
    long idleJiffies = LinuxParser::IdleJiffies();
    float cpuUtilization = (activeJiffies - idleJiffies) / (float) (totalJiffies);
    return cpuUtilization;
}