/********************************
 FileName: executor/config.cpp
 Version:  0.1
 Description: config for executor
*********************************/

#include "gflags/gflags.h"

DEFINE_int32(port, 9997, "executor port");
DEFINE_string(resource_scheduler_endpoint, "127.0.0.1:9998", "ResouceScheduler endpoint");
DEFINE_string(jobs_manager_endpoint, "127.0.0.1:9999", "JobsManager endpoint");
DEFINE_int32(hb_interval, 15, "heartbeat interval");
DEFINE_int32(vm_hb_interval, 15, "vm heartbeat interval");
