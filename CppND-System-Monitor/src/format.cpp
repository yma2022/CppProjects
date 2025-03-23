#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long hours = seconds / 3600;
    long minutes = (seconds % 3600) / 60;
    long remaining_seconds = seconds % 60;
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%02ld:%02ld:%02ld", 
             hours, minutes, remaining_seconds);    
    return string(buffer);
}