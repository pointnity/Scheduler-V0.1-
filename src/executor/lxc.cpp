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
