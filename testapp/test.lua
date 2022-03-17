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
    weblua.load_file(request_id,"test.png")
    --set reponce mimetype
    weblua.set_mimetype(request_id, "image/x-png")
    weblua.set_status(request_id, "200 OK")
end)