#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Linux systems typically use 100 clock ticks per second
constexpr long int CLOCK_TICKS_PER_SEC = 100;

Process::Process(int pid) : pid_(pid) {
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
  ram_ = LinuxParser::Ram(pid);
  uptime_ = LinuxParser::UpTime(pid);
}

int Process::Pid() {
    return pid_;
}

float Process::CpuUtilization() const {
    long total_time = LinuxParser::ActiveJiffies(pid_) / CLOCK_TICKS_PER_SEC;
    long seconds = LinuxParser::UpTime(pid_);
    
    // Avoid division by zero
    if (seconds == 0) {
        return 0.0f;
    }
    
    return static_cast<float>(total_time) / static_cast<float>(seconds);
}

string Process::Command() {
    return command_;
}

string Process::Ram() {
    return ram_;
}

string Process::User() {
    return user_;
}

long int Process::UpTime() {
    return uptime_;
}

bool Process::operator<(Process const& a) const {
    return CpuUtilization() < a.CpuUtilization();
}

bool Process::operator>(Process const& a) const {
    return CpuUtilization() > a.CpuUtilization();
}