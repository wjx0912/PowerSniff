function foo ()
  ...
  if unexpected_condition then error() end
  ...
  print(a[i]) -- potential error: `a' may not be a table
  ...
end

if pcall(foo) then
  -- no errors while running `foo'
  ...
else
  -- -- `foo' raised an error: take appropriate actions
  ...
end

local status, err = pcall(function () a = 'a'+1 end)
print(err)
--> stdin:1: attempt to perform arithmetic on a string value

local status, err = pcall(function () error("my error") end)
print(err)
--> stdin:1: my error

--error第二个参数指定代码的层次
function foo (str)
  if type(str) ~= "string" then
    error("string expected", 2)
  end
  ...
end

--堆栈
print(debug.traceback())
