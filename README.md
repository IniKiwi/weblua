# weblua
a C++ http server with lua

## installation on debian based
#### Step 1: Install dependencies
```bash
sudo apt -y install build-essential cmake libpthread-stubs0-dev libluajit-5.1-dev
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
weblua [lua script]
```
use `ctrl + c` to stop the program
## the weblua lua api
### demo script
```lua
local function callback(request_id)
    weblua.log(request_id,"callback")
end

weblua.add("/","test.html")
weblua.add("/callback/","test.html", callback)
weblua.add("/get_ip/",function (request_id)
    --get client ip
    local ip = weblua.get_ip(request_id)
    weblua.log(request_id, "user ip: "..ip)

    --custom html responce
    weblua.set_mimetype(request_id, "text/plain")
    weblua.set_status(request_id, "200 OK")
    weblua.set_data(request_id, ip)

end)

weblua.add("/png/",function (request_id)
    --load png file
    weblua.set_data_file(request_id,"test.png")
    --set reponce mimetype
    weblua.set_mimetype(request_id, "image/x-png")
    weblua.set_status(request_id, "200 OK")
end)
```
### functions

#### weblua.add()
```lua
weblua.add("/path/","file") 
weblua.add("/path/","file", callback_function)
weblua.add("/path/", callback_function)
```

### callback functions
callback functions needs a `request_id`
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

#### weblua.set_data_file()
load and read file
```lua
weblua.set_data_file(request_id,"image.png") --needs weblua.set_mimetype(request_id, "image/x-png")
``` 