/********************************
 FileName: executor/hdfs_wrapper.cpp
 Version:  0.1
 Description: hdfs wrapper
*********************************/

#include "submitter/hdfs_wrapper.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

using log4cplus::Logger;

static Logger logger = Logger::getInstance("submitter");

DECLARE_string(hdfs_host);
DECLARE_int32(hdfs_port);


bool HDFSWrapper::Init() {
    m_local_fs = hdfsConnect(NULL, 0);
    if (NULL == m_local_fs) {
        LOG4CPLUS_ERROR(logger, "Failed to connect local fs");
        return false;
    }
    m_h_fs = hdfsConnect(FLAGS_hdfs_host.c_str(), FLAGS_hdfs_port);
    if (NULL == m_h_fs) {
        LOG4CPLUS_ERROR(logger, "Failed to connect hdfs, hdfs_host:" << FLAGS_hdfs_host << ", hdfs_port:" << FLAGS_hdfs_port);
        return false;
    }

    // test
    /*string local_path = "/tmp/";
    local_path += "output";
    string h_path = "/user/wm/output";
    if (!CopyToLocalFile(h_path, local_path)) {
        LOG4CPLUS_ERROR(logger, "Failed to copy file");
        return false;
    } 
    string local_path2 = "/home/wm/b.cpp";
    string h_path2 = "/user/root/b.cpp";
    if (!CopyFromLocalFile(local_path2, h_path2)) {
        LOG4CPLUS_ERROR(logger, "Failed to copy file");
        return false;
