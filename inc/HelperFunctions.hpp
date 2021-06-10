#ifndef INCLUDE_HELPERFUNCTIONS_HPP
#define INCLUDE_HELPERFUNCTIONS_HPP

#include <ctime>

namespace utils{

/**
 * @brief Returns the current time in microseconds since epoch.
 *
 * This function is not in use yet. Maybe will be used for steps per second control.
 *
 * @param time    A certain struct receiving the current time.
 *
 * @return        The current time in microseconds since epoch.
 *
 */
long get_time_microsec(struct timeval time);

} //namespace utils

#endif //INCLUDE_HELPERFUNCTIONS_HPP
