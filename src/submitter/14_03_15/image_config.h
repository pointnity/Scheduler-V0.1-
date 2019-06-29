/********************************
 File name: submitter/image_config.h
 Version:  0.1
 Description: image config 
*********************************/

#ifndef SRC_SUBMITTER_APP_CONFIG_H
#define SRC_SUBMITTER_APP_CONFIG_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/rwlock.h"
#include "common/clynn/rpc.h"
#include "common/clynn/singleton.h"

class ImageConfigI{
public:
     bool Init(const string& conf_file);
