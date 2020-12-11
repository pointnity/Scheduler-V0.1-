/********************************
 FileName: ResourceScheduler/resource_scheduler_engine.cpp
 
 
 Version:  0.1
 Description: resource scheduler engine
*********************************/

#include <sys/time.h>

#include <tr1/functional>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <algorithm>
#include <gflags/gflags.h>

#include <iostream>

#include "common/clynn/rpc.h"
#include "ResourceScheduler/resource_scheduler_engine.h"
#include "include/classad_attr.h"
#include "include/type.h"
#include "proxy/ResourceScheduler/gen-cpp/ResourceScheduler.h"
#include "ResourceScheduler/dispatcher.h"


using log4cplus::Logger;
using std::tr1::function;
using std::tr1::placeholders::_1; 

DEFINE_int32(hb_interval, 5, "");
DEFINE_string(lynn_version, "", "");

static Logger logger = Logger::getInstance("ResourceScheduler");

using clynn::ReadLocker;
using clynn::WriteLocker;

ResourceSchedulerEngine::ResourceSchedulerEngine() {
}

ResourceSchedulerEngine::~ResourceSchedulerEngine() {
} 

int ResourceSchedulerEngine::Init() {
    m_cur_id = 0;
    return 0;
}

void ResourceSchedulerEngine::GetCurVersion(string& new_version) {
    new_version = FLAGS_lynn_version;
    return;
}

int ResourceSchedulerEngine::RegistMachine(const string& ip, const string& version) {
    if(version != FLAGS_lynn_version) {
	return -1;
    }
    //regist ip to map
    int id;
    if(!GetIdByIp(id, ip)) {
        if(!MapIpToId(id, ip)){
            LOG4CPLUS_ERROR(logger, "The number of Machine is full");
            return MachineError::MACHINE_ERROR_NUM_LIMIT;
        }
    }
    return id;
}

int ResourceSchedulerEngine::UpdateMachine(const string& machine_ad) {
    ClassAdPtr machine_ad_ptr = ClassAdComplement::StringToAd(machine_ad); 

    if(machine_ad_ptr == NULL) {
        return MachineError::MACHINE_ERROR_CLASSAD_SYNTAX;
    }

    MachinePtr machine_ptr(new Machine(machine_ad_ptr));
    if(!machine_ptr->ParseMachineAd()) {
        return MachineError::MACHINE_ERROR_ATTR;
    }

    int id;

    string ip = machine_ptr->GetIp();
    if(!GetIdByIp(id, ip)) {
        if(!MapIpToId(id, ip)){
            return MachineError::MACHINE_ERROR_NUM_LIMIT;
        }
    }

    WriteLocker lock(m_machine_locks[id]);
    m_machine_pool[id] = machine_ptr;
    return 0;
}

int ResourceSchedulerEngine::NewUpdateMachine(const string& machine_ad, const vector<string>& task_list) {
    //string to classad ptr
    ClassAdPtr machine_ad_ptr = ClassAdComplement::StringToAd(machine_ad);
    
    //check classad 
    if(machine_ad_ptr == NULL) {
	LOG4CPLUS_ERROR(logger, "A machine_ad is null:" << machine_ad);
        return MachineError::MACHINE_ERROR_CLASSAD_SYNTAX;
    }

    //new a machine object and parse it
    MachinePtr machine_ptr(new Machine(machine_ad_ptr));
    if(!machine_ptr->ParseMachineAd()) {
        LOG4CPLUS_ERROR(logger, "Failed to parse a machine_ad");
        return MachineError::MACHINE_ERROR_ATTR;
    }

    //create a map from ip to id 
    int id;

    string ip = machine_ptr->GetIp();
    if(!GetIdByIp(id, ip)) {
        string endpoint = machine_ptr->GetMachineEndpoint();
	try {
           Proxy<ExecutorClient> proxy = RpcClient<ExecutorClient>::GetProxy(endpoint);
           proxy().Exit();
           } catch (TException &tx) {
                LOG4CPLUS_ERROR(logger, "executor exit error:" << endpoint);
           }
	return MachineError::MACHINE_ERROR_NOT_FOUND;     
    }
    	        ClassAdPtr task_ad_ptr = ClassAdComplement::StringToAd(*it);

    //update temporary alloc resource by task state
    if(m_machine_pool[id] != NULL){
	map<TaskID, AllocResource> TemproryAllocResourceMap;
        if(m_machine_pool[id]->GetTemproryAllocResourceMap(TemproryAllocResourceMap) == true) {
	    for(vector<string>::const_iterator it = task_list.begin(); it != task_list.end(); ++it) {
	        if(task_ad_ptr == NULL) {
		    continue;
	        }
		
    	        TaskID task_id;
	        if(task_ad_ptr->EvaluateAttrInt(ATTR_JOB_ID_RS, task_id.job_id) == false){
