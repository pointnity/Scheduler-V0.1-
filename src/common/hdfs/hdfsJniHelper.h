#ifndef LIBHDFS_JNI_HELPER_H
#define LIBHDFS_JNI_HELPER_H

#include <jni.h>
#include <stdio.h>

#include <stdlib.h>
#include <stdarg.h>
#include <search.h>
#include <pthread.h>
#include <errno.h>

#define PATH_SEPARATOR ':'


/** Denote the method we want to invoke as STATIC or INSTANCE */
typedef enum {
    STATIC,
    INSTANCE
} MethType;


/** Used for returning an appropriate return value after invoking
 * a method
 */
typedef jvalue RetVal;

/** Used for returning the exception after invoking a method */
typedef jthrowable Exc;

/** invokeMethod: Invoke a Static or Instance method.
 * className: Name of the class where the method can be found
 * methName: Name of the method
 * methSignature: the signature of the method "(arg-types)ret-type"
 * methType: The type of the method (STATIC or INSTANCE)
 * instObj: Required if the methType is INSTANCE. The object to invoke
   the method on.
 * env: The JNIEnv pointer
 * retval: The pointer to a union type which will contain the result of the
   method invocation, e.g. if the method returns an Object, retval will be
   set to that, if the method returns boolean, retval will be set to the
   value (JNI_TRUE or JNI_FALSE), etc.
 * exc: If the methods throws any exception, this will contain the reference
 * Arguments (the method arguments) must be passed after methSignature
 * RETURNS: -1 on error and 0 on success. If -1 is returned, exc will have 
   a valid exception reference, and the result stored at retval is undefined.
 */
int invokeMethod(JNIEnv *env, RetVal *retval, Exc *exc, MethType methType,
                 jobject instObj, const char *className, const char *methName, 
                 const char *methSignature, ...);

/** constructNewObjectOfClass: Invoke a constructor.
 * className: Name of the class
 * ctorSignature: the signature of the constructor "(arg-types)V"
 * env: The JNIEnv pointer
 * exc: If the ctor throws any exception, this will contain the reference
 * Arguments to the ctor must be passed after ctorSignature 
 */
jobject constructNewObjectOfClass(JNIEnv *env, Exc *exc, const char *className, 
                                  const char *ctorSignature, ...);

jmethodID methodIdFromClass(const char *className, const char *methName, 
                            const char *methSignature, MethType methType, 
                            JNIEnv *env);

jclass globalClassReference(const char *className, JNIEnv *env);

/** classNameOfObject: Get an object's class name.
 * @param jobj: The object.
 * @param env: The JNIEnv pointer.
 * @return Returns a pointer to a string containing the class name. This string
 * must be freed by the caller.
 */
char *classNameOfObject(jobject jobj, JNIEnv *env);

/** getJNIEnv: A helper function to get the JNIEnv* for the given thread.
 * If no JVM exists, then one will be created. JVM command line arguments
