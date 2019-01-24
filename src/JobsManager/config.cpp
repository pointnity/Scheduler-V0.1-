/********************************
 FileName: JobsManager/config.cpp


 Version:  0.1
 Description: config for JobsManager
*********************************/

#include "gflags/gflags.h"

DEFINE_int32(jobs_manager_port, 9997, "JobsManager rpc port");
DEFINE_string(resource_scheduler_endpoint, "127.0.0.1:9998", "resource_scheduler endpoint");
