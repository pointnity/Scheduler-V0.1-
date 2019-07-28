// =============================================================================
// Version: 0.1
// Author: mavisluo 
// Revision: 17158 
// Description:test for zookeeper
// Id：zk_common_test.cpp
// =============================================================================

#include <gtest/gtest.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <gflags/gflags.h>
#include <map>
#include <list>
#include <string>
#include <vector>
#include "tborg/conf_manager/zk_common.h"
#include "tborg/include/get_hostip.h"
#include "common/config/cflags.hpp"
#include "tborg/collector/collector_conf.h"
#include "tborg/conf_manager/dynamic_configuration.h"
#include "tborg/conf_manager/conf_manager.h"
#include "common/system/concurrency/atomic/atomic.hpp"

/// ip:port of zk servers
DEFINE_string(zk_servers, "", "ip:port of zk servers");

std::string test_node = "/zk/xaec/tborg/" + FLAGS_cluster_name +
                        "/tns/mavisluo";

class MyEnvironment : public testing::Environment
{
    public:
        virtual void SetUp() {
            std::cout << "MyEnvironment SetUP" << std::endl;
            FLAGS_cluster_name = "xaec-comprehensive_test_zk_" + get_hostip();
            /// create test nodes
