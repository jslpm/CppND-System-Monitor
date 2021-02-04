#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <fstream>
#include <sstream>

#include "linux_parser.h"

class Processor {
  public:
    float Utilization();  // See src/processor.cpp

  private:
    std::string cpu_label_;
    long user_, nice_, system_, idle_, iowait_, irq_, softirq_, steal_, guest_, guest_nice_;
    long Idle_, NonIdle_, Total_;
    float CPU_Percentage_;
};

#endif