print "enter a number:"
n = assert(io.read("*number"), "invalid input")

n = io.read()
assert(tonumber(n), "invalid input: " .. n .. " is not a number")

--不用assert
local file, msg
repeat
  print "enter a file name:"
  local name = io.read()
  if not name then return end -- no input
  file, msg = io.open(name, "r")
  if not file then print(msg) end
until file

--使用assert
file = assert(io.open(name, "r"))
