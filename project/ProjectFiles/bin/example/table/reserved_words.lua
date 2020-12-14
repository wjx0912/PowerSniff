reserved = {
  ["while"] = true, ["end"] = true,
  ["function"] = true, ["local"] = true,
}

for w in allwords() do
  if reserved[w] then
    -- `w' is a reserved word
    -- ... 
    --
  end 
end 

function Set (list)
  local set = {}
  for _, l in ipairs(list) do set[l] = true end
  return set
end
reserved = Set{"while", "end", "function", "local", }
