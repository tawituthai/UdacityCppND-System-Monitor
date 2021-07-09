#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();
  void Pid(int id) { this->pid_ = id; };
  std::string User();
  void User(std::string u) { this->user_ = u; };
  std::string Command();
  void Command(std::string c) { this->cmd_ = c; };
  float CpuUtilization();
  void CpuUtilization(float cpu_) { this->cpu_util_ = cpu_; };
  std::string Ram();
  void Ram(std::string r) { this->ram_ = r; };
  long int UpTime();
  void UpTime(long int ut) { this->uptime_ = ut; };
  bool operator<(Process const& a) const;

  // Declare any necessary private members
 private:
  int pid_;
  std::string user_;
  std::string cmd_;
  std::string ram_;
  long int uptime_;
  float cpu_util_;
};

#endif