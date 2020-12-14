--[[ 
--nil只和自己相等 
--值类型不同lua认为两者不同 
--通过引用比较tables,userdata,functions
--比较的时候数字和字符串不自动进行类型转换
--
--"0" == 0 -- false
--2 < 15 -- true
--"2" < "15" -- false (alphabetical order!)>>
--
--只有false和nil为假,比如0和""是真
--]]

a = {}; a.x = 1; a.y = 0
b = {}; b.x = 1; b.y = 0

c = a

print(a == c)
print(a ~= b)
print(a == b)
