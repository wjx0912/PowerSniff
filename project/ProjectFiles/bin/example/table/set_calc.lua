Set = {}
Set.mt = {} -- metatable for sets 

function Set.new (t)
  local set = {}

  -- important
  setmetatable(set, Set.mt)

  for _, l in ipairs(t) do set[l] = true end
  return Set 
end 

function Set.union (a,b)
  local res = Set.new{}
  for k in pairs(a) do res[k] = true end
  for k in pairs(b) do res[k] = true end
  return res
end 

function Set.intersection (a,b)
  local res = Set.new{}
  for k in pairs(a) do
    res[k] = b[k]
  end
  return res
end 

function Set.tostring (set)
  local s = "{"
  local sep = ""
  for e in pairs(set) do
    s = s .. sep .. e
    sep = ", "
  end
  return s .. "}"
end 

function Set.print (s)
  print(Set.tostring(s))
end 

s1 = Set.new{10, 20, 30, 50}
s2 = Set.new{30, 1} 

--共享相同的metatable
print(getmetatable(s1)) 
print(getmetatable(s2)) 

Set.mt.__add = Set.union
s3 = s1 + s2
Set.print(s3) --> {1, 10, 20, 30, 50} 

Set.mt.__mul = Set.intersection
Set.print((s1 + s2)*s1) --> {10, 20, 30, 50} 

--小于等于
Set.mt.__le = function (a,b) -- set containment
  for k in pairs(a) do
    if not b[k] then return false end
  end
  return true
end

--小于
Set.mt.__lt = function (a,b)
  return a <= b and not (b <= a)
end 

--相等
Set.mt.__eq = function (a,b)
  return a <= b and b <= a
end 

s1 = Set.new{2, 4}
s2 = Set.new{4, 10, 2}

print(s1 <= s2) --> true
print(s1 < s2) --> true
print(s1 >= s1) --> true
print(s1 > s1) --> false
print(s1 == s2 * s1) --> true 

--保护不被修改
Set.mt.__metatable = "not your business"
s1 = Set.new{}
print(getmetatable(s1)) --> not your business
setmetatable(s1, {})
--stdin:1: cannot change protected metatable 
