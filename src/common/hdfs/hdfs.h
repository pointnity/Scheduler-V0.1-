#ifndef LIBHDFS_HDFS_H
#define LIBHDFS_HDFS_H

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <jni.h>

#ifndef O_RDONLY
#define O_RDONLY 1
#endif

#ifndef O_WRONLY 
#define O_WRONLY 2
#endif

#ifndef EINTERNAL
#define EINTERNAL 255 
#endif


/** All APIs set errno to meaningful values */
#ifdef __cplusplus
extern  "C" {
#endif

    /**
     * Some utility decls used in libhdfs.
     */

    typedef int32_t   tSize; /// size of data for read/write io ops 
    typedef time_t    tTime; /// time type in seconds
    typedef int64_t   tOffset;/// offset within the file
    typedef uint16_t  tPort; /// port
    typedef enum tObjectKind {
        kObjectKindFile = 'F',
        kObjectKindDirectory = 'D',
    } tObjectKind;


    /**
     * The C reflection of org.apache.org.hadoop.FileSystem .
     */
    typedef void* hdfsFS;

    
    /**
     * The C equivalent of org.apache.org.hadoop.FSData(Input|Output)Stream .
     */
    enum hdfsStreamType
    {
        UNINITIALIZED = 0,
        INPUT = 1,
        OUTPUT = 2,
    };

    
    /**
     * The 'file-handle' to a file in hdfs.
     */
    struct hdfsFile_internal {
        void* file;
        enum hdfsStreamType type;
    };
    typedef struct hdfsFile_internal* hdfsFile;
      

    /** 
     * hdfsConnectAsUser - Connect to a hdfs file system as a specific user
     * Connect to the hdfs.
     * @param host A string containing either a host name, or an ip address
     * of the namenode of a hdfs cluster. 'host' should be passed as NULL if
     * you want to connect to local filesystem. 'host' should be passed as
     * 'default' (and port as 0) to used the 'configured' filesystem
     * (core-site/core-default.xml).
     * @param port The port on which the server is listening.
     * @param user the user name (this is hadoop domain user). Or NULL is equivelant to hhdfsConnect(host, port)
     * @return Returns a handle to the filesystem or NULL on error.
     */
     hdfsFS hdfsConnectAsUser(const char* host, tPort port, const char *user);


    /** 
     * hdfsConnect - Connect to a hdfs file system.
     * Connect to the hdfs.
     * @param host A string containing either a host name, or an ip address
     * of the namenode of a hdfs cluster. 'host' should be passed as NULL if
     * you want to connect to local filesystem. 'host' should be passed as
     * 'default' (and port as 0) to used the 'configured' filesystem
     * (core-site/core-default.xml).
     * @param port The port on which the server is listening.
     * @return Returns a handle to the filesystem or NULL on error.
     */
     hdfsFS hdfsConnect(const char* host, tPort port);


    /** 
     * hdfsDisconnect - Disconnect from the hdfs file system.
     * Disconnect from hdfs.
     * @param fs The configured filesystem handle.
     * @return Returns 0 on success, -1 on error.  
     */
    int hdfsDisconnect(hdfsFS fs);
        

    /** 
     * hdfsOpenFile - Open a hdfs file in given mode.
     * @param fs The configured filesystem handle.
     * @param path The full path to the file.
     * @param flags - an | of bits/fcntl.h file flags - supported flags are O_RDONLY, O_WRONLY (meaning create or overwrite i.e., implies O_TRUNCAT), 
     * O_WRONLY|O_APPEND. Other flags are generally ignored other than (O_RDWR || (O_EXCL & O_CREAT)) which return NULL and set errno equal ENOTSUP.
     * @param bufferSize Size of buffer for read/write - pass 0 if you want
     * to use the default configured values.
     * @param replication Block replication - pass 0 if you want to use
     * the default configured values.
     * @param blocksize Size of block - pass 0 if you want to use the
     * default configured values.
     * @return Returns the handle to the open file or NULL on error.
