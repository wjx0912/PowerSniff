A = {
	x = 10,
	y = 20
}

function A:new( t )
	local t = t or {}
	self.__index = self
	setmetatable( t, self )
	return t
end

s = A:new()
