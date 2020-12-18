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
    ClassAdPtr job_ad = ClassAdComplement::StringToAd(job);
    if(job_ad == NULL) {
        LOG4CPLUS_ERROR(logger, "invalid job ad");
        return 1; 
    }
    int num_task = 0;
    if(job_ad->EvaluateAttrInt(ATTR_TOTAL_TASKS, num_task) || num_task < 1) {
        LOG4CPLUS_ERROR(logger, "no num_task or num_task < 1");
        return 1;
    }
    vector<ClassAd> machine_ads;
    if(COLLECTOR_ENGINE::Instance()->FetchMachinesForNegotiator(job_ad, machine_ads) == 0){  
         LOG4CPLUS_ERROR(logger, "no server info, aborting negotiation cycle");
         return 1; 
    }
    MatchList match_list;

    return 0;
}

int MatchMaker::NegotiationTask(const string& task, string& machine) {
