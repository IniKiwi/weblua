--png file
weblua.add("/test.png","test.png")

--html page, POST forms, storage set
weblua.add("/","test.html",function (request_id)
    if request.isPOST(request_id) then
        local username = request.get_form_feild(request_id,"username")
        local message = request.get_form_feild(request_id,"message")
        request.log(request_id, "username: "..username)
        weblrequestua.log(request_id, "message: "..message)
        storage.set("messages."..username, message)
    end
end)

--storage read
weblua.add("/messages", function (request_id)
    request.set_mimetype(request_id, "text/html")
    if request.isPOST(request_id) then
        local username = request.get_form_feild(request_id,"username")
        local message = storage.get("messages."..username)
        if message ~= nil then
            request.render(request_id,"message.html", {username=username, message=message})
        else
            request.http_redirect(request_id,"/")
        end
        
    else
        request.http_redirect(request_id,"/")
    end
end)