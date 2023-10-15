#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Returns this process' ID
int Process::Pid() const { return this->pid_; }

// Returns this process' CPU utilization
float Process::CpuUtilization() const { 
  long utime, stime, cutime, cstime, starttime;
  long uptime = LinuxParser::UpTime();
  auto hertz = sysconf(_SC_CLK_TCK);
  string line;
  std::ifstream file {LinuxParser::kProcDirectory + to_string(Pid()) + LinuxParser::kStatFilename};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open Process file in Process::CpuUtilization()");
  }
  
  std::getline(file, line);
  std::istringstream lineStream {line};
  
  for (auto i = 1; i <= 22; ++i) {
    switch (i) {
      case 14:
        lineStream >> utime;
        break;
      case 15:
        lineStream >> stime;
        break;
      case 16:
        lineStream >> cutime;
        break;
      case 17:
        lineStream >> cstime;
        break;
      case 22:
        lineStream >> starttime;
        break;
      default:
        string temp;
        lineStream >> temp;
    }
  }

  // Total time spent for the process, including the time from children processes.
  auto totalTime = utime + stime + cutime + cstime;
  
  auto elapsedSeconds = uptime - (starttime / hertz);

  return 100.0 * totalTime / hertz / elapsedSeconds;
}

// Returns the command that generated this process
string Process::Command() const {
  string command;
  std::ifstream file {LinuxParser::kProcDirectory + to_string(Pid()) + LinuxParser::kCmdlineFilename};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open Process file in Process::Command()");
  }

  std::getline(file, command);
  
  return command;
}

// Returns this process's memory utilization
string Process::Ram() const { 
  long kb;
  string line;
  std::ifstream file {LinuxParser::kProcDirectory + to_string(Pid()) + LinuxParser::kStatusFilename};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open Process file in Process::Ram()");
  }

  while (std::getline(file, line)) {
    if (line.find("VmSize:") != std::string::npos) {
      std::istringstream lineStream {line};
      string temp;
      lineStream >> temp >> kb;
      break;
    }
  }

  return to_string(static_cast<int>(std::round(kb * 0.001)));
}

// Returns the user (name) that generated this process
string Process::User() const { 
  string uid, userId, userName;
  string line;
  std::ifstream file {LinuxParser::kProcDirectory + to_string(Pid()) + LinuxParser::kStatusFilename};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open Process file in Process::User()");
  }

  while (std::getline(file, line)) {
    if (line.find("Uid:") != std::string::npos) {
      std::istringstream lineStream {line};
      string temp;
      lineStream >> temp >> uid;
      break;
    }
  }

  std::ifstream stream {LinuxParser::kPasswordPath};

  if (!stream.is_open()) {
    throw std::runtime_error("Failed to open Password file in Process::User()");
  }

  while (std::getline(stream, line)) {

    std::replace(line.begin(), line.end(), ':', ' '); 
    std::istringstream lineStream {line};
    string temp;
    lineStream >> userName >> temp >> userId;

    if (userId == uid) { return userName; }
  }

  return string(); 
}

// Returns the age of this process (in seconds)
long int Process::UpTime() const { 
  long starttime;
  string line;
  std::ifstream file {LinuxParser::kProcDirectory + to_string(Pid()) + LinuxParser::kStatFilename};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open Process file in Process::UpTime()");
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

// Overloading the "less than" comparison operator for Process objects
bool Process::operator<(const Process& process) const { 
  return process.Pid() < this->pid_;
}