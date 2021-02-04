#include "processor.h"


// Return the aggregate CPU utilization
float Processor::Utilization() {
  std::string line, linestream, cpu;
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> cpu_label_ >> user_ >> nice_ >> system_ >> idle_ >> iowait_ >> 
          irq_ >> softirq_ >> steal_ >> guest_ >> guest_nice_) {
      Idle_ = idle_ + iowait_;
      NonIdle_ = user_ + nice_ + system_ + irq_ + softirq_ + steal_;
      Total_ = Idle_ + NonIdle_;
      CPU_Percentage_ = float(Total_ - Idle_) / float(Total_);
      return CPU_Percentage_;
    }
  }
  return CPU_Percentage_;
}