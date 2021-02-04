#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  Process(int pid, std::string user, float cpu_utilization, std::string ram,
          long uptime, std::string cmdline) : 
          pid_(pid), user_(user), cpu_utilization_(cpu_utilization), ram_(ram),
          uptime_(uptime), cmdline_(cmdline) {};
  int Pid();                               // See src/process.cpp
  std::string User();                      // See src/process.cpp
  std::string Command();                   // See src/process.cpp
  float CpuUtilization();                  // See src/process.cpp
  std::string Ram();                       // See src/process.cpp
  long int UpTime();                       // See src/process.cpp
  bool operator<(Process const& a) const;  // See src/process.cpp

 private:
  int pid_;
  std::string user_;
  float cpu_utilization_;
  std::string ram_;
  long uptime_;
  std::string cmdline_;

};

#endif