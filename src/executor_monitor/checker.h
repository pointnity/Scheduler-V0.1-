/********************************
 FileName: executor/image_maneger.h
 Version:  0.1
 Description: image manager
*********************************/

#ifndef SRC_EXECUTOR_CHECKER_H
#define SRC_EXECUTOR_CHECKER_H

#include <string>
using std::string;

class Checker {
public:
    bool CheckVersion();
    bool IsProcess(const string& process_name);

private:
    bool UpdateExecutorFromHdfs();
};

#endif  
