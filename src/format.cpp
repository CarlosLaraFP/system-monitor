#include <string>
#include <iomanip>
#include <sstream>

#include "format.h"

using std::string;
using std::ostringstream;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  long remainingSeconds = seconds % 3600;
  long minutes = remainingSeconds / 60;
  long secs = remainingSeconds % 60;

  ostringstream timeStream;
  /*
    In the first line below, if hours is a single-digit number, std::setw(2) will be format it to occupy two characters.
    If the next item to be streamed into timeStream doesn't fill the entire width set by setw, the remaining space will be filled with '0'. This is how we get hours like 01, 02, 03, ..., when the actual number is a single digit.
    We do this for each variable, given OUTPUT: HH:MM:SS
  */
  timeStream << std::setw(2) << std::setfill('0') << hours << ":"
    << std::setw(2) << std::setfill('0') << minutes << ":"
    << std::setw(2) << std::setfill('0') << secs;

  return timeStream.str();
}