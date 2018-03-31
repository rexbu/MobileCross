//
//  basic.h
//  MeIM
//
//  Created by Fantasist on 14-2-27.
//  Copyright (c) 2014年 Fantasist. All rights reserved.
//

#ifndef __basic_h
#define __basic_h

#include "json.h"
#include "device.h"
#include "file.h"
#include "http.h"
#include "zip.h"
#include "thread.h"
#include "reference.h"
#include "shared_preferences.h"
#include "task.h"
#include "threadpool.h"
#include "timer.h"

// 内存管理好烦啊
// 异步删除内存，用于回调中删除自己
#define MC_ASYNC_FREE(type, p)      do{     \
    AsyncFrame::shareInstance()->addTask(type##delete__, p); \
}while(0);

// 异步删除Task函数
#define MC_DELETE_FUNC(type)    void type##delete__(void* para){ \
    type * callback = (type *)para;     \
    delete callback;                    \
}

#define MC_DELETE_CLASS_FUNC(type)      class type; \
void* type##delete__(void* para){       \
    type * callback = (type*)para;      \
    delete callback;                    \
    return NULL;                        \
}
#endif
