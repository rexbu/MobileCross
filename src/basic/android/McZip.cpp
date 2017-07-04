/**
 * file :	McZip.cpp
 * author :	Rex
 * create :	2016-11-25 18:56
 * func : 
 * history:
 */

#include <jni.h>
#include "bs.h"
#include "McZip.h"
#include "McFile.h"
#include "libzip/zip.h"

extern JavaVM*  g_jvm;
using namespace mc;
bool Zip::unzip(const char* path, const char* destination, ZipCallback* callback, bool overwrite ,
        const char* password, ZipArchiveCallback* delegate){
    JNIEnv*         env;
    g_jvm->AttachCurrentThread(&env, NULL);
    
    jstring jpath = env->NewStringUTF(path);
    jstring jdest = env->NewStringUTF(destination);
    jclass jc= env->FindClass("com/rex/utils/FileUtil");
    jmethodID method = env->GetStaticMethodID(jc, "unzip", "(Ljava/lang/String;Ljava/lang/String;)Z");
    return env->CallStaticBooleanMethod(jc, method, jpath, jdest);
}
bool unzip(const char* path, const char* destination){
    int error;
    struct zip* apkArchive=zip_open(path, ZIP_CHECKCONS, &error);
    struct zip_stat fstat; 
   	zip_stat_init(&fstat);
   	if (apkArchive==NULL)
   	{
   		err_log("Open zip file[%s] error: %d", path, error);
   		return false;
   	}

   	if (!mc::FileManager::exist(destination))
   	{
   		mc::FileManager::mkdir(destination);
   	}

   	int numFiles = zip_get_num_files(apkArchive);
   	for (int i=0; i<numFiles; i++) {  
    	const char* name = zip_get_name(apkArchive, i, 0);   
     	if (name == NULL) {  
      		err_log("Error reading zip file name at [%d/%s] : %s", i, name, zip_strerror(apkArchive));  
      		return false;  
    	}  
     	err_log("unzip name: %s", name);
    	//struct zip_file* file = zip_fopen(apkArchive, name, 0);  
    	struct zip_file* file = zip_fopen_index(apkArchive, i, 0);  
		if (!file) {  
			err_log("Error opening %s from %s", name, path);  
		 	return false;  
		}  

		zip_stat(apkArchive, name, 0, &fstat);  
		char *buffer=(char *)malloc(fstat.size+1);  
		buffer[fstat.size]=0;  
		int numBytesRead =  zip_fread(file, buffer,fstat.size);

		char path[1024];
		sprintf(path, "%s/%s", destination, name);
		mc::FileManager::write(path, buffer, numBytesRead);
		free(buffer);
		zip_fclose(file);
   }

   zip_close(apkArchive);
   return true;
}
