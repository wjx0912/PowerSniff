--迭代函数都难写易用

function all_words()
	local line = io.read() -- current line
	local pos = 1 -- current position in the line
	
	return function () -- iterator function
		while line do -- repeat while there are lines
			local s, e = string.find(line, "%w+", pos)
			
			if s then -- found a word?
				pos = e + 1 -- next position is after this word
				return string.sub(line, s, e) -- return the word
			else
				line = io.read() -- word not found; try next line
				pos = 1 -- restart from first position
			end
		end
		
		return nil -- no more lines: end of traversal
	end
end

for word in all_words() do
	print(word)
end 