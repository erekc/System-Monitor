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

template <typename T>
T LinuxParser::findValueByKey(const std::string &keyFilter, std::string &filename){
  std::string line;
  std::string key;
  T val;
  std::ifstream filestream(filename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == keyFilter){
        return val;
      }
    }
  }
  filestream.close();
  return val;
}

template <typename T>
T LinuxParser::findValueOfFile(std::string &filename){
  T val;
  std::string line;
  std::ifstream filestream(filename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> val;
  }
  filestream.close();
  return val;
}

float LinuxParser::MemoryUtilization() {
  float memUsed;
  float memUtilization;

  std::string filename{kProcDirectory + kMeminfoFilename};

  float memTotal = findValueByKey<float>(filterMemTotalString, filename);
  float memFree = findValueByKey<float>(filterMemFreeString, filename);
  float buffers = findValueByKey<float>(filterBuffers, filename);
  float cached = findValueByKey<float>(filterCached, filename);
  float slab = findValueByKey<float>(filterSlab, filename);
  
  memUsed = memTotal - memFree - buffers - cached - slab;
  memUtilization = memUsed / memTotal;
  return memUtilization; 
}

long LinuxParser::UpTime() {
  std::string filename{kProcDirectory + kUptimeFilename};
  long uptime = findValueOfFile<long>(filename);
  return uptime;
}

long LinuxParser::Jiffies() {
  long jiffies = UpTime() * sysconf(_SC_CLK_TCK);
  return jiffies;
}

// long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

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

long LinuxParser::IdleJiffies() {
  std::vector<std::string> jiffies = CpuUtilization();
  std::string::size_type sz;
  long idle = std::stol(jiffies[CPUStates::kIdle_], &sz);
  long iowait = std::stol(jiffies[CPUStates::kIOwait_], &sz);
  long totalIdle = idle + iowait;
  return totalIdle;
}

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

int LinuxParser::TotalProcesses() {
  std::string filename{kProcDirectory + kStatFilename};
  int totalProcesses = findValueByKey<int>(filterProcesses, filename);
  return totalProcesses;
}

int LinuxParser::RunningProcesses(){
  std::string filename{kProcDirectory + kStatFilename};
  int runningProcesses = findValueByKey<int>(filterRunningProcesses, filename);
  return runningProcesses;
}

string LinuxParser::Command(int pid) {
  std::string filename{kProcDirectory + std::to_string(pid) + kCmdlineFilename};
  std::string command = findValueOfFile<std::string>(filename);
  return command;
}

string LinuxParser::Ram(int pid) {
  std::string filename{kProcDirectory + std::to_string(pid) + kStatusFilename};
  std::string ram = findValueByKey<std::string>(filterProcMem, filename);
  std::istringstream convert(ram);
  float convertedRam;
  convert >> convertedRam;
  convertedRam *= 0.001;
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << convertedRam;
  return stream.str();
}

string LinuxParser::Uid(int pid) {
  std::string filename{kProcDirectory + std::to_string(pid) + kStatusFilename};
  std::string uid = findValueByKey<std::string>(filterUID, filename);
  return uid;
}

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

long LinuxParser::UpTime(int pid) {
  std::vector<std::string> utils = ProcessCpuUtilization(pid);
  std::string uptimeString = utils[ProcStat::kStarttime_];
  std::string::size_type sz;
  long startTime = std::stol(uptimeString, &sz);
  long upTime = UpTime() - (startTime / sysconf(_SC_CLK_TCK));
  return upTime;
}