#include "format.h"

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
std::string Format::ElapsedTime(long seconds) {
    double hours, minutes, secs;
    std::string formatted;

    hours = seconds / 3600.0;
    minutes = fmod(hours, 1) * 60;
    secs = fmod(minutes, 1) * 60;

    std::string hour_string = (int)hours < 10 ? "0" + std::to_string((int)hours) : std::to_string((int)hours);
    std::string minutes_string = (int)minutes < 10 ? "0" + std::to_string((int)minutes) : std::to_string((int)minutes);
    std::string secs_string = (int)secs < 10 ? "0" + std::to_string((int)secs) : std::to_string((int)secs);
    
    formatted = hour_string + ":" + minutes_string + ":" + secs_string;
    return formatted;
}