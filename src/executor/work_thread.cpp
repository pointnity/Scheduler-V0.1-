/********************************
 FileName: executor/work_thread.h
 Version:  0.1
 Description: work thread
*********************************/

#include <stdlib.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "include/proxy.h"
#include "common/clynn/rpc.h"

#include "task_entity_pool.h"
#include "vm_pool.h"
#include "resource_manager.h"

using log4cplus::Logger;

DECLARE_string(resource_scheduler_endpoint);
DECLARE_int32(hb_interval);
DECLARE_bool(resource_scheduler_up);

static Logger logger = Logger::getInstance("executor");

// thread, start task
void* TaskProcessor(void* unused) {
    while (true) {
        // at one time, only start task that first found
        TaskPoolI::Instance()->StartTask();
        usleep(1000);
    }
    return NULL;
}

// thread, start vm
void* VMProcessor(void* unused) { 
    while (true) {
        // at one time, only start vm that first found
        VMPoolI::Instance()->StartVM();
        usleep(1000*100);
    }
    return NULL;
}

// thread, send heartbeat periodically
void* HeartbeatProcessor(void* unused) {
    //int32_t counter = 0;
    while(true) {
