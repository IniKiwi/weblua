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
    weblua.set_mimetype(request_id, "text/html")
    if weblua.isPOST(request_id) then
        local username = weblua.get_form_feild(request_id,"username")
        local message = storage.get("messages."..username)
        if message ~= nil then
            weblua.render(request_id,"message.html", {username=username, message=message})
        else
            weblua.http_redirect(request_id,"/")
        end
        
    else
        weblua.http_redirect(request_id,"/")
    end
end)