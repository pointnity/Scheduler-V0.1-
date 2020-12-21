/********************************
 FileName: common/clynn/cond.h


 Version:  0.1
 Description: pthread condition lock 
*********************************/

#ifndef COMMON_COND_H
#define COMMON_COND_H

#include <pthread.h>

#include <string>
#include "common/clynn/mutex.h"

using std::runtime_error;
using std::string;

namespace clynn {

    class Cond {
        public:
            Cond() {
                CheckError("Cond::Cond", pthread_cond_init(&m_cond, NULL));
            }

            ~Cond() { 
                pthread_cond_destroy(&m_cond);
            }

            /// send signal to wake up the thread
            void Signal(){
                CheckError("Cond::Signal", pthread_cond_signal(&m_cond));
