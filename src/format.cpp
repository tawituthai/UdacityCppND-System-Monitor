#include "format.h"

#include <iomanip>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours_, minutes_, seconds_;
  std::string result;
  std::stringstream ss;

  hours_ = int(seconds / 3600.0);
  minutes_ = int((seconds - (hours_ * 3600.0)) / 60.0);
  seconds_ = int(seconds - (hours_ * 3600.0) - (minutes_ * 60.0));

  ss << std::setw(2) << std::setfill('0') << hours_ << ":"
     << std::setw(2) << std::setfill('0') << minutes_ << ":" 
     << std::setw(2) << std::setfill('0') << seconds_;

  result = ss.str();
  return (result);
}