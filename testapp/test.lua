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

weblua.add("/test.png","test.png")

weblua.add("/","test.html",function (request_id)
    if weblua.isPOST(request_id) then
        weblua.log(request_id, "username: "..weblua.get_form_feild(request_id,"username"))
        weblua.log(request_id, "message: "..weblua.get_form_feild(request_id,"message"))
    end
end)