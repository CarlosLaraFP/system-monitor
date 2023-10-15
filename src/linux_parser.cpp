#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>

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

// Reads and returns the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  float memTotal, memFree;
  std::ifstream file {kProcDirectory + kMeminfoFilename};

  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (line.find("MemTotal:") != std::string::npos) {
        std::istringstream linestream(line);
        string temp;
        linestream >> temp >> memTotal;
      }
      else if (line.find("MemFree:") != std::string::npos) {
        std::istringstream linestream(line);
        string temp;
        linestream >> temp >> memFree;
      }
      else continue;
    }
  }

  return (memTotal - memFree) / memTotal; // Total used memory
}

// Reads and returns the system uptime
long LinuxParser::UpTime() { 
  string line;
  double upTime;
  std::ifstream file {kProcDirectory + kUptimeFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the System UpTime file.");
  }

  std::getline(file, line);
  std::istringstream linestream(line);
  linestream >> upTime;
  
  return static_cast<long>(std::round(upTime));
}

// Reads and returns the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line, cpu;
  long currentJiffies, totalJiffies = 0;
  std::ifstream file {kProcDirectory + kStatFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/stat file.");
  }

  std::getline(file, line);
  std::istringstream lineStream {line};
  lineStream >> cpu;
  while (lineStream >> currentJiffies) {
    totalJiffies += currentJiffies;
  }
  
  return totalJiffies;
}

// Reads and returns the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line, value;
  long utime, stime = 0;
  std::ifstream file {kProcDirectory + to_string(pid) + kStatFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/[pid]/stat file.");
  }

  std::getline(file, line);
  std::istringstream lineStream {line};

  // utime and stime are the 14th and 15th elements, respectively.
  for (int i = 1; i <= 14 && lineStream >> value; ++i) {
    if (i == 14) {
      utime = std::stol(value);
    }
  }
  // lineStream already at the 15th element
  lineStream >> stime;

  return utime + stime; // active jiffies for the process
}

// Reads and returns the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line, cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream file {kProcDirectory + kStatFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/stat file.");
  }

  std::getline(file, line);
  std::istringstream lineStream {line};
  lineStream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  
  return user + nice + system + irq + softirq + steal + guest + guest_nice; // active jiffies
}

// Reads and returns the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line, cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream file {kProcDirectory + kStatFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/stat file.");
  }

  std::getline(file, line);
  std::istringstream lineStream {line};
  lineStream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  
  return idle + iowait; // idle jiffies
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Reads and returns the total number of processes
int LinuxParser::TotalProcesses() { 
  int totalProcesses;
  string line;
  std::ifstream file {kProcDirectory + kStatFilename};
  
  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (line.find("processes") != std::string::npos) {
        std::istringstream lineStream {line};
        string temp;
        lineStream >> temp >> totalProcesses;
        break;
      }
    }
  }
  
  return totalProcesses;
}

// Reads and returns the number of running processes
int LinuxParser::RunningProcesses() { 
  int runningProcesses;
  string line;
  std::ifstream file {kProcDirectory + kStatFilename};
  
  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (line.find("procs_running") != std::string::npos) {
        std::istringstream lineStream {line};
        string temp;
        lineStream >> temp >> runningProcesses;
        break;
      }
    }
  }
  
  return runningProcesses;
}

// Reads and returns the command associated with a process
string LinuxParser::Command(int pid) { 
  string command, line;
  std::ifstream file {kProcDirectory + to_string(pid) + kCmdlineFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/[pid]/cmdline file.");
  }

  std::getline(file, line);

  return command;
}

// Reads and returns the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  long ram;
  std::ifstream file {kProcDirectory + to_string(pid) + kStatusFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/[pid]/status file.");
  }

  while (std::getline(file, line)) {
    if (line.find("VmSize:") != std::string::npos) {
      std::istringstream lineStream(line);
      string temp;
      lineStream >> temp >> ram;
      break;
    }
  }

  return to_string(0.001 * ram); // returning process memory as MB
}

// Reads and returns the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, uid;
  std::ifstream file {kProcDirectory + to_string(pid) + kStatusFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/[pid]/status file.");
  }

  while (std::getline(file, line)) {
    if (line.find("Uid:") != std::string::npos) {
      std::istringstream lineStream(line);
      string temp;
      lineStream >> temp >> uid;
      break;
    }
  }

  return uid;
}

// Reads and returns the user associated with a process
string LinuxParser::User(int pid) { 
  auto uid = LinuxParser::Uid(pid);
  string line, userName, userId;
  std::ifstream stream {LinuxParser::kPasswordPath};

  if (!stream.is_open()) {
    throw std::runtime_error("Failed to open Password file in LinuxParser::User()");
  }

  while (std::getline(stream, line)) {
    std::replace(line.begin(), line.end(), ':', ' '); 
    std::istringstream lineStream {line};
    string temp;
    lineStream >> userName >> temp >> userId;

    if (userId == uid) { break; }
  }

  return userName; 
}

// Reads and returns the uptime of a process
long LinuxParser::UpTime(int pid) { 
  long starttime;
  string line;
  std::ifstream file {LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open Process file in LinuxParser::UpTime()");
  }
  
  std::getline(file, line);
  std::istringstream lineStream {line};
  
  for (auto i = 1; i <= 22; ++i) {
    if (i == 22) {
      lineStream >> starttime;
    }
    else {
      string temp;
      lineStream >> temp;
    }
  }
  
  return LinuxParser::UpTime() - starttime / sysconf(_SC_CLK_TCK); // seconds
}
