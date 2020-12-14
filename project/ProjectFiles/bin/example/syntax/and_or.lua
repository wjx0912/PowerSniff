--[[ 
--and的优先级比or高
--a and b --> a为false返回a，否则返回b 
--a or b  --> a为true返回a，否则返回b
--]]

print(4 and 5) --> 5
print(nil and 13) --> nil
print(false and 13) --> false
print(4 or 5) --> 4
print(false or 5) --> 5

--技巧
v = 0
x = x or v
--等价于
if not x then
  x = v
end

--a ? b : c 
(a and b) or c
a and b or c

--not总是返回true或false
print(not nil) --> true
print(not false) --> true
print(not 0) --> false
print(not not nil) --> false
