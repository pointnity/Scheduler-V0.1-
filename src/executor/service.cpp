/********************************
 FileName: executor/service.cpp
 Version:  0.1
 Description: executor service
*********************************/
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include <iostream>
#include "include/type.h"
#include "executor/task_entity_pool.h"
#include "executor/vm_pool.h"
#include "executor/dispatcher.h"
#include "executor/service.h"
#include "executor/image_manager.h"

using log4cplus::Logger;

static Logger logger = Logger::getInstance("executor");

DECLARE_bool(debug);

// test, hello world
int32_t ExecutorService::Helloworld() {
    printf("Hello world\n");
    return 0;
}

//RS
bool ExecutorService::Exit() {
    // new KillActionEvent
    EventPtr event(new ExitExecutorEvent());
    // Push event into Queue
    EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    return true;
}
// task
bool ExecutorService::StartTask(const string& info) {
    TaskPtr ptr(new TaskEntity(info));
    if (false == ptr->IsLegal()) {
        LOG4CPLUS_ERROR(logger, "Failed to initialize TaskEntity");
        return false;
    }
    if (false == TaskPoolI::Instance()->InsertIfAbsent(ptr)) {
        LOG4CPLUS_ERROR(logger, "The task had exist in the executor, job_id:" << ptr->GetID().job_id << ", task_id:" << ptr->GetID().task_id);
        TaskPoolI::Instance()->PrintAll();
        return false;
