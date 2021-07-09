#include "processor.h"

#include <iostream>

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  float active_ = (float)LinuxParser::ActiveJiffies();
  float all_ = (float)LinuxParser::Jiffies();
  float utilization_ = (active_/all_)*1.0f;
  return (utilization_);
}