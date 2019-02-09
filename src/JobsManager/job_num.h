#ifndef JOBS_MANAGER_JOB_NUM_H
#define JOBS_MANAGER_JOB_NUM_H

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5) 
#include <atomic>  // for ubuntu
#else
#include <cstdatomic> // for centos
#endif

using std::atomic_int;

class JobNum {
    public:
        JobNum(): m_job_id(0) {}
        //The default is inline's
        int GetNewJobId() {
            return m_job_id++;
        }
    private:
        atomic_int m_job_id;
};
