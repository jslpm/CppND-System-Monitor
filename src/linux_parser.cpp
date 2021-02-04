#include <dirent.h>
#include <unistd.h>   // sysconf(_SC_CLK_TCK)
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read and return operating system
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

// Read and return system kernel
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Read and return a vector of process IDs (PIDs)
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::string line, linestream;
  std::string memType;
  int memValue, memTotal, memFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> memType >> memValue) {
      if (memType == "MemTotal:")
        memTotal = memValue;
      else if (memType == "MemFree:")
        memFree = memValue;
      }
    }
  }
  return float(memTotal - memFree) / float(memTotal);
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  std::string line, linestream;
  double uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> uptime) {
      return long(uptime);
    }
  }
  return 0;
}

// Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) {
  float cpu_utilization;
  int uptime = LinuxParser::UpTime();
  std::string line, linestream;
  int hertz = sysconf(_SC_CLK_TCK);
  int pid_stat;             // #1
  std::string comm;         // #2
  char state;               // #3
  int ppid, pgrp, session, tty_nr, tpgid;                             // #4, #5, #6, #7, #8
  unsigned int flags;       // #9
  long unsigned int minflt, cminflt, majflt, cmajflt, utime, stime;   // #10, #11, #12, #13, #14, #15
  long int cutime, cstime, priority, nice, num_threads, itrealvalue;  // #16, # 17, #18, #19, #20, #21
  long long int starttime;  // #22
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> pid_stat >> 
                          comm >> 
                          state >>
                          ppid >> pgrp >> session >> tty_nr >> tpgid >> 
                          flags >>
                          minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >>
                          cutime >> cstime >> priority >> nice >> num_threads >> itrealvalue >>
                          starttime) {
        long unsigned int total_time = utime + stime;
        total_time = total_time + cutime + cstime;
        float seconds = uptime - ( float(starttime) / float(hertz) );
        cpu_utilization = ( float(total_time) / float(hertz) ) / float(seconds);
        return cpu_utilization;
      }
    }
  }
  return 0.0;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line, linestream;
  std::string processes;
  int processesNumber;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> processes >> processesNumber) {
        if (processes == "processes")
          return processesNumber;
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line, linestream;
  std::string procs_running;
  int procs_running_number;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> procs_running >> procs_running_number) {
        if (procs_running == "procs_running")
          return procs_running_number;
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string line, linestream;
  std::string cmdline;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> cmdline) {
        return cmdline;
      }
    }
  }
  return std::string{};
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string line, linestream;
  std::string vmsize_name;
  long vmsize_value, vmsize_value_mb;
  float megabyte_factor = 0.001;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> vmsize_name >> vmsize_value) {
        if (vmsize_name == "VmSize:"){
          vmsize_value_mb = vmsize_value * megabyte_factor;
          return std::to_string(vmsize_value_mb);
        }
      }
    }
  }
  return std::string{};
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string line, linestream;
  std::string uid_name, uid_value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> uid_name >> uid_value) {
        if (uid_name == "Uid:")
          return uid_value;
      }
    }
  }
  return std::string{};
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  std::string uid_string = LinuxParser::Uid(pid);
  std::string line, linestream;
  std::ifstream stream(kPasswordPath);
  std::string user_name, x, uid_passwd;
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> user_name >> x >> uid_passwd) {
        if (uid_passwd == uid_string)
          return user_name;
      }
    }
  }
  return std::string{};
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line, linestream;
  double uptime;
  int pid_stat;             // #1
  std::string comm;         // #2
  char state;               // #3
  int ppid, pgrp, session, tty_nr, tpgid;                             // #4, #5, #6, #7, #8
  unsigned int flags;       // #9
  long unsigned int minflt, cminflt, majflt, cmajflt, utime, stime;   // #10, #11, #12, #13, #14, #15
  long int cutime, cstime, priority, nice, num_threads, itrealvalue;  // #16, # 17, #18, #19, #20, #21
  long long int starttime;  // #22
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> pid_stat >> 
                          comm >> 
                          state >>
                          ppid >> pgrp >> session >> tty_nr >> tpgid >> 
                          flags >>
                          minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >>
                          cutime >> cstime >> priority >> nice >> num_threads >> itrealvalue >>
                          starttime) {
        uptime = float(starttime) / float(sysconf(_SC_CLK_TCK));
        return long(uptime);
      }
    }
  }
  return 0;
}