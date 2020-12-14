function allwords (f)
	-- repeat for each line in the file
	
	for l in io.lines() do
		-- repeat for each word in the line
		for w in string.gfind(l, "%w+") do
			-- call the function
			f(w)
		end 
	end 
end

--打印单词
allwords(print)

--匿名函数作为参数
--打印hello出现的次数
local count = 0

allwords(
		function (w)
			if w == "hello" then count = count + 1 end
		end
		)
print(count)

--用for完成
local count = 0

for w in allwords() do
	if w == "hello" then count = count + 1 end
end
print(count)