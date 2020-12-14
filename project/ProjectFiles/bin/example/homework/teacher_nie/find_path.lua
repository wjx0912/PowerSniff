
map = {
	{ 0,	0,	0,	0,	0,	1,	0,	0,	0,	3, },
	{ 0,	1,	1,	1,	0,	1,	0,	0,	1,	0, },
	{ 0,	1,	0,	0,	0,	1,	0,	0,	1,	0, },
	{ 0,	1,	0,	0,	0,	1,	0,	0,	1,	0, },
	{ 0,	1,	0,	1,	0,	1,	0,	0,	1,	0, },
	{ 0,	1,	0,	1,	0,	1,	0,	0,	1,	0, },
	{ 2,	1,	0,	1,	0,	1,	0,	0,	1,	0, },
	{ 0,	1,	0,	1,	0,	0,	0,	0,	0,	0, },
	{ 0,	1,	0,	1,	0,	0,	0,	0,	0,	0, },
	{ 0,	0,	0,	1,	0,	0,	0,	0,	0,	0, },
}


function getStartPoint(m)
	for y, line in ipairs(m) do
		for x, v in ipairs(line) do
			if v == 2 then
				return x, y
			end
		end
	end
end

function printTable(m)
	io.write("\27[2J")
	for y, line in ipairs(m) do
		for x, v in ipairs(line) do
			local str = "\27[%d;%dH"..v
			io.write(string.format(str, y, x))
		end
	end
	io.flush()
end

local hasFound = false

function pathFinding(m, x, y, depth)
	local depth = depth or 0
	if hasFound then
		return
	end
	
	if m[y] == nil or m[y][x] == nil or m[y][x] == 1 or m[y][x] == 4 then
		return
	elseif m[y][x] == 3 then
		io.write("\27[11;1HPath found!\n")
		hasFound = true
	elseif m[y][x] == 0 or m[y][x] == 2 then
		io.write(string.format("\27[%d;%dH4", y, x))
		io.flush()
		os.execute('sleep 1')
		m[y][x] = 4
		pathFinding(m, x + 1, y, depth + 1)
		pathFinding(m, x - 1, y, depth + 1)
		pathFinding(m, x, y + 1, depth + 1)
		pathFinding(m, x, y - 1, depth + 1)
	end
	
	if depth == 0 then
		io.write(string.format("\27[11;1HNo path found!\n"))
	end
end
io.write("\27[?25l")
printTable(map)
pathFinding(map, getStartPoint(map))
io.write("\27[?25h")
