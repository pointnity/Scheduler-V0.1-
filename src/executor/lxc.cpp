/********************************
 FileName: executor/lxc.cpp
 Version:  0.1
 Description: lxc(Linux Container), inherit from vm
*********************************/

#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
//#include <sys/unistd.h>
//#include <dirent.h>
#include <fcntl.h> // for func open
//#include <time.h>
//#include <signal.h>

#include <boost/algorithm/string.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "lxc/lxc.h"
#include "lxc/conf.h"

#include "common/clynn/string_utility.h"
#include "common/lxc/confile.h"
#include "executor/hdfs_wrapper.h"
#include "include/proxy.h"
#include "executor/system.h"
#include "executor/lxc.h"
#include "executor/task_entity_pool.h"
#include "executor/resource_manager.h"

using std::cout;
using std::endl;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using boost::trim;
using log4cplus::Logger;
using clynn::WriteLocker;
using clynn::ReadLocker;

DECLARE_string(lxc_dir);
DECLARE_string(lxc_template);
DECLARE_bool(lxc_create);
DECLARE_bool(lxc_relative_path);

static Logger logger = Logger::getInstance("executor");

static const double DEFAULT_CPU_SHARE = 1.0;

string LXC::m_conf_template = "";

/// @brief: public function
// virtual function, from VM
// virtual CreateEnv, include ..
int32_t LXC::Execute() {
    if (CreateVM() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to create lxc, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }

    if (InstallApp() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to install app, lxc name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }

    if (LXC::StartApp() != 0){
        LOG4CPLUS_ERROR(logger, "Failed to start app, lxc name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;	
    }

    return 0;

}

int32_t LXC::CreateVM() {
    if (CreateLXC() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to create lxc, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }
    return 0;
}

// install app from hdfs
int32_t LXC::InstallApp() {
    if (!HDFSMgrI::Instance()->CopyToLocalFile(GetTaskInfo().app_info.app_src_path, m_dir)) {
        LOG4CPLUS_ERROR(logger, "Failed to install app, from hdfs:" << GetTaskInfo().app_info.app_src_path  << ", to local fs:" << m_dir);
        return -1;
    }
    return 0;
}

// execute the task, run the app
int32_t LXC::StartApp() {
    if (ExecuteLXC() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to execute lxc, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }
    return 0;
}

bool LXC::Stop() {
    // 
    string cmd = "lxc-stop -n " + GetName();
    int32_t ret = system(cmd.c_str());
    ret = ret >> 8;
    if (ret != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to stop lxc, name:" << GetName());
        return false;
    }
    return true;
}

bool LXC::Kill() {
    string cmd_stop = "lxc-stop -n " + GetName();
    int32_t ret_stop = system(cmd_stop.c_str());
    ret_stop = ret_stop >> 8;
    if (ret_stop != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to stop lxc, name:" << GetName());
        return false;
    }

    string cmd_destroy = "lxc-destroy -n " + GetName();
    int32_t ret_destroy = system(cmd_destroy.c_str());
    ret_destroy = ret_destroy >> 8;
    if (ret_destroy != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to destroy lxc, name:" << GetName());
        return false;
    }

    string cmd_rmr = "rm -rf " + m_dir;
    int32_t ret_rmr = system(cmd_rmr.c_str());
    ret_rmr = ret_rmr >> 8;
    if (ret_rmr != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to rmr work directory:" << m_dir << ", name:" << GetName());
        return false;
    }
    return true;
}
/*
VMState::type LXC::GetState() {
    // return VMState::VM_SERVICE_ONLINE;
    return m_hb_vm_info.state;
}
*/
HbVMInfo LXC::GetHbVMInfo() {
    // "empty" means NULL
    HbVMInfo empty;
    empty.id = GetID();
    empty.name = GetName();
    empty.type = GetVMType();
    empty.cpu_usage = 0;
    empty.memory_usage = 0;
    empty.bytes_in = 0;
    empty.bytes_out = 0;
    empty.app_state = AppState::APP_RUNNING;
    //empty.app_running = false;

    if (!SetProcesses()) {
        LOG4CPLUS_ERROR(logger, "Failed to set processes for lxc.");
        return empty;
    }

    static bool first = true;
    if (first) {
        m_hb_vm_info.id = GetID();
        m_hb_vm_info.name = GetName();
        m_hb_vm_info.type = GetVMType();
    }

    m_memory_used_in_kb = GetUsedMemory();
    m_hb_vm_info.memory_usage = (double)m_memory_used_in_kb / (GetTaskInfo().vm_info.memory * 1024);
    m_hb_vm_info.cpu_usage = GetCPUUsage();
    m_hb_vm_info.bytes_in = 0;
    m_hb_vm_info.bytes_out = 0;
    if (GetVeth() != "") {
        System::GetNetFlowinBytes(m_veth.c_str(), m_hb_vm_info.bytes_in, m_hb_vm_info.bytes_out);
    }
    m_hb_vm_info.app_state = AppState::APP_RUNNING;
    //m_hb_vm_info.app_running = true;

    return m_hb_vm_info;
}

void LXC::SetHbVMInfo(const string& hb_vm_info_ad) {
}

/// @brief: unique 
pid_t LXC::GetPid() {
    return m_pid;
}

string LXC::GetVeth() {
    return m_veth;
}

/// @brief: private function
// set name
void LXC::SetName() {
    // app_name + "_lxc_" + job_id + "_" + task_id
    TaskID id = GetID();
    stringstream ss_job, ss_task;
    ss_job << id.job_id;
    ss_task << id.task_id;
    string name = GetTaskInfo().app_info.name + "_lxc_" + ss_job.str() + "_" + ss_task.str();
    SetNameByString(name);
}

bool LXC::SetVeth() {
    // no ip, no veth
    if (GetTaskInfo().vm_info.ip == "") {
        m_veth = "";
        return true;
    }
    // set veth name
    TaskID id = GetID();
    stringstream ss_job, ss_task;
    ss_job << id.job_id;
    ss_task << id.task_id;
    m_veth = "veth_" + ss_job.str() + "_" + ss_task.str();
    if (strlen(m_veth.c_str()) > veth_max_length) {
        LOG4CPLUS_ERROR(logger, "veth name is too long.");
        return false;
    }
    return true;
}
// set name m_dir, mk work dir, copy lxc.conf, and change work director to m_dir
int32_t LXC::Init() {
    // set name, work dir
    SetName();
    m_dir = FLAGS_lxc_dir + "/" + GetName() + "/";
    m_conf_bak = m_dir + "lxc_bak.conf";
    m_conf_path = m_dir + "lxc.conf";

    if (!SetVeth()) {
        LOG4CPLUS_ERROR(logger, "Failed to set veth name.");
        return -1;
