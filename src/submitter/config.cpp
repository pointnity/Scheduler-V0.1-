/********************************
 FileName: submitter/config.cpp
 Version:  0.1
 Description: config for submitter
*********************************/

#include "gflags/gflags.h"

DEFINE_string(JobsManager_endpoint, "127.0.0.1:9999", "JobsManager endpoint");
DEFINE_string(xml_path, "", "xml path");
DEFINE_int32(hdfs_port, 9000, "hdfs port");
