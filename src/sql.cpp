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

int sql::l_create_table(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        size_t len = lua_objlen(state,2);
        if(len % 2 == 0){
            int _err;
            std::string sqlcom = "CREATE TABLE IF NOT EXISTS ";
            sqlcom += lua_tostring(state,1);
            sqlcom += "(id INTEGER PRIMARY KEY";
            for(int i=0;i<len/2;i++){
                sqlcom += ", ";
                lua_rawgeti(state,2,i*2+1);
                lua_rawgeti(state,2,i*2+2);
                sqlcom += lua_tostring(state, -2);
                if(lua_tointeger(state, -1)== SQLITE_TEXT){
                    sqlcom += " TEXT";
                }
                if(lua_tointeger(state, -1)== SQLITE_FLOAT){
                    sqlcom += " REAL";
                }
                lua_pop(state, 1);
                lua_pop(state, 1);
            }
            sqlcom += ");";
            _err = sqlite3_exec(sql::DB,sqlcom.c_str(),NULL,0,&sql::message_error);
            if (_err != SQLITE_OK) {
                std::cerr << "SQL error: "<< sql::message_error << std::endl;
                sqlite3_free(sql::message_error);
            }
        }
        
    }
    return 0;
}

int sql::l_insert(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        int _err;
        std::string valuesname = "";
        std::string values = "";
        std::string table = lua_tostring(state,1);
        std::string v;
        lua_pushnil(state); 
        while (lua_next(state, 2) != 0){
            // uses 'key' (at index -2) and 'value' (at index -1)
            // removes 'value'; keeps 'key' for next iteration
            valuesname += lua_tostring(state,-2);
            valuesname += ", ";
            if(lua_isstring(state,-1)){
                v = lua_tostring(state,-1);
                replace_all(v,"\"","\"\"");
                replace_all(v,"'","''");
                values += "'"+v+"'";
            } else {
                v = lua_tonumber(state,-1);
                values += v;
            }
            
            values += ", ";
            lua_pop(state, 1);
        }
        values = values.substr(0, values.size()-2);
        valuesname = valuesname.substr(0, valuesname.size()-2);
        std::string sqlcom = "INSERT INTO "+table+"("+valuesname+") VALUES("+values+");";
        _err = sqlite3_exec(sql::DB,sqlcom.c_str(),NULL,0,&sql::message_error);
        if (_err != SQLITE_OK) {
        std::cerr << "SQL error: "<< sql::message_error << std::endl;
        sqlite3_free(sql::message_error);
        }
    }
    return 0;
}
    
int sql::l_get(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        std::string sqlcom = "SELECT * FROM ";
        std::string v;
        sqlite3_stmt * stmt;
        int _err;
        sqlcom += lua_tostring(state,1);
        sqlcom += " WHERE ";
        lua_pushnil(state); 
        while (lua_next(state, 2) != 0){
            sqlcom += lua_tostring(state,-2);
            sqlcom += " = ";
            if(lua_isstring(state,-1)){
                v = lua_tostring(state,-1);
                replace_all(v,"\"","\"\"");
                replace_all(v,"'","''");
                sqlcom += "'"+v+"'";
            }else{
                sqlcom += lua_tonumber(state,-1);
            }
            sqlcom += " AND ";
            lua_pop(state, 1);
        }
        sqlcom = sqlcom.substr(0, sqlcom.size()-5);
        sqlcom += ";";
        int rows = 0;
        _err = sqlite3_prepare_v2(sql::DB, sqlcom.c_str(), -1, &stmt, NULL );
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            rows++;
        }
        sqlite3_finalize(stmt);
        if (_err != SQLITE_OK) {
            std::cerr << "SQL error: "<< sql::message_error << std::endl;
            sqlite3_free(sql::message_error);
        }
        if(rows==0){
            lua_createtable(state, rows, 0);
            return 1;
        }

        _err = sqlite3_prepare_v2(sql::DB, sqlcom.c_str(), -1, &stmt, NULL );
        int row = 0;
        int columns;
        int column_type;
        lua_createtable(state, rows, 0);
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            row++;
            lua_pushnumber(state, row);
            columns = sqlite3_column_count(stmt);
            lua_createtable(state, 0, columns);
            for(int column=0; column<columns; column++){
                column_type = sqlite3_column_type(stmt, column);
                const char * columnName = sqlite3_column_name(stmt, column);
                if (column_type == SQLITE_FLOAT){
                    double valDouble = sqlite3_column_double(stmt, column); 
                    lua_pushnumber(state, sqlite3_column_double(stmt, column));
                    lua_setfield(state, -2, columnName);
                }
                if (column_type == SQLITE_TEXT){
                    const unsigned char * valChar = sqlite3_column_text(stmt, column);
                    lua_pushstring(state, (const char *)sqlite3_column_text(stmt, column));
                    lua_setfield(state, -2, columnName);
                }
                if (column_type == SQLITE_NULL){
                    lua_pushnil(state);
                    lua_setfield(state, -2, columnName);
                }
            }
            lua_settable(state, -3);
        }
        sqlite3_finalize(stmt);
        return 1;
    }
    lua_createtable(state, 0, 0);
    return 1;
}

int sql::l_update(lua_State* state){
    int args = lua_gettop(state);
    if(args == 3){
        std::string sqlcom = "UPDATE ";
        std::string v;
        sqlcom += lua_tostring(state,1);
        sqlcom += " SET ";
        lua_pushnil(state); 
        while (lua_next(state, 3) != 0){
            sqlcom += lua_tostring(state,-2);
            sqlcom += " = ";
            if(lua_isstring(state,-1)){
                v = lua_tostring(state,-1);
                replace_all(v,"\"","\"\"");
                replace_all(v,"'","''");
                sqlcom += "'"+v+"'";
            }else{
                sqlcom += lua_tonumber(state,-1);
            }
            sqlcom += ",";
            lua_pop(state, 1);
        }
        sqlcom = sqlcom.substr(0, sqlcom.size()-1);
        lua_pop(state, 1);

        sqlcom += " WHERE ";
        lua_pushnil(state); 
        while (lua_next(state, 2) != 0){
            sqlcom += lua_tostring(state,-2);
            sqlcom += " = ";
            if(lua_isstring(state,-1)){
                v = lua_tostring(state,-1);
                replace_all(v,"\"","\"\"");
                replace_all(v,"'","''");
                sqlcom += "'"+v+"'";
            }else{
                sqlcom += lua_tonumber(state,-1);
            }
            sqlcom += " AND ";
            lua_pop(state, 1);
            
        }
        sqlcom = sqlcom.substr(0, sqlcom.size()-5);
        sqlcom += ";";
        sqlite3_exec(sql::DB,sqlcom.c_str(),NULL,0,NULL);
    }
    return 0;
}

int sql::l_delete(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        std::string sqlcom = "DELETE FROM ";
        std::string v;
        sqlcom += lua_tostring(state,1);
        sqlcom += " WHERE ";
        lua_pushnil(state); 
        while (lua_next(state, 2) != 0){
            sqlcom += lua_tostring(state,-2);
            sqlcom += " = ";
            if(lua_isstring(state,-1)){
                v = lua_tostring(state,-1);
                replace_all(v,"\"","\"\"");
                replace_all(v,"'","''");
                sqlcom += "'"+v+"'";
            }else{
                sqlcom += lua_tonumber(state,-1);
            }
            sqlcom += " AND ";
            lua_pop(state, 1);
            
        }
        sqlcom = sqlcom.substr(0, sqlcom.size()-5);
        sqlcom += ";";
        sqlite3_exec(sql::DB,sqlcom.c_str(),NULL,0,NULL);
    }
}