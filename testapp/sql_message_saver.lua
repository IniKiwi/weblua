--sql initialisation
sql.create_table("users",{"name",STRING, "message",STRING })

--show weblua version
print("weblua version: "..WEBLUA_VERSION_STR)

--pages functions
local function main_page(request_id)
    if request.isPOST(request_id) then
        local username = request.get_form_feild(request_id,"username")
        local message = request.get_form_feild(request_id,"message")
        request.log(request_id, "username: "..username)
        request.log(request_id, "message: "..message)
        if #sql.get("users", {name=username}) == 0 then --if not exists
            sql.insert("users",{name=username,message=message})
        else --aready exists
            request.log(request_id, "update "..username.." message")
            sql.update("users",{name=username},{message=message})
        end
    end
end

local function show_message(request_id)
    request.set_mimetype(request_id, "text/html")
    if request.isPOST(request_id) then
        local username = request.get_form_feild(request_id,"username")
        local messages = sql.get("users", {name=username}) --get results
        request.log(request_id, "username: "..username)
        if #messages ~= 0 then -- check number of results
            request.render(request_id, "message.html",{username=username,message=messages[1].message}) --send first result
        else --no results
            request.http_redirect(request_id,"/")
        end
        
    else
        request.http_redirect(request_id,"/")
    end
end

--register pages
weblua.add("/test.png","test.png")
weblua.add("/","test.html",main_page)
weblua.add("/messages",show_message)