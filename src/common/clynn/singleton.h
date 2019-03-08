/********************************
 FileName: common/clynn/singleton.h


 Version:  0.1
 Description: singleton, single case 
*********************************/

#ifndef COMMON_SINGLETON_H
#define COMMON_SINGLETON_H

#include "common/clynn/mutex.h"

using clynn::MutexLocker;

template <typename T>
class Singleton {
public:
    static T* Instance() {
        MutexLocker locker(m_lock);
        if (m_instance == NULL)
            m_instance = new T;
        return m_instance;
    }

private:
    static clynn::Mutex m_lock;
