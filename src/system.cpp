#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  this->processes_.clear();
  vector<int> pid_vec_ = LinuxParser::Pids();
  for (auto pid_ : pid_vec_) {
    this->processes_.push_back(Process(pid_));
  }
  // Sort by process's attribute
  std::sort(processes_.begin(), processes_.end());

  return processes_;
}

std::string System::Kernel() { return (LinuxParser::Kernel()); }

float System::MemoryUtilization() { return (LinuxParser::MemoryUtilization()); }

std::string System::OperatingSystem() {
  return (LinuxParser::OperatingSystem());
}

int System::RunningProcesses() { return (LinuxParser::RunningProcesses()); }

int System::TotalProcesses() { return (LinuxParser::TotalProcesses()); }

long int System::UpTime() { return (LinuxParser::UpTime()); }