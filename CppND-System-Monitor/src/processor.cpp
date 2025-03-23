#include "processor.h"
#include <fstream>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::string;
using std::vector;

float Processor::Utilization() {
  vector<string> cpu_values = LinuxParser::CpuUtilization();
  
  if (cpu_values.size() >= 10) {
    long user = stol(cpu_values[LinuxParser::kUser_]);
    long nice = stol(cpu_values[LinuxParser::kNice_]);
    long system = stol(cpu_values[LinuxParser::kSystem_]);
    long idle = stol(cpu_values[LinuxParser::kIdle_]);
    long iowait = stol(cpu_values[LinuxParser::kIOwait_]);
    long irq = stol(cpu_values[LinuxParser::kIRQ_]);
    long softirq = stol(cpu_values[LinuxParser::kSoftIRQ_]);
    long steal = stol(cpu_values[LinuxParser::kSteal_]);
    long guest = stol(cpu_values[LinuxParser::kGuest_]);
    long guest_nice = stol(cpu_values[LinuxParser::kGuestNice_]);

    long total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    long total_idle = idle + iowait;

    long delta_total = total - prev_total_;
    long delta_idle = total_idle - prev_idle_;

    prev_total_ = total;
    prev_idle_ = total_idle;
    prev_user_ = user;
    prev_nice_ = nice;
    prev_system_ = system;
    prev_irq_ = irq;
    prev_softirq_ = softirq;
    prev_steal_ = steal;
    prev_guest_ = guest;
    prev_guest_nice_ = guest_nice;

    if (delta_total == 0) return 0.0;
    return (float)(delta_total - delta_idle) / (float)delta_total;
  }
  return 0.0;
}