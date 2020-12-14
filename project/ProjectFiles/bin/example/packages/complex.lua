complex = {}

function complex.new (r, i) return {r=r, i=i} end

--defines a constant `i'
complex.i = complex.new(0, 1)

function complex.add (c1, c2)
  return complex.new(c1.r + c2.r, c1.i + c2.i)
end 

function complex.sub (c1, c2)
  return complex.new(c1.r - c2.r, c1.i - c2.i)
end 

--使用 
c = complex.add(complex.i, complex.new(10, 20)) 

--缺点
--1. 每个操作函数都要加包名称
--2. 包内函数互相调用要加包名 

--使用固定的局部变量名改善 
local P = {}

complex = P -- package name
P.i = {r=0, i=1}

function P.new (r, i) return {r=r, i=i} end

function P.add (c1, c2)
  return P.new(c1.r + c2.r, c1.i + c2.i)
end 

--...
--others codes here

--私有化
local P = {}
complex = P

local function checkComplex (c)
  if not ((type(c) == "table") and
    tonumber(c.r) and tonumber(c.i)) then
    error("bad complex number", 3)
  end
end

function P.add (c1, c2)
  checkComplex(c1);
  checkComplex(c2);
  return P.new(c1.r + c2.r, c1.i + c2.i)
end

--... 

return P

--继续改进 
local function checkComplex (c)
  if not ((type(c) == "table")
    and tonumber(c.r) and tonumber(c.i)) then
    error("bad complex number", 3)
  end 
end 

local function new (r, i) return {r=r, i=i} end
local function add (c1, c2)
  checkComplex(c1);
  checkComplex(c2);
  return new(c1.r + c2.r, c1.i + c2.i)
end 

--... 

complex = {
  new = new,
  add = add,
  sub = sub,
  mul = mul,
  div = div,
} 

--更爽 
local P = {}
complex = P
setfenv(1, P) 

--自动变成complex.add
function add (c1, c2)
  return new(c1.r + c2.r, c1.i + c2.i)
end 
