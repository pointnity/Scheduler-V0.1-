#include <stdio.h>
#include <zookeeper/zookeeper.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include <iostream>
#include "common/classad/classad_complement.h"
#include "include/type.h"

using std::string;

int32_t main(int argc, char* argv[]){
    string         classad_string = "[ATTR_JOB_ID = 1; b = \"Cardini\"]";
