/********************************
 FileName: executor_monitor/checker.cpp
 Version:  0.1
 Description: checker
*********************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <iostream>
#include <fstream>
#include <string>

#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "executor_monitor/checker.h"
#include "executor_monitor/hdfs_wrapper.h"

DECLARE_string(resource_scheduler_endpoint);
DECLARE_string(lynn_version);
DECLARE_string(executor_hdfs_path);


using log4cplus::Logger;
using namespace std;

static Logger logger = Logger::getInstance("executor_monitor");

bool Checker::CheckVersion() {
    // check executor version
    string new_lynn_version;
    //update executor version in executor.conf
    char cur_dir[100];
    if (getcwd(cur_dir, sizeof(cur_dir)-1) == NULL) {
         LOG4CPLUS_ERROR(logger, "Failed to get current dir.");
         return false;
    }
    string executor_dir(cur_dir);

    try {
        Proxy<ResourceSchedulerClient> proxy = RpcClient<ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
        proxy().GetCurVersion(new_lynn_version);
       	if(new_lynn_version !=FLAGS_lynn_version) {
             LOG4CPLUS_ERROR(logger, "executor version is error, old_version:" << FLAGS_lynn_version << "  new_version:" << new_lynn_version);
