#include "HelperFunctions.hpp"
#include "Constants.hpp"
#include <sys/time.h>

namespace utils{

long get_time_microsec(struct timeval time){
    gettimeofday(&time, NULL);
    return ((time.tv_sec * COGNA::MICROSECOND_FACTOR + time.tv_usec));
}

}; //namespace utils
