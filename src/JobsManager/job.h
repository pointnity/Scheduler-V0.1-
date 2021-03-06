#ifndef JOBS_MANAGER_JOB_H
#define JOBS_MANAGER_JOB_H

#include <string>
#include <list>
#include <vector>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5) 
#include <atomic>  // for ubuntu
#else
#include <cstdatomic> // for centos
#endif

#include <boost/shared_ptr.hpp>
#include "common/classad/classad_complement.h"
#include "common/clynn/block_queue.h"
#include "common/clynn/singleton.h"
#include "common/clynn/rwlock.h"
#include "JobsManager/task.h"
#include "JobsManager/job_enum.h"
#include "JobsManager/constraint_edge.h"

using std::string;
using std::list;
using std::vector;
using std::map;
using boost::shared_ptr;
using clynn::BlockQueue;
using clynn::RWLock;
using std::atomic_int;

class Job {
public:
    Job();
    void Init(const ClassAdPtr& classad, bool constraints_among_tasks = false);
    void InitTasks(const list<TaskAdPtr>& task_ad_list);
    void InitTasks(const list<TaskAdPtr>& task_ad_list, const list<EdgePtr>& edge_list);
    void SetWaitTime();
    void SetRunTime();
    void UpdateTimeByState(const TaskState& state);
    virtual TaskPtr NewTask() const = 0;
    virtual bool HaveWaitingTasks() = 0;    
    virtual bool IsScheduling() = 0;
    virtual bool IsRunning() = 0;
    string GetGroupName();
    int32_t GetRawPriority();
    int32_t GetSchedModel();
    int32_t GetState(); 
    void SetState(int32_t state);
    int32_t GetJobId();
    double GetNeedCpu();
    int32_t GetNeedMemory(); 
    TaskPtr GetTaskPtr(int32_t task_id);
    int32_t GetRunTaskNum();
    int32_t GetTaskNum();
    void CheckTasks();
    void UpdateState();

    //is a similar mechanism implemented?
    JobQueueNum GetJobQueueNum();

    list<TaskPtr> GetTaskList();
  
    bool GetConstraintsAmongTasks();

    void GetTaskStateInfo(vector<TaskStateInfo>& task_state_info_list);
    // test & TODO
    void PrintAllTasks();

    // constraints among tasks
    string ReadMap(const int32_t task_id);
    list<string> ReadMapAndGetSoftList();
    list<string> ReadMapAndGetSoftList(int32_t task_id);
    void WriteMap(const int32_t task_id, const string& ip);

protected:
    atomic_int m_next_task_id;
    atomic_int m_state;
    ClassAdPtr m_job_classad_ptr;
    list<TaskPtr> m_task_list;
    time_t m_submit_time;
    time_t m_wait_time;
    time_t m_run_time;
    time_t m_total_running_time;
    time_t m_total_waiting_time;
    int m_job_id;
    int m_task_num;
    // int m_machine_num;
    string m_group_name;
    int32_t m_sched_model;
    int32_t m_raw_priority;
    double m_need_cpu;
    int32_t m_need_memory;
    RWLock m_lock;
    JobQueueNum m_job_queue_num;
   
    // constraints among tasks
    bool m_constraints_among_tasks;
    // <task_id, ip>   
    map<int32_t, string> m_id_to_ip_map;
    RWLock m_map_lock;
};

typedef shared_ptr<Job> JobPtr;
// Because it is a singleton pattern, the following two variables are in fact a blocking queue, although the names are different.
// HistoryJobList The new implementation is written in Job_map.H, becomes a member variable
typedef Singleton<BlockQueue<JobPtr> > NewJobList;
// BlockQueue<JobPtr> NewJobList;Wrong return multi definition of ' newjoblist '
// typedef Singleton<BlockQueue<JobPtr> > HistoryJobList;

#endif
