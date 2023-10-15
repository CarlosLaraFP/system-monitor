#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Returns the system's (aggregate) CPU
Processor& System::Cpu() { return this->cpu_; }

// Returns a container composed of the system's processes
vector<Process>& System::Processes() { 
  for (const auto& pid : LinuxParser::Pids()) {
    /*
      Using emplace_back() to its fullest potential by directly passing the pid argument to 
      the Process constructor within the vector, thus constructing the Process object in-place.
    */
    this->processes_.emplace_back(pid);
  }
  return this->processes_;
}

// Returns the system's kernel identifier (string)
string System::Kernel() { return LinuxParser::Kernel(); }

// Returns the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Returns the operating system name
string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Returns the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Returns the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Returns the number of seconds since the system started running
// Requires Format::ElapsedTime(long seconds) to be implemented
long System::UpTime() { return LinuxParser::UpTime(); }
