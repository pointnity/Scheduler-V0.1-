/********************************
 FileName: executor/config.cpp
 Version:  0.1
 Description: config for executor
*********************************/

#include "gflags/gflags.h"

DEFINE_int32(port, 9997, "executor port");
DEFINE_string(resource_scheduler_endpoint, "127.0.0.1:9998", "ResouceScheduler endpoint");
DEFINE_string(jobs_manager_endpoint, "127.0.0.1:9999", "JobsManager endpoint");
