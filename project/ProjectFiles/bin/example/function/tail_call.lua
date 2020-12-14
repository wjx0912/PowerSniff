--最后一个语句函数结束后不再返回到调用者，所以尾调用之后程序不必保留任何关于调用者的信息
--尾调用不使用额外的栈
--可以无限递归不会溢出

function f(x)
  return g(x)
end 

function foo (n)
  if n > 0 then return foo(n - 1) end
end 

--不是尾调用 
function f (x)
  g(x)
  return
end 

return g(x) + 1 -- must do the addition
return x or g(x) -- must adjust to 1 result
return (g(x)) -- must adjust to 1 result 

--是尾调用 
return x[i].foo(x[j] + a*b, i + j)
