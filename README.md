# weblua
a C++ http server with lua

## installation on debian based
#### Step 1: Install dependencies
```bash
sudo apt -y install build-essential cmake libpthread-stubs0-dev libluajit-5.1-dev libsqlite3-dev
```

#### Step 2: build executable

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```
## run
```bash
sudo weblua [lua script]
```
use `ctrl + c` to stop the program \
`sudo` is needed to run the program (port 80)
## the weblua lua api
### functions
#### weblua.add()
```lua
weblua.add("/path/","file") 
weblua.add("/path/","file", callback_function)
weblua.add("/path/", callback_function)
```

### sql api
the sql module include all basic functions
#### sql.create_table()
create a table **if not exists** \
the type can be `STRING` or `NUMBER`
> :warning: the id is created automatically
```lua
sql.create_table("table_name",{"a_string",STRING, "random_number",NUMBER})
```
#### sql.insert()
insert a row in sql table
1. table name 
2. row data (table)
```lua
sql.insert("table_name",{a_string="0_0",random_number=99999})
```
#### sql.update()
update a row
1. table name
2. where (table)
3. new values (table)
```lua
--before: [id:1, a_string:"0_0",random_number: 99999 ]
sql.insert("table_name",{random_number=99999}, {a_string="I like the number 99999!"})
--after: [id:1, a_string:"I like the number 99999!",random_number: 99999 ]
```
#### sql.get()
return a table of rows
1. table name 
2. where (table)
```lua
local results = sql.get("table_name", {random_number=99999})
print(#results.." results") --prints number of results: 1 results
if #results > 0 then
    print("number 1: "..results[1].random_number) --prints first result number
    print("string 1: "..results[1].a_string) --prints first result string
end
```
#### sql.delete()
delete  rows
1. table name
2. where (table)
```lua
sql.delete("table_name", {random_number=99999})
```

### storage functions
the storage module use sqlite to store values
#### storage.set()
store a string or a number with a unique key
```lua
storage.set("key", "data")
storage.set("key", 750)
``` 
#### storage.get()
get a value with a unique key \
returns 2 values:
1. the data 
2. the data type `STRING` or `NUMBER`
```lua
local data, data_type = storage.get("key")
if data ~= nil then
    if data_type == STRING then
        --data is a string
    else if data_type == NUMBER then
        --data is a number
    end
else
    --NULL
end
```
### callback functions
callback functions needs a `request_id`
#### weblua.render()
store in dynamic memory a text file render (see example) \
activate dynamic memory
> :warning: **only for 2Mo can be sent to the internet browser** 
```lua
weblua.render(request_id, "status.html", {name="IniKiwi", status="busy"})
-- input (status.html)
-- <h1>{{name}} is {{status}} {{unused}}</h1>
-- output (internet browser view)
-- <h1>IniKiwi is busy</h1>
```
#### weblua.log()
print string to the server console
```lua
weblua.log(request_id, "new request!")
```
#### weblua.get_ip() 
returns client ip as a string
```lua
local ip = weblua.get_ip(request_id)
```
#### weblua.set_mimetype()
set the http response mimetype
```lua
weblua.set_mimetype(request_id, "text/html")
``` 
#### weblua.set_status()
set the http response status
```lua
weblua.set_status(request_id, "404 Not Found")
```
#### weblua.set_data()
set the http response data
```lua
weblua.set_data(request_id,"<h1>hello!</h1>")
```

#### weblua.load_file()
save file name in variable\
the file will be sent at the end of request with chunked transfer
```lua
weblua.load_file(request_id,"schoolfiles.zip") 
``` 
#### weblua.isPOST()
returns true if the request is a POST
```lua
if weblua.isPOST(request_id) then
    --code
end
```
#### weblua.get_form_feild()
returns the form feild data
> :warning: **only for POST requests** use `weblua.isPOST()` to check
```lua
local value = weblua.get_form_feild(request_id, "feild_name")
```

#### weblua.http_redirect()
returns 301 Moved Permanently to the navigator
> :warning: **no other content can be sent to the internet browser** but the callback will continue its execution until the end
```lua
weblua.http_redirect(request_id, "/")
```