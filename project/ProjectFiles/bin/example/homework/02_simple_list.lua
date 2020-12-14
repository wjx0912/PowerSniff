#!/usr/bin/env lua

--Homework
--Powered by Bob Wang
--Wangbo1982@nibirutech.com
--Lua 5.2 on MacxOS                                                                             
--
----print usage
function print_usage()
	print '1) lua 01_print_table.lua'
	print "2) 01_print_table.lua"
	print [[3) lua dofile to execute...]]
	print([[------------------------------]]..'--')
end

TSimpleList = {}

function TSimpleList:create(o)
	local o = o or { head = nil, tail = nil }
	setmetatable(o, self)
	self.__index = self
	return o
end

function TSimpleList:append(new_name)
	if not self.head then
		self.head = {name = new_name, next = nil}
		self.tail = self.head
	else
		self.tail.next = {name = new_name, next = nil}
		self.tail = self.tail.next
	end

end

function TSimpleList:print_all()
	local node = self.head
	local idx = 0

	while node do 
		idx = idx + 1
		io.write(idx)
		io.write(") ")
		print(node.name)
		node = node.next
	end
end

--尾调用???
function TSimpleList:reverse(pre_node, cur_node)
	--empty list
	if cur_node == nil then return cur_node end
	if cur_node.next == nil then
		--tail node
		cur_node.next = pre_node
		return cur_node
	else
		local first_node = nil
		if cur_node == self.head then
			self.tail = cur_node
		end
		first_node = self:reverse(cur_node, cur_node.next)
		cur_node.next = pre_node
		return first_node
	end
end

--main
print_usage()

local sl = TSimpleList:create()
sl:append("zhangsan")
sl:append("lisi")
sl:append("wangwu")
sl:append("kangshifu")
sl:append("lilei")
sl:append("hanmeimei")
sl:append("watchdog")

print "\n--reversed before:"
io.write 'head:'
print(sl.head.name)
io.write 'tail:'
print(sl.tail.name)
sl:print_all()

sl.head = sl:reverse(nil, sl.head)

print "\n--reversed result:"
io.write 'head:'
print(sl.head.name)
io.write 'tail:'
print(sl.tail.name)
sl:print_all()

--output
--➜ lua 02_simple_list.lua 
--1) lua 01_print_table.lua
--2) 01_print_table.lua
--3) lua dofile to execute...
----------------------------------
--
----reversed before:
--head:zhangsan
--tail:watchdog
--1) zhangsan
--2) lisi
--3) wangwu
--4) kangshifu
--5) lilei
--6) hanmeimei
--7) watchdog
--
----reversed result:
--head:watchdog
--tail:zhangsan
--1) watchdog
--2) hanmeimei
--3) lilei
--4) kangshifu
--5) wangwu
--6) lisi
--7) zhangsan
