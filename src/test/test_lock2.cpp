#include <stdio.h>
#include <zookeeper/zookeeper.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "common/clynn/rwlock.h"
#include "include/proxy.h"
#include "common/clynn/rpc.h"

using std::string;
using std::map;
using boost::shared_ptr;
using boost::thread;
using clynn::RWLock;
using clynn::ReadLocker;
using clynn::WriteLocker;

map<int, RWLock> map1;

void Thread1() {
    map<int, RWLock>::iterator it = map1.find(1);
    WriteLocker locker(it->second);
    printf("111111\n");
