#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
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
  string os, kernel, version;
  string line;
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
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memUsed, memUtilization, memTotal, memFree, buffers, cached, slab;
  std::string line, tag;
  std::ifstream memoryStream(kProcDirectory + kMeminfoFilename);
  if (memoryStream.is_open()){
    while (std::getline(memoryStream, line)) {
      std::istringstream linestream(line);
      linestream >> tag;
      if (tag.compare("MemTotal:") == 0){
        linestream >> memTotal;
      }
      if (tag.compare("MemFree:") == 0){
        linestream >> memFree;
      }
      if (tag.compare("Buffers:") == 0){
        linestream >> buffers;
      }
      if (tag.compare("Cached:") == 0){
        linestream >> cached;
      }
      if (tag.compare("Slab:") == 0){
        linestream >> slab;
        break;
      }
    }
  }
  memUsed = memTotal - memFree - buffers - cached - slab;
  memUtilization = memUsed / memTotal;
  return memUtilization; 
}

long LinuxParser::UpTime() {
  long uptime, idle;
  std::string line;
  std::ifstream uptimeStream(kProcDirectory+kUptimeFilename);
  if (uptimeStream.is_open()){
    std::getline(uptimeStream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies = UpTime() * sysconf(_SC_CLK_TCK);
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> jiffies = CpuUtilization();
  long jiffyTotal = 0;
  long indiJiffy;
  for (int i = 0; i < 10; i++){
    std::string jiffy = jiffies[i];
    std::istringstream convert(jiffy);
    convert >> indiJiffy;
    jiffyTotal += indiJiffy;
  }
  return jiffyTotal;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> jiffies = CpuUtilization();
  std::string::size_type sz;
  long idle = std::stol(jiffies[CPUStates::kIdle_], &sz);
  long iowait = std::stol(jiffies[CPUStates::kIOwait_], &sz);
  long totalIdle = idle + iowait;
  return totalIdle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::string line;
  std::ifstream cpustream(kProcDirectory + kStatFilename);
  std::vector<string> utils;
  std::string state;
  std::string tag;
  if (cpustream.is_open()){
    std::getline(cpustream, line);
    std::istringstream cpuline(line);
    cpuline >> tag;
    for (int i = 0; i < 10; i++){
      cpuline >> state;
      utils.push_back(state);
    }
  }
  return utils; 
}

vector<string> LinuxParser::ProcessCpuUtilization(int pid) {
  std::string line, value;
  std::ifstream cpuStream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::vector<std::string> utils;
  std::getline(cpuStream, line);
  std::istringstream linestream(line);
  for (int i = 0; i < 22; i++){
    linestream >> value;
    if (i == 13 || i == 14 || i == 15 || i == 16 || i == 21){
      utils.push_back(value);
    }
  }
  return utils;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcesses;
  std::string line, tag = "dummy";
  std::ifstream statstream(kProcDirectory + kStatFilename);
  if (statstream.is_open()){
    while (!(tag.compare("processes")==0)){
      std::getline(statstream, line);
      std::istringstream linestream(line);
      linestream >> tag;
    }
    std::istringstream linestream(line);
    linestream >> tag >> totalProcesses;
  }
  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses(){
  int runningProcesses;
  std::string line, tag = "dummy";
  std::ifstream statstream(kProcDirectory + kStatFilename);
  if (statstream.is_open()){
    while (!(tag.compare("procs_running") == 0 )){
      std::getline(statstream, line);
      std::istringstream linestream(line);
      linestream >> tag;
    }
    std::istringstream linestream(line);
    linestream >> tag >> runningProcesses;
  }
  return runningProcesses;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream commandStream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::string command;
  if (commandStream.is_open()){
    std::getline(commandStream, command);
  }
  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream ramStream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  std::string line, tag, ram;
  if (ramStream.is_open()){
    while(1){
      std::getline(ramStream, line);
      std::istringstream linestream(line);
      linestream >> tag;
      if (tag.compare("VmSize:") == 0){
        linestream >> ram;
        break;
      }
    }
  }
  std::istringstream convert(ram);
  float convertedRam;
  convert >> convertedRam;
  convertedRam *= 0.001;
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << convertedRam;
  return stream.str();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string line, tag, uid;
  std::ifstream uidStream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (uidStream.is_open()){
    while(1){
      std::getline(uidStream, line);
      std::istringstream linestream(line);
      linestream >> tag;
      if (tag.compare("Uid:") == 0){
        linestream >> uid;
        break;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string line, user, x, uid;
  std::ifstream userStream(kPasswordPath);
  if (userStream.is_open()){
    while(1){
      std::getline(userStream, line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if (uid.compare(Uid(pid)) == 0){
        break;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long uptime;
  std::vector<std::string> utils = ProcessCpuUtilization(pid);
  std::string uptimeString = utils.back();
  std::istringstream convert(uptimeString);
  convert >> uptime;
  return uptime;
}