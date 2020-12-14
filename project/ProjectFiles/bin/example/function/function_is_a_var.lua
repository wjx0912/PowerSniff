a = {p = print}
a.p("Hello World") --> Hello World
print = math.sin -- `print' now refers to the sine function
a.p(print(1)) --> 0.841470
sin = a.p -- `sin' now refers to the print function
sin(10, 20) --> 10 20 

function foo (x) return 2*x end 
foo = function (x) return 2*x end 


