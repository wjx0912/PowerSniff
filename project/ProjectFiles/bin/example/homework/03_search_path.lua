#!/usr/bin/env lua                                                                              
--Homework
--Powered by Bob Wang
--Wangbo1982@nibirutech.com
--Lua 5.2 on MacxOS                                                                             --
------print usage
function print_usage()
	print '1) lua 01_print_table.lua'
	print "2) 01_print_table.lua"
	print [[3) lua dofile to execute...]]
	print([[------------------------------]]..'--')
end

function create_map(count)
	local map = {}

	for idx = 1,count do 
		map[idx] = {}
		for idy = 1, count do 
			math.randomseed(os.time() + idx * idy)
			map[idx][idy] = math.ceil(math.random() * 100000) % 3
		end
	end

	return map
end

--print map matrix
function print_map(start_point, map, count)
	for idx = 1, count do 
		for idy = 1, count do 
			if start_point.x == idx and start_point.y == idy then
				map[idx][idy] = '@'
			end
			io.write("  "..map[idx][idy])	
		end
		print('')
	end
end

function print_search_path(start_point, map, count)
	--start position
	local start_idx = start_point.x	
	local start_idy = start_point.y
	local cur_x = start_idx
	local cur_y = start_idy

	print("start point: "..start_idx..", "..start_idy)

	while start_idx <= g_count and start_idy <= g_count do 
		local near_path = {
			{start_idx, start_idy - 1},
			{start_idx, start_idy + 1},
			{start_idx - 1, start_idy},
			{start_idx + 1, start_idy}
		}
		
		local found_way = false

		for k, v in pairs(near_path) do 
			local x = v[1]
			local y = v[2]

			print("try x,y = "..x..","..y)

			if x > 0 and y > 0 and map[x][y] ~= -1 then 
				if not found_way and (map[x][y] == g_path_gem or map[x][y] == g_path_ok) then
					cur_x,cur_y = x,y
					map[x][y] = '$'
					found_way = true
					print("\tgo to "..x..","..y)
				elseif map[x][y] ~= '$' and map[x][y] ~= '@' then
					map[x][y] = -1
				end
			end
		end

		if not found_way then
			print "game over"
			break
		end

		start_idx, start_idy = cur_x, cur_y
	end
end

function print_flags()
	print "  0 -- OK"
	print "  1 -- Gems"
	print "  2 -- Super Killer"
	print "--------------------------------"
end

--main
print_usage()

g_count = 10
g_path_ok = 0
g_path_gems = 1
g_path_killer = 2

math.randomseed(os.time())
start_point = {x = math.random(1, g_count / 2), y = math.random(1, g_count / 2)}

print_flags()
g_map = create_map(g_count)
print_map(start_point, g_map, g_count)
print_search_path(start_point, g_map, g_count)
print_map(start_point, g_map, g_count)

--➜ lua 03_search_path.lua
--1) lua 01_print_table.lua
--2) 01_print_table.lua
--3) lua dofile to execute...
----------------------------------
  --0 -- OK
  --1 -- Gems
  --2 -- Super Killer
----------------------------------
  --1  2  1  0  1  0  1  0  2  1
  --2  0  0  0  1  0  1  1  2  1
  --1  0  2  0  @  2  0  0  2  0
  --0  0  0  1  1  0  0  1  1  2
  --1  1  0  1  1  0  2  2  1  0
  --0  0  2  0  0  1  0  0  1  2
  --1  1  0  0  2  0  2  1  1  0
  --0  1  0  1  2  0  1  0  1  2
  --2  2  2  1  1  1  1  1  0  0
  --1  1  0  2  0  2  0  2  0  2
--start point: 3, 5
--try x,y = 3,4
	--go to 3,4
--try x,y = 3,6
--try x,y = 2,5
--try x,y = 4,5
--try x,y = 3,3
--try x,y = 3,5
--try x,y = 2,4
	--go to 2,4
--try x,y = 4,4
--try x,y = 2,3
	--go to 2,3
--try x,y = 2,5
--try x,y = 1,4
--try x,y = 3,4
--try x,y = 2,2
	--go to 2,2
--try x,y = 2,4
--try x,y = 1,3
--try x,y = 3,3
--try x,y = 2,1
--try x,y = 2,3
--try x,y = 1,2
--try x,y = 3,2
	--go to 3,2
--try x,y = 3,1
--try x,y = 3,3
--try x,y = 2,2
--try x,y = 4,2
	--go to 4,2
--try x,y = 4,1
	--go to 4,1
--try x,y = 4,3
--try x,y = 3,2
--try x,y = 5,2
--try x,y = 4,0
--try x,y = 4,2
--try x,y = 3,1
--try x,y = 5,1
--game over
  --1  -1  -1  -1  1  0  1  0  2  1
  ---1  $  $  $  -1  0  1  1  2  1
  ---1  $  -1  $  @  -1  0  0  2  0
  --$  $  -1  -1  -1  0  0  1  1  2
  ---1  -1  0  1  1  0  2  2  1  0
  --0  0  2  0  0  1  0  0  1  2
  --1  1  0  0  2  0  2  1  1  0
  --0  1  0  1  2  0  1  0  1  2
  --2  2  2  1  1  1  1  1  0  0
  --1  1  0  2  0  2  0  2  0  2
