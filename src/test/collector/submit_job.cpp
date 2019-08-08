/********************************
 FileName: test/collector/negotiater.cpp
 Version:  0.1
 Description: match a job  (with xml file)
*********************************/

#include <iostream>
#include <string>
#include <string.h>

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include "common/clynn/rpc.h"
#include "include/proxy.h"
#include "include/classad_attr.h"

using namespace std;

static string usage = "./submit-job job_type task_num";

int main(int argc, char ** argv) {
    if(argc != 3) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }

    // job_type
    string job_type = argv[1];

    // task_num 
    int32_t task_num = atoi(argv[2]);
    if (task_num <= 0) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        cout << "Task_num must be a positive interger." << endl;
        return -1;
    }

    cout << "submit job, job_type:" << job_type << ", task_num:" << task_num << endl;
    string endpoint = "192.168.10.187:9999"; // hy
    // string endpoint = "192.168.120.88:9999"; // iie

    // build job ClassAd 
    ClassAd ad;
    // overview
    ad.InsertAttr(ATTR_JOB_TYPE, job_type);
    ad.InsertAttr(ATTR_GROUP, "abc");
