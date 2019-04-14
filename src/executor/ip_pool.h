/********************************
 FileName: executor/ip_pool.h
 Version:  0.1
 Description: the pool of ip
*********************************/

#ifndef SRC_EXECUTOR_IP_POOL_H
#define SRC_EXECUTOR_IP_POOL_H

#include <map>

#include "common/clynn/singleton.h"
#include "common/clynn/rwlock.h"

using std::string;
using std::map;
using clynn::RWLock;

class IPPool {
public:

    bool Init();

    // TODO print all
    void PrintAll();

    // get a avail ip
    string GetAvailIp();

    // release a ip
    bool ReleaseIp(const string& ip);

private:
    int32_t iptoint(const char *ip );
  
    string inttoip(int ip_num);

    bool IsIPAddress(const char *s, int32_t& sub4);

private:
