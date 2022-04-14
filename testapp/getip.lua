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

--register page
weblua.add("/",get_ip)