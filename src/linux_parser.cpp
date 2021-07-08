#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);  // /proc/meminfo
  string key;
  string value;
  string line;
  float TotalUsed_, Memtotal_, MemFree_;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          value.erase(value.rfind("kB"), 2);  // discard the last "kB"
          value.erase(std::remove(value.begin(), value.end(), '_'),
                      value.end());
          Memtotal_ = std::stof(value);  // string to float
        }
        if (key == "MemFree") {
          value.erase(value.rfind("kB"), 2);  // discard the last "kB"
          value.erase(std::remove(value.begin(), value.end(), '_'),
                      value.end());
          MemFree_ = std::stof(value);  // string to float
        }
      }
    }
  }
  TotalUsed_ = (Memtotal_ - MemFree_) / Memtotal_;  // convert to percentage
  return (TotalUsed_);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  std::ifstream filestream(kProcDirectory + kUptimeFilename);  // /proc/uptime
  string line;
  long sys_uptime_;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::size_t pos = line.find(" ");
    std::string upTime_ = line.substr(0,pos);
    // std::string idleTime_ = line.substr(pos+1);
    sys_uptime_ = std::stol(upTime_);   // string to long
  }
  return (sys_uptime_); 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);  // /proc/stat
  string key;
  string value;
  string line;
  int total_processed_ = 0;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      while (filestream >> key >> value) {
        if (key == "processes") {
          total_processed_ = std::stoi(value);  // string to int
        }
      }
    }
  }
  return (total_processed_);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);  // /proc/stat
  string key;
  string value;
  string line;
  int running_processed_ = 0;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      while (filestream >> key >> value) {
        if (key == "procs_running") {
          running_processed_ = std::stoi(value);  // string to int
        }
      }
    }
  }
  return (running_processed_);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
