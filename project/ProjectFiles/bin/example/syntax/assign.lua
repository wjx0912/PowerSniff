a = "hello" .. "world"
t.n = t.n + 1

a, b = 10, 2*x --> a=10; b=2*x

x, y = y, x -- swap 'x' for 'y'
a[i], a[j] = a[j], a[i] -- swap 'a[i]' for 'a[i]' 

a, b, c = 0, 1
print(a,b,c) --> 0 1 nil
a, b = a+1, b+1, b+2 -- value of b+2 is ignored
print(a,b) --> 1 2
a, b, c = 0
print(a,b,c) --> 0 nil nil

a, b, c = 0, 0, 0 
print(a,b,c) --> 0 0 0

a, b = f() --f函数返回两个值
