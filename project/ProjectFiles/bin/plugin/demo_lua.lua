-- file encode must be UTF8
-- sdk function:
-- init()		
-- handle_data()	
-- handle_click()	
-- handle_double()	

function init()
	trace("plugin init:  ".._VERSION.."\n")
	trace("package path: "..package.path.."\n")
	trace("package path: "..package.cpath.."\n")
	--for k,v in pairs(_G) do
	--	trace(string.format("%s,%s\n", k, v))
	--end
end

--protocol:	a string, one of: tcp,udp,icmp
--data:		binary data
--len_total:	total data len
--len_data:	valid data len, exclusive protocol head
function handle_data(protocol,data,len_total,len_data)
	-- if 54 == len_total then
	if len_total > 0 and len_total < 100 then
		return "delete"		-- remove handshake
	end
end

function handle_click(protocol,data,len_total,len_data)
	trace("len_total: "..len_total.."\n")
end

function handle_double(protocol,data,len_total,len_data)
	plugin_output_clear()
	plugin_output("protocol: "..protocol.."\n")
	plugin_output("len_total: "..len_total.."\n")
	plugin_output("len_data: "..len_data.."\n")
	plugin_output("================\n")
	for i=len_total-len_data+1,len_total do
		plugin_output(string.format("%02X", data:byte(i)).." ")
		if 0 == i % 16 then
			plugin_output("\n")
		end
	end
	plugin_output("\n")
	plugin_output("================\n\n")
	plugin_summary("handle_double test summary")
	
	-- 下面是trace test
	--[[
	print("x is " .. x)
	print("y is " .. y)
	trace_raw("\nhandle_double(), data len: "..data:len().."\n")
	trace("================\n")
	--for j=1,10 do
		for i=1,len_total do
			--trace(i.." value: "..string.byte(data,i).."\n")
			--trace(i.." value: "..data:byte(i).."\n")
			trace_raw(string.format("%02X", data:byte(i)).." ")
			if 0 == i % 16 then
				trace_raw("\n")
			end
		end
		trace_raw("\n")
	--end
	trace("================\n\n")
	plugin_output_clear()
	plugin_output("test plugin handle_double\n")
	return x+y
	]]--
end

