#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <ctime>
#include <vector>
#include "HelperFunctions.hpp"
#include "Constants.hpp"

std::condition_variable worker_cond_var;

void worker_thread(int thread_number, int *local_thread_lock){
    std::mutex worker_mutex;
    std::unique_lock<std::mutex> thread_lck(worker_mutex);
    std::cout << "Data is ready." << std::endl << std::endl;
    int counter = 0;

    for(int i = 0; i < 10; i++){
        worker_cond_var.wait(thread_lck);
        //usleep(1000000);
        counter ++;
        std::cout << "Thread nr. " << thread_number << " counter is now " << counter << std::endl;

        (*local_thread_lock)++;
    }
}

int main(){
    struct timeval time_var;
    long curr_time, prev_time, time_delta, sleep_time = 0;
    int frequency = 5;

    int *main_lock = new int(0);

    std::vector<std::thread> thread_vector;
    thread_vector.push_back(std::thread(worker_thread, 1, main_lock));
    thread_vector.push_back(std::thread(worker_thread, 2, main_lock));
    thread_vector.push_back(std::thread(worker_thread, 3, main_lock));

    usleep(1000);
    curr_time = utils::get_time_microsec(time_var);
    prev_time = curr_time;

    while(true){
        prev_time = utils::get_time_microsec(time_var);
        std::cout << "Waiting for work." << std::endl;
        worker_cond_var.notify_all();

        while(*main_lock < (int)thread_vector.size()){

        }

        *main_lock = 0;

        curr_time = utils::get_time_microsec(time_var);

        time_delta = curr_time - prev_time;
        sleep_time = (COGNA::MICROSECOND_FACTOR / frequency) - time_delta;

        std::cout << std::endl;

        std::cout << "Sleep time = " << sleep_time << std::endl;
        if(sleep_time > 0){
            usleep(sleep_time);
        }
    }

    std::cout << "Work done." << std::endl;

    //t1.join();

    return 0;
}
