/********************************
 FileName: executor/register.cpp
 Version:  0.1
 Description: register
*********************************/
#include <stdlib.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <iostream>
#include <string>

#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "executor/executor_register.h"
#include "executor/system.h"

DECLARE_bool(debug);
DECLARE_string(resource_scheduler_endpoint);
DECLARE_string(lynn_version);
DECLARE_string(interface);


using log4cplus::Logger;
using namespace std;

static Logger logger = Logger::getInstance("executor");

bool ExecutorRegister::RegistMachine() {
