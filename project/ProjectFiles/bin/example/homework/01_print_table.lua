#!/usr/bin/env lua

--Homework
--Powered by Bob Wang
--Wangbo1982@nibirutech.com
--Lua 5.2 on MacxOS

--print usage
function print_usage()
	print '1) lua 01_print_table.lua'
	print "2) 01_print_table.lua"
	print [[3) lua dofile to execute...]]
	print ([[------------------------------]]..'--')
end

--print table
function print_table(table, level)
	local level = 0

	for k,v in pairs(table) do
		if type(v) == "table" then
			level = level + 1
			print_table(v, level)
		else
			if level > 0 then
				for idx= 1,level do 
					io.write "."
				end
			end
			print(k.." => "..v)
		end
	end
end

--to be print table
--global var with g_ prefix
g_table = {
	lilei = "hanmeimei", 
	game = "lua", 
	{one, two, three}, 
	languages = {"C", "C++", Ruby = {"DBI", "Rails", "Has Many Bugs"}, "Python", "Yacc", "Lex", Perl = "Complex" }
}

--main
print_usage()
print_table(g_table, 0)

--1 => C
--2 => C++
--3 => Python
--4 => Yacc
--5 => Lex
--Perl => Complex
--1 => DBI
--2 => Rails
--3 => Has Many Bugs
--..game => lua
--..lilei => hanmeimei
--打印出的结果的层次和顺序不是我想的那样
