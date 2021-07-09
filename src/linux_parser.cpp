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
  string key, value, line;
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
  filestream.close();

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
  stream.close();
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

// Return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);  // /proc/meminfo
  string key, value, line;
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
  filestream.close();

  TotalUsed_ = (Memtotal_ - MemFree_) / Memtotal_;  // convert to percentage
  return (TotalUsed_);
}

// Return the system uptime, time since system start in second
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);  // /proc/uptime
  string line;
  long sys_uptime_;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::size_t pos = line.find(" ");
    std::string upTime_ = line.substr(0, pos);
    sys_uptime_ = std::stol(upTime_);  // string to long
  }
  filestream.close();

  return (sys_uptime_);
}

// Return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long all_jiffies = 0;
  vector<string> stat_vec_ = CpuUtilization();
  for (string stat_ : stat_vec_) {
    long stat_l_ = std::stol(stat_);
    all_jiffies += stat_l_;
  }
  return (all_jiffies);
}

// Return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) +
                           kStatFilename);  // /proc/[pid]/stat
  string line;
  long utime, stime, cutime, cstime, starttime;

  std::size_t s_pos, n_pos, len_;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    n_pos = 0;
    for (int i = 0; i < 22; i++) {
      s_pos = n_pos + 1;
      n_pos = line.find(" ", s_pos);
      len_ = n_pos - s_pos;
      if (i == 13) utime = std::stol(line.substr(s_pos, len_));   // time spent in user code
      if (i == 14) stime = std::stol(line.substr(s_pos, len_));   // time spent in kernel code
      if (i == 15) cutime = std::stol(line.substr(s_pos, len_));  // children's time spent in user code
      if (i == 16) cstime = std::stol(line.substr(s_pos, len_));  // children's time spent in kernel code
      if (i == 21) utime = std::stol(line.substr(s_pos, len_));   // time when process start
    }
  }
  filestream.close();

  long active_time = utime + stime + cutime + cstime;
  return (active_time);
}

// Return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long idle_jiffies = IdleJiffies();
  long all_jiffies = Jiffies();
  long active_jiffies = all_jiffies - idle_jiffies;
  return (active_jiffies);
}

// Return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> stat_vec_ = CpuUtilization();
  return (std::stol(stat_vec_[3]));
}

// Return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);  // /proc/stat
  string line;
  vector<string> stat_vec_;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::size_t start_pos = 0;
    std::size_t end_pos = line.find(" ");
    start_pos = end_pos + 2;
    for (int i = 0; i < 10; i++) {
      end_pos = line.find(" ", start_pos);
      stat_vec_.push_back(line.substr(start_pos, (end_pos - start_pos)));
      start_pos = end_pos + 1;
    }
  }
  filestream.close();

  return (stat_vec_);
}

int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);  // /proc/stat
  string key, value, line;
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
  filestream.close();

  return (total_processed_);
}

int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);  // /proc/stat
  string key, value, line;
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
  filestream.close();

  return (running_processed_);
}

// Return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) +
                           kCmdlineFilename);  // /proc/[pid]/cmdline
  string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  filestream.close();
  return (line);
}

// Return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) +
                           kStatusFilename);  // /proc/[pid]/status
  string key;
  string value, string_kB;
  string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      line.erase(std::remove(line.begin(), line.end(), '\t'),
                 line.end());  // remove "\t"
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          std::replace(value.begin(), value.end(), '_', ' ');
          std::size_t start_pos_, end_pos_;
          start_pos_ = value.find_first_not_of(" ", 0);
          end_pos_ = value.find(" ", start_pos_);
          string ram_ =
              value.substr(start_pos_, (end_pos_ - start_pos_));  // unit kB
          long ram_MB_ = std::stol(ram_) / 1000;
          return (std::to_string(ram_MB_));
        }
      }
    }
  }
  filestream.close();
  return string();
}

// Return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) +
                           kStatusFilename);  // /proc/[pid]/status
  string key, value, line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), '\t', '_');
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          std::replace(value.begin(), value.end(), '_', ' ');
          std::size_t start_pos_, end_pos_;
          start_pos_ = value.find_first_not_of(" ", 0);
          end_pos_ = value.find(" ", start_pos_);
          string uid_ = value.substr(start_pos_, (end_pos_ - start_pos_));
          return (uid_);
        }
      }
    }
  }
  filestream.close();
  return (string());
}

// Return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);  // /etc/passwd
  string key, line;
  string value[7];
  // Get Uid of this process
  string uid = Uid(pid);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value[0] >> value[1] >> value[2] >> value[3] >>
             value[4] >> value[5] >> value[6]) {
        if (value[2] == uid) {
          string user_ = value[0];
          return (user_);
        }
      }
    }
  }
  return string();
}

// Return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) +
                           kStatFilename);  // /proc/[pid]/stat
  string line;
  std::size_t s_pos, n_pos, len_;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    n_pos = 0;
    for (int i = 0; i < 22; i++) {
      s_pos = n_pos + 1;
      n_pos = line.find(" ", s_pos);
      len_ = n_pos - s_pos;
    }
  }
  filestream.close();

  long process_start_time_ =
      std::stol(line.substr(s_pos, len_)) / sysconf(_SC_CLK_TCK);  // in sec.
  long system_uptime_ = UpTime();                                  // in sec
  long process_up_time_ = system_uptime_ - process_start_time_;
  return (process_up_time_);
}
