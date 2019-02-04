#include "JobsManager/job.h"
#include "JobsManager/job_num.h"
#include "JobsManager/task.h"
#include "include/classad_attr.h"
#include "common/classad/classad_complement.h"

using clynn::ReadLocker;
using clynn::WriteLocker;

Job::Job() {
    m_next_task_id = 0;
    m_job_id = 0;
    m_raw_priority = JOB_RAW_PRIO_ORDINARY;
    m_total_running_time = 0; 
    m_total_waiting_time = 0;
}

void Job::SetWaitTime() {
    m_wait_time = time(NULL);
}

void Job::SetRunTime() {
    m_run_time = time(NULL);
}

void Job::UpdateTimeByState(const TaskState& state){
    if (state == TASK_RUNNING) {
        m_total_running_time += (time(NULL) - m_run_time);
    } else if (state == TASK_WAITING){
        m_total_waiting_time += (time(NULL) - m_wait_time);
    }
}

string Job::GetGroupName() {
    return m_group_name;
}

int32_t Job::GetRawPriority() {
    return m_raw_priority;
}

int32_t Job::GetSchedModel() {
    return m_sched_model;
}

int32_t Job::GetState() {
    return m_state;
}

void Job::SetState(int state) {
    m_state = state;
}

int32_t Job::GetJobId(){
    return m_job_id;
}

double Job::GetNeedCpu() {
    return m_need_cpu;
}

int32_t Job::GetNeedMemory() {
    return m_need_memory;
}

void Job::Init(const ClassAdPtr& classad_ptr, bool constraints_among_tasks) {
    WriteLocker locker(m_lock);
