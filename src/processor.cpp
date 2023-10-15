#include <stdexcept>
#include "processor.h"
#include "linux_parser.h"

// Returns the aggregate CPU utilization
float Processor::Utilization() {
  long user, nice, system, idle, iowait, irq, softirq, steal;
  std::string line;
  std::ifstream file {LinuxParser::kProcDirectory + LinuxParser::kStatFilename};
  
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the /proc/stat file for CPU utilization.");
  }
  
  while (std::getline(file, line)) {
    if (line.find("cpu") != std::string::npos) {
      std::istringstream lineStream {line};
      std::string temp;
      lineStream >> temp >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
      break;
    }
  }
  
  auto totalTime = user + nice + system + idle + iowait + irq + softirq + steal;
  auto totalDifference = totalTime - this->previousTotalTime;
  auto idleTime = idle + iowait;
  auto idleDifference = idleTime - this->previousIdleTime;

  // Update previous values for the next call
  this->previousTotalTime = totalTime;
  this->previousIdleTime = idleTime;

  return static_cast<float>(totalDifference - idleDifference) / totalDifference; // CPU utilization
}