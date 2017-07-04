/**
 * file :	DataBase.cpp
 * author :	Rex
 * create :	2017-06-15 17:07
 * func : 
 * history:
 */

#include "Database.h"
#include "McFile.h"

using namespace mc;

SQLiteDb::SQLiteDb(const char* name):
Database(name){
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", FileManager::resourcePath());
    m_db_path = path;
    
    if (!FileManager::exist(path)) {
        int result = sqlite3_open(path, &m_db); //
        if (result != SQLITE_OK){
            err_log("Open Database Error: %d\n", result);
        }
    }
}

void SQLiteDb::createTable(const char* table, JSONObject* columns){
    char sql[10240];
    
}
void SQLiteDb::insert(const char* table, JSONObject* obj){
    
}
void SQLiteDb::query(const char* table, JSONObject* obj){
    
}
void SQLiteDb::remove(const char* table, JSONObject* obj){
    
}
bool SQLiteDb::update(const char* table, JSONObject* obj){
    return true;
}
bool SQLiteDb::query_forward(){
    return true;
}
bool SQLiteDb::column_int(){
    return true;
}
bool SQLiteDb::column_string(){
    return true;
}
bool SQLiteDb::column_double(){
    return true;
}
bool SQLiteDb::column_bool(){
    return true;
}

SQLiteDb::~SQLiteDb(){
    sqlite3_close(m_db);
    m_db = NULL;
}
