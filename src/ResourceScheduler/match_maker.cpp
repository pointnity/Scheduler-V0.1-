/********************************
 FileName: ResourceScheduler/match_maker.cpp


 Version:  0.1
 Description: match machine with job
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "common/classad/classad_complement.h"
#include "include/classad_attr.h"
#include "ResourceScheduler/match_maker.h"
#include "ResourceScheduler/resource_scheduler_engine.h"
#include "ResourceScheduler/match_list.h"

using log4cplus::Logger;
static Logger logger = Logger::getInstance("ResourceScheduler");

int MatchMaker::Negotiation(const string& job, vector<string>& candidates) {
    LOG4CPLUS_INFO(logger, "receive a job:" << job);
    candidates.clear();
