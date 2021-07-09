#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours_, minutes_, seconds_;
  string result;
  hours_ = int(seconds / 3600.0);
  minutes_ = int((seconds - (hours_ * 3600.0)) / 60.0);
  seconds_ = int(seconds - (hours_ * 3600.0) - (minutes_ * 60.0));
  result = std::to_string(hours_) + ":" + std::to_string(minutes_) + ":" +
           std::to_string(seconds_);
  return (result);
}