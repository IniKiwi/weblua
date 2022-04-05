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
### demo script
```lua
--get ip function
local function get_ip(request_id)
    --get client ip
    local ip = weblua.get_ip(request_id)
    weblua.log(request_id, "user ip: "..ip)

    --custom html responce
    weblua.set_mimetype(request_id, "text/plain")
    weblua.set_status(request_id, "200 OK")
    weblua.set_data(request_id, ip)
end
weblua.add("/get_ip",get_ip)

--png file
weblua.add("/test.png","test.png")

--html page, POST forms, storage set
weblua.add("/","test.html",function (request_id)
    if weblua.isPOST(request_id) then
        local username = weblua.get_form_feild(request_id,"username")
        local message = weblua.get_form_feild(request_id,"message")
        weblua.log(request_id, "username: "..username)
        weblua.log(request_id, "message: "..message)
        storage.set("messages."..username, message)
    end
end)

--storage read
weblua.add("/messages", function (request_id)
    weblua.set_mimetype(request_id, "text/plain")
    weblua.set_status(request_id, "200 OK")
    if weblua.isPOST(request_id) then
        local username = weblua.get_form_feild(request_id,"username")
        local message = storage.get("messages."..username)
        if message ~= nil then
            weblua.set_data(request_id, message)
        else
            weblua.set_data(request_id, "message not found!")
        end
        
    else
        weblua.set_data(request_id, "the page is only for POST requests")
        weblua.http_redirect(request_id,"/")
    end
end)
```
### functions

#### weblua.add()
```lua
weblua.add("/path/","file") 
weblua.add("/path/","file", callback_function)
weblua.add("/path/", callback_function)
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
2. the data type `STORAGE_STRING` or `STORAGE_NUMBER`
```lua
local data, data_type = storage.get("key")
if data ~= nil then
    if data_type == STORAGE_STRING then
        --data is a string
    else if data_type == STORAGE_NUMBER then
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