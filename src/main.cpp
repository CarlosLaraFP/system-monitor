#include "ncurses_display.h"
#include "system.h"

// Resources consulted include the links provided in the course description, as well as ChatGPT for quick syntax reminders and debugging.

int main() {
  System system;
  NCursesDisplay::Display(system);
}