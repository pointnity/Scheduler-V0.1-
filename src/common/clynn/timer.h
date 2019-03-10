#ifndef COMMON_CLYNN_TIMER_H
#define COMMON_CLYNN_TIMER_H

#include <unistd.h>
#include <sys/time.h>

namespace clynn {

    class Timer {
    public:
       explicit Timer(bool auto_start = true) {
           m_is_running = auto_start; 
           m_cumulated_time = 0; 
           m_start_time.tv_sec = 0;
           m_start_time.tv_usec = 0;
           if(auto_start) {
               gettimeofday(&m_start_time, NULL); 
           }
       } 
  
       void Start() {
           if(m_is_running == false) { 
               m_is_running = true;
               gettimeofday(&m_start_time, NULL);
           } 
       }
  
       void Stop() {
           if(m_is_running == true) {
               m_is_running = false;
               struct timeval now;
               gettimeofday(&now, NULL); 
               m_cumulated_time += 
               1000000*(now.tv_sec - m_start_time.tv_sec) + now.tv_usec - m_start_time.tv_usec;
