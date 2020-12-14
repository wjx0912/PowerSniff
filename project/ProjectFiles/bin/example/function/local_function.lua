local f = function (...)
  ...
end 

local g = function (...)
  ...
  f() -- external local `f' is visible here
  ...
end 

local function f (...)
  ...
end 

local fact = function (n)
  if n == 0 then
    return 1
  else
    return n*fact(n-1) -- buggy,lua认为fact是全局函数
  end
end 

--改进处理 
local fact
fact = function (n)
  if n == 0 then
    return 1
  else
    return n*fact(n-1)
  end
end 

--lua扩展了递归语法，但是非直接递归函数要先申明 
local f, g -- `forward' declarations

function g ()
  ... f() ...
end

function f ()
  ... g() ...
end 
