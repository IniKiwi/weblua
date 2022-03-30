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