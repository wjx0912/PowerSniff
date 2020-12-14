--module("base64", package.seeall)

function tcp_ip_get_src_port(data)
	return data:byte(35) * 256 + data:byte(36)
end

function tcp_ip_get_dst_port(data)
	return data:byte(37) * 256 + data:byte(38)
end
