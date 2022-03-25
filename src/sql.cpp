#include <sql.h>
#include <iostream>
#include <sstream>
#include <lua_api/util.h>

sqlite3* sql::DB;
char* sql::message_error;

int sql::callback(void* data, int argc, char** argv, char** azColName){
    return 0;
}

void sql::init(){
    int _err;
    std::string sql_newtable = "CREATE TABLE IF NOT EXISTS weblua_storage ( id INTEGER PRIMARY KEY,weblua_key TEXT UNIQUE, type INTEGER, data_text TEXT, data_real REAL);";
    sqlite3_open("data.db", &sql::DB);
    _err = sqlite3_exec(sql::DB, sql_newtable.c_str(),NULL,0,&sql::message_error);
    if (_err != SQLITE_OK) {
        std::cerr << "SQL error: "<< sql::message_error << std::endl;
        sqlite3_free(sql::message_error);
    }
}

int sql::set(sql_storage_t value){
    int _err;
    replace_all(value.key,"\"","\"\"");
    replace_all(value.data_text,"\"","\"\"");
    replace_all(value.key,"'","''");
    replace_all(value.data_text,"'","''");
    std::string sqldel = "DELETE FROM weblua_storage WHERE weblua_key = '"+value.key+"';";
    sqlite3_free(sql::message_error);
    _err = sqlite3_exec(sql::DB, sqldel.c_str(),NULL,0,&sql::message_error);
    if(value.type == SQLITE_TEXT){
        std::string sqlcom = "INSERT INTO weblua_storage (weblua_key, type, data_text) VALUES ('"+value.key+"', "+std::to_string(value.type)+", '"+value.data_text+"');";
        _err = sqlite3_exec(sql::DB, sqlcom.c_str(),NULL,0,&sql::message_error);
    }
    if(value.type == SQLITE_FLOAT){
        std::stringstream sstream;
        sstream << value.data_float;
        std::string sqlcom = "INSERT INTO weblua_storage (weblua_key, type, data_real) VALUES ('"+value.key+"', "+std::to_string(value.type)+", "+sstream.str()+");";
        _err = sqlite3_exec(sql::DB, sqlcom.c_str(),NULL,0,&sql::message_error);
    }
    //_err = sqlite3_exec(sql::DB, "COMMIT",NULL,0,&sql::message_error);
    if (_err != SQLITE_OK) {
        std::cerr << "SQL error: "<< sql::message_error << std::endl;
        sqlite3_free(sql::message_error);
        return 1;
    }
    return 0;
}

sql_storage_t sql::get(std::string key){
    int _err;
    sqlite3_stmt * stmt;
    sql_storage_t ret;
    replace_all(key,"\"","\"\"");
    replace_all(key,"'","''");
    std::string sqlcom = "SELECT * FROM weblua_storage WHERE weblua_key = '"+key+"';";
    _err = sqlite3_prepare_v2( sql::DB, sqlcom.c_str(), -1, &stmt, NULL );
    if (_err != SQLITE_OK) {
        ret.type = 0;
        return ret;
    }
    _err = sqlite3_step(stmt);
    if(_err == SQLITE_ROW){
        if(sqlite3_column_int(stmt,2) == SQLITE_FLOAT){
            ret.type = sqlite3_column_int(stmt,2);
            ret.data_float = sqlite3_column_double(stmt,4);
        }
        if(sqlite3_column_int(stmt,2) == SQLITE_TEXT){
            ret.type = sqlite3_column_int(stmt,2);
            ret.data_text = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,3)));
        }
    }
    return ret;
}