local iterator -- to be defined later

function allwords()
	--使用table实现不如闭包的性能
	local state = {line = io.read(), pos = 1}
	return iterator, state
end

function iterator (state)
	while state.line do -- repeat while there are lines
		-- search for next word
		local s, e = string.find(state.line, "%w+", state.pos)

		if s then -- found a word?
			-- updatenext position (after this word)
			state.pos = e + 1
			return string.sub(state.line, s, e)
		else -- word not found
			state.line = io.read() -- try next line...
			state.pos = 1 -- ... from first position
		end
	end
	
	return nil -- no more lines: end loop
end