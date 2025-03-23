#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <ctime>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float total_memory = 0.0;
  float free_memory = 0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "MemTotal:") {
          linestream >> total_memory;
        }
        if (key == "MemFree:") {
          linestream >> free_memory;
        }
      }
    }
  }
  return (total_memory - free_memory) / total_memory;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime = 0;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  vector<string> values;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  if (values.size() >= 17) {
    return stol(values[13]) + stol(values[14]) + stol(values[15]) + stol(values[16]);
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_values = CpuUtilization();
  if (cpu_values.size() >= 8) {
    return stol(cpu_values[kUser_]) + stol(cpu_values[kNice_]) +
           stol(cpu_values[kSystem_]) + stol(cpu_values[kIRQ_]) +
           stol(cpu_values[kSoftIRQ_]) + stol(cpu_values[kSteal_]) +
           stol(cpu_values[kGuest_]) + stol(cpu_values[kGuestNice_]);
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_values = CpuUtilization();
  if (cpu_values.size() >= 5) {
    return stol(cpu_values[kIdle_]) + stol(cpu_values[kIOwait_]);
  }
  return 0;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string value, key;
  vector<string> cpu_values;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key != "cpu") {
        cpu_values.push_back(key);
      }
    }
  }
  return cpu_values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "processes") {
          linestream >> value;
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "procs_running") {
          linestream >> value;
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> command;
    return command;
  }
  return string();
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "VmSize:") {
          linestream >> value;
          return to_string(stoi(value) / 1024);
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "Uid:") {
          linestream >> value;
          return value;
        }
      }
    }
  }
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line;
  string name, x, uid_str;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> x >> uid_str) {
        if (uid_str == uid) {
          return name;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  vector<string> values;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  if (values.size() >= 22) {
    long starttime = stol(values[21]);
    return UpTime() - (starttime / sysconf(_SC_CLK_TCK));
  }
  return 0;
}