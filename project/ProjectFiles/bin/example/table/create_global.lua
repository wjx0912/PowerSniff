setmetatable(_G, {
  __newindex = function (_, n)
    error("attempt to write to undeclared variable "..n, 2)
  end,
  __index = function (_, n)
    error("attempt to read undeclared variable "..n, 2)
  end,
})

--> a = 1
--stdin:1: attempt to write to undeclared variable a 

--绕过metamethod
function declare (name, initval)
  rawset(_G, name, initval or false)
end

if rawget(_G, var) == nil then
  -- 'var' is undeclared
  -- ...
end

--> a = 1
--stdin:1: attempt to write to undeclared variable a
--> declare "a"
--> a = 1 -- OK

--允许全局变量为nil的方案
local declaredNames = {}
function declare (name, initval)
  rawset(_G, name, initval)
  declaredNames[name] = true
end
setmetatable(_G, {
  __newindex = function (t, n, v)
    if not declaredNames[n] then
      error("attempt to write to undeclared var. "..n, 2)
    else
      rawset(t, n, v) -- do the actual set
    end
  end,
  __index = function (_, n)
    if not declaredNames[n] then
      error("attempt to read undeclared var. "..n, 2)
    else
      return nil
    end
  end,
})
