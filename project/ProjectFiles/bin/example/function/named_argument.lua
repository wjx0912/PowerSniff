-- invalid code
-- rename(old="temp.lua", new="temp1.lua") 

--valid
--rename{old="temp.lua", new="temp1.lua"}
function rename (arg)
  return os.rename(arg.old, arg.new)
end 
