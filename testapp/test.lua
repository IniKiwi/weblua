local function callback()
    print("callback");
end

weblua.add("/","test.html")
weblua.add("/callback/","test.html", callback)