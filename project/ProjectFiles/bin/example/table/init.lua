days = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"} 
print(days[4]) --> Wednesday 

tab = {sin(1), sin(2), sin(3), sin(4),
sin(5),sin(6), sin(7), sin(8)}

a = {x=0, y=0} --> a = {}; a.x=0; a.y=0 

w = {x=0, y=0, label="console"}
x = {sin(0), sin(1), sin(2)}
w[1] = "another field"
x.f = w
print(w["x"]) --> 0
print(w[1]) --> another field
print(x.f[1]) --> another field
w.x = nil -- remove field "x"

polyline = {color="blue", thickness=2, npoints=4,
{x=0, y=0},
{x=-10, y=0},
{x=-10, y=1},
{x=0, y=1}
}

opnames = {["+"] = "add", ["-"] = "sub",
["*"] = "mul", ["/"] = "div"} 

{x=0, y=0} --> {["x"]=0, ["y"]=0}
{"red", "green", "blue"} --> {[1]="red", [2]="green", [3]="blue"}

--下标从0开始无法使用很多库函数
days = {[0]="Sunday", "Monday", "Tuesday", "Wednesday",
"Thursday", "Friday", "Saturday"}

--分割不同类型的元素
{x=10, y=45; "one", "two", "three"}
