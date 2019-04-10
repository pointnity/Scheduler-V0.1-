/********************************
 FileName: executor/handler.cpp
 Version:  0.1
 Description: handler for event
*********************************/

#include "executor/handler.h"

using std::tr1::bind;

Handler::~Handler() {
    delete m_work_thread;
}

// start the thread
void Handler::Start() {
    Thread::ThreadFunc func = bind(&Handler::Handle, this);
    m_work_thread = new Thread(func);
    m_work_thread->Start();
}

void Handler::Handle() {
    while (true) {
