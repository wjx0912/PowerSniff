-- create a namespace
Window = {}
-- -- create the prototype with default values
Window.prototype = {x=0, y=0, width=100, height=100, }
-- -- create a metatable
Window.mt = {}
-- -- declare the constructor function
function Window.new (o)
  setmetatable(o, Window.mt)
  return o
end

--用函数实现继承 
Window.mt.__index = function (table, key)
  return Window.prototype[key]
end 

w = Window.new{x=10, y=20}
print(w.width) --> 100 

--直接用表实现
Window.mt.__index = Window.prototype 
--Window.prototype["width"] 
