--dofile每次都要编译文件 

function dofile (filename)
  local f = assert(loadfile(filename))
  return f()
end 

--loadstring和loadfile相似
f = loadstring("local a = 10; return a + 20")

--loadfile和loadstring都不会抛出错误,如果发生错误将返回nil和错误信息

-- file `foo.lua'
function foo (x)
  print(x)
end

--loadfile编译代码但是不定义代码 

f() -- defines `foo'
foo("ok") --> ok

--或者 
loadstring(s)()
assert(loadstring(s))()

--使用return加载表达式 
print "enter your expression:"
local l = io.read()
local func = assert(loadstring("return " .. l))
print("the value of your expression is " .. func())

--loadstring加载的函数可以多次调用
print "enter function to be plotted (with variable `x'):"
local l = io.read()
local f = assert(loadstring("return " .. l))

for i=1,20 do
  x = i -- global `x' (to be visible from the chunk)
  print(string.rep("*", f()))
end
