
function printTab(n)
	for i = 1, n do
		io.write('\t')
	end
end

function printKey(k)
	if type(k) == 'string' then
		io.write('["'..k..'"]')
	elseif type(k) == 'number' then
		io.write('['..k..']')
	elseif type(k) == 'boolean' then
		io.write('['..((k and 'true') or 'false')..']')
	else
		error('unkown key type!')
	end
end

function printValue(v)
	if type(v) == 'string' then
		io.write('"'..v..'"')
	elseif type(v) == 'number' then
		io.write(v)
	elseif type(v) == 'boolean' then
		io.write((v and 'true') or 'false')
	else
		error('unkown value type!')
	end
end


function printTable(t, depth)
	local depth = depth or 1
	io.write('{\n')
	
	for k, v in pairs(t) do
		printTab(depth)
		printKey(k)
		
		io.write(' = ')
		
		if type(v) == 'table' then
			printTable(v, depth + 1)
		else
			printValue(v)
		end
		io.write(',\n')
	end
	
	printTab(depth-1)
	io.write('}')
	
end

function outputTable(t, name)
	io.write(name..' = ')
	printTable(t)
end

t = {
	['abc def'] = 1,
	2,
	d1 = 3,
	d2 = 'right',
	['s s'] = {
		3,
		sd1 = 2,
		sd2 = 'wrong',
	},
	[true] = 1,
	canDelete = false,
}

outputTable(t, 't')

