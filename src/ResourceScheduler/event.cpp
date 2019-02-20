/********************************
 FileName: resourcescheduler/event.cpp


 Version:  0.1 
 Description: event, and its handler
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "ResourceScheduler/event.h"
#include "ResourceScheduler/resource_scheduler_engine.h"

using log4cplus::Logger;

static Logger logger = Logger::getInstance("ResourceScheduler");


/*machine event handle*/
bool MachineUpdateEvent::Handle() {
    string heartbeat_ad = GetHeartbeatAd();
    vector<string> task_list = GetTaskList();
