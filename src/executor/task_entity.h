/********************************
 FileName: executor/task_entity.h
 Version:  0.1
 Description: task entity in executor, corresponds to task, corresponds to kvm/lxc
*********************************/

#ifndef SRC_EXECUTOR_TASKENTITY_H
#define SRC_EXECUTOR_TASKENTITY_H

#include <string>
#include <boost/shared_ptr.hpp>

// #include "include/proxy.h"
#include "include/type.h"
#include "common/clynn/rwlock.h"

using std::string;
using boost::shared_ptr;
using clynn::RWLock;

class TaskEntity {
public:
    explicit TaskEntity(const string& task_info);

    bool IsLegal();

    ~TaskEntity() {} 

    TaskID GetID() const {
        return m_id;
    }

    VMType::type GetVMType() const {
        return m_info.type;
    }

    TaskEntityState::type GetState();

    double GetPercentage();
   
    bool SetStates(const TaskEntityState::type state, const double percentage);

    bool SetState(const TaskEntityState::type state);
