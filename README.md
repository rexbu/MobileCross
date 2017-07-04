# MobileCross
一个移动端跨平台的通用库，包括http、文件、json、线程、异步等

# Basic

## JSON
基于cJSON的的json解析库，包括JSONObject和JSONArray

### JSONObject
jsonn对象
### JSONArray
json数组

## McDevice
设备信息获取库

- device_id: 设备唯一ID
- bundle_id：ios包名
- package_name: 安卓包名
- device_version: 设备信息
- system_version: 系统信息

## McFile
文件管理

###Folder 文件夹
- type： 查找某个后缀的文件路径
- name： 查找某个文件名的文件路径

### File
- open : 打开某个文件
- read : 读取文件内容
- exist: 文件是否存在
- size : 文件大小

### FileManager
- shareInstance： 单例
- isFile : 是否是文件
- isDir : 是否是文件夹
- exist : 是否存在，不区分文件或者文件夹
- mkdir : 创建文件夹
- move : 移动文件或者文件夹
- remove : 删除文件或者文件夹
- write: 写入到某个文件
- rootPath: 跟目录，ios在Cache，安卓在data
- resourcePath: ios在mainBundle.resourcePath，安卓在data/__resouce
- tempPath: 临时存储路径

### Resource
安卓用于管理assets文件

## McHttp
异步http

### HttpCallback http回调
- done：完成后的回调函数

### HttpSession
- get(url, callback): callback为完成后异步回调
- post(url, callback): callback为完成后异步回调
- put(url, callback): callback为完成后异步回调
- delete(url, callback): callback为完成后异步回调
- addHttpHeader: 添加http头

## McTask
基于管道和select的异步线程，性能不好，推荐ThreadPool

## McZip
zip压缩，已经删除，改为使用libzippp

## SharedPreferences
一个轻量级的对象存储，类似Android的SharedPreferences，基于json

###SharedPreferences
- commit：提交更改，保存到本地

## Thread
线程管理

### Thread
- run : 线程执行函数

### LoopThread 循环线程
- loop ：循环函数，需要自定义

## ThreadPool
异步线程池，基于pthread_cond_t

### ThreadWork 工作线程

### ThreadPool 线程池
- add(void* (\*run)(void\*),void\* arg) : 将一个线程函数run提交到线程池，参数为arg

### async_run 
异步执行一个函数

# 3rdparty
第三方依赖

## libzippp
zip压缩与解压，参考github： ctabin/libzippp

## libzip
zip解压与压缩，参考github： julienr/libzip-android

## cJSON
json解析

# model
一些框架库

- async： 异步socket框架
- utp： 基于udp的可靠传输
- p2p：基于utp的p2p传输
- push：推送

## async

### AsyncSocket
异步Socket

### AsyncFrame
基于管道的异步框架

### SocketFrame
基于AsyncFrame的异步socket框架

### ListenSocket
SocketFrame的tcp实现和udp实现
