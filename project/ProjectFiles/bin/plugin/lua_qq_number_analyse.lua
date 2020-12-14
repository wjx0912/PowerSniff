-- file encode must be UTF8
-- qq号码登录监视脚本（不支持手机号码登录，不支持webqq，只在pc上用qq2015测试通过）
-- 2015.9.14
require "base64"
require "tcp_ip"

function init()
	trace("plugin init:  ".._VERSION.."\n")
	trace("package path: "..package.path.."\n")
	trace("package path: "..package.cpath.."\n")
	--for k,v in pairs(_G) do
	--	trace(string.format("%s,%s\n", k, v))
	--end
end

-- protocol:	字符串如tcp,udp,icmp
-- data:	二进制数据
-- len_total:	总共数据长度
-- len_data:	有效数据长度（去除各种头之后的数据）
function handle_data(protocol,data,len_total,len_data)
	if 54 == len_total then
		return "delete"		-- remove handshake
	end
	src_port = tcp_ip_get_src_port(data)
	dst_port = tcp_ip_get_dst_port(data)
	-- if 8000 != src_port && 8000 != dst_port then
	if (8000 ~= dst_port) or (len_data < 100) then
		return "delete"
	end
	if 2 ~= data:byte(43) then	-- 0x2是qq udp协议magic number
		return "delete"
	end
	if 8 ~= data:byte(46) then	-- 8和37是 0x8和0x25是协议类型，表示登录
		return "delete"
	end
	if 37 ~= data:byte(47) then
		return "delete"
	end
	-- 50, 51, 52, 53字节是qq号（lua index从1开始而不是0）
	qq_number = data:byte(50) * 256 * 256 * 256 + data:byte(51) * 256 * 256 + data:byte(52) * 256 + data:byte(53)
	plugin_summary("qq_number is: " .. qq_number)
end

function handle_click(protocol,data,len_total,len_data)
	if 54 == len_total then
		return
	end
	src_port = tcp_ip_get_src_port(data)
	dst_port = tcp_ip_get_dst_port(data)
	-- if 8000 != src_port && 8000 != dst_port then
	if (8000 ~= dst_port) or (len_data < 100) then
		return
	end
	if 2 ~= data:byte(43) then	-- 0x2是qq udp协议magic number
		return 
	end
	if 8 ~= data:byte(46) then	-- 8和37是 0x8和0x25是协议类型，表示登录
		return
	end
	if 37 ~= data:byte(47) then
		return
	end
	-- 50, 51, 52, 53字节是qq号（lua index从1开始而不是0）
	qq_number = data:byte(50) * 256 * 256 * 256 + data:byte(51) * 256 * 256 + data:byte(52) * 256 + data:byte(53)
	plugin_output_clear()
	plugin_output("qq_number is: " .. qq_number.."\n")
end

function handle_double(protocol,data,len_total,len_data)
	handle_data(protocol,data,len_total,len_data)
end
