/********************************
 FileName: ResourceScheduler/dispatcher.h
 
 
 Version:  0.1
 Description: dispatcher for events; events -> dispatcher -> handler
*********************************/

#ifndef SRC_RESOURCE_SCHEDULER_DISPATCHER_H
#define SRC_RESOURCE_SCHEDULER_DISPATCHER_H

#include <map>
#include "common/clynn/singleton.h"
#include "ResourceScheduler/handler.h"

using std::map;

class Dispatcher {
