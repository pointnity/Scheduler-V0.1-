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
    
    //update temporary alloc resource by task state
    if(m_machine_pool[id] != NULL){
	map<TaskID, AllocResource> TemproryAllocResourceMap;
        if(m_machine_pool[id]->GetTemproryAllocResourceMap(TemproryAllocResourceMap) == true) {
	    for(vector<string>::const_iterator it = task_list.begin(); it != task_list.end(); ++it) {
	        ClassAdPtr task_ad_ptr = ClassAdComplement::StringToAd(*it);
	        if(task_ad_ptr == NULL) {
		    continue;
	        }
		
    	        TaskID task_id;
	        if(task_ad_ptr->EvaluateAttrInt(ATTR_JOB_ID_RS, task_id.job_id) == false){
		    continue;
                } 
	        if(task_ad_ptr->EvaluateAttrInt(ATTR_TASK_ID_RS, task_id.task_id) == false) {
       		    continue;
    	        }

		map<TaskID, AllocResource>::iterator it_1 = TemproryAllocResourceMap.find(task_id);
		if(it_1 != TemproryAllocResourceMap.end()) {
		    TemproryAllocResourceMap.erase(it_1);
		}
	    }

	    //update new machine_ad
	    for(map<TaskID, AllocResource>::iterator it = TemproryAllocResourceMap.begin(); 
		it != TemproryAllocResourceMap.end(); ++it){
                if(machine_ptr->TempAllocResource(it->first, it->second) == false) {
                    continue;
                }
            }
        } 
    } 
       
    WriteLocker lock(m_machine_locks[id]);
    m_machine_pool[id] = machine_ptr;    
    UpdateMachineStamp(ip);
    return 0;
}


int ResourceSchedulerEngine::DeleteMachine(const string& ip) {
    int id;
    if(!GetIdByIp(id, ip)) {
        return MachineError::MACHINE_ERROR_NOT_FOUND;
    }
    WriteLocker lock(m_machine_locks[id]);
    //empty ptr
    m_machine_pool[id] = MachinePtr();
    return 0;
}


MultiD_Resource ResourceSchedulerEngine::GetTotalResource() {
     MultiD_Resource r;
     r.total_cpu = 0;
     r.total_memory = 0;
     r.total_disk = 0;
    
     ReadLocker lock(m_ip_to_id_map_lock); 
     for(map<string, int>::iterator it = m_ip_to_id_map.begin(); it != m_ip_to_id_map.end(); ++it) {
         int id = it->second;
	 ReadLocker lock(m_machine_locks[id]);
	 if(m_machine_pool[id] == NULL) {
	     continue;
	 }
         int32_t cpu = 0;
         m_machine_pool[id]->GetMachineAd()->EvaluateAttrNumber(ATTR_TotalCPUNum, cpu);
         r.total_cpu += cpu; 

         int32_t memory = 0;
         m_machine_pool[id]->GetMachineAd()->EvaluateAttrNumber(ATTR_TotalMemory, memory);
         r.total_memory += memory;

         int32_t disk = 0;
         m_machine_pool[id]->GetMachineAd()->EvaluateAttrNumber(ATTR_TotalDisk, disk);
         r.total_disk += disk;
     }
     return r;
}

void ResourceSchedulerEngine::MachineMonitor() {
    WriteLocker lock(ip_to_stamp_map_lock);
    if(ip_to_stamp_map.size() != 0) {
	//std::cout<< "map size:" << ip_to_stamp_map.size()<<std::endl;
        for(map<string, int32_t>::iterator it = ip_to_stamp_map.begin(); it!= ip_to_stamp_map.end(); ++it) {
	    if(time(NULL) - it->second > 10*FLAGS_hb_interval) {
	        // new MachineUpdateEvent
                EventPtr event(new MachineDeleteEvent(it->first));
               // Push event into Queue
                EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
		    
                LOG4CPLUS_INFO(logger, "Delete machine ip:" << it->first);
		ip_to_stamp_map.erase(it);
	    }
        }
    }
}


void ResourceSchedulerEngine::UpdateMachineStamp(string ip) {
    WriteLocker lock(ip_to_stamp_map_lock);
    map<string, int32_t>::iterator it = ip_to_stamp_map.find(ip);
    if(it != ip_to_stamp_map.end()) {
	it->second = time(NULL);
    } else {
	ip_to_stamp_map[ip] = time(NULL);
    }
}


int ResourceSchedulerEngine::InvokeHousekeeper() {
    time_t now;
    LOG4CPLUS_INFO(logger, "Begine to invoke house keeper.");

    time(&now);  /// get the cur_time
    if (now == (time_t)-1) {
        LOG4CPLUS_ERROR(logger, "Error in reading system time --- aborting");
        return -1;
    }

    CleanMachines(now);
    return 1;
}

int ResourceSchedulerEngine::CleanMachines(time_t now) {
    return 0;
}

void ResourceSchedulerEngine::HousekeeperTimer(int* count, uint64_t timer_id) {
    InvokeHousekeeper();
}

bool ResourceSchedulerEngine::GetIdByIp(int& id, const string& ip) {
    ReadLocker lock(m_ip_to_id_map_lock);
    map<string, int>::iterator it = m_ip_to_id_map.find(ip);
    if(it == m_ip_to_id_map.end()) {
        return false;
    }
    id = it->second;
    //LOG4CPLUS_INFO(logger, "GetIdByIp " << id << " " << ip);
    return true;
}

bool ResourceSchedulerEngine::MapIpToId(int& id, const string& ip) {
    WriteLocker locker(m_ip_to_id_map_lock);
    map<string, int>::iterator it = m_ip_to_id_map.find(ip);

    //for some special cases
    if(it != m_ip_to_id_map.end()) {
        id = it->second;
        return true;
    }

    if(m_cur_id == MACHINE_NUM) {
        return false;
    }

    id = m_cur_id++;
    m_ip_to_id_map[ip] = id;

    LOG4CPLUS_INFO(logger, "MapIdByIp " << id << " " << ip);
    return true;
}


//Todo
int ResourceSchedulerEngine::FetchMachinesForNegotiator(const ClassAdPtr job, vector<ClassAd>& results){
    return 0;
}


//Argument:
//	@task: Poninter of the task to be matched 
//	@machine: classad of the machine to be matched
//Return:
//	bool  
bool ResourceSchedulerEngine::FetchMachinesForNegotiatorTask(const ClassAdPtr task, string& machine_endpoint){
    std::cout<<"yes"<<"\n"<<std::endl;
    int id;
    int match_machine_id;
    bool match_flag = false;
    string machine;
    string user;
