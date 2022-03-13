local function callback(request_id)
    weblua.log(request_id,"callback")
end

weblua.add("/","test.html")
weblua.add("/callback/","test.html", callback)