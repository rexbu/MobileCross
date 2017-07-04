/**
 * file :	NativeObject.cpp
 * author :	bushaofeng
 * create :	2016-08-24 14:53
 * func : 
 * history:
 */

#include "NativeObject.h"

extern JavaVM*	g_jvm;

// NativeObject::NativeObject(const char* className){
// 	//jvm->AttachCurrentThread(&m_jenv, NULL);
// 	g_jvm->GetEnv(&m_jenv, JNI_VERSION_1_6);

// 	m_class = m_jenv->FindClass(className);
// 	jmethodID construction_id = m_jenv->GetMethodID(m_class,  
//             "<init>", "()V");
// 	m_object = m_jenv->NewObject(m_class, construction_id);
// }