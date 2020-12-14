t = {} -- original table (created somewhere)

-- keep a private access to original table
local _t = t

-- -- create proxy
t = {}

-- -- create metatable
local mt = {
  __index = function (t,k)
    print("*access to element " .. tostring(k))
    return _t[k] -- access the original table
  end,

  __newindex = function (t,k,v)
    print("*update of element " .. tostring(k) .." to " .. tostring(v))
  _t[k] = v -- update original table
  end
}

setmetatable(t, mt)

--> t[2] = 'hello'
--*update of element 2 to hello
--> print(t[2])
--*access to element 2
--hello
