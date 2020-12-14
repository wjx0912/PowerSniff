do
  local a2 = 2*a
  local d = sqrt(b^2 - 4*a*c)

  x1 = (-b + d)/a2
  x2 = (-b - d)/a2
end -- scope of 'a2' and 'd' ends here

print(x1, x2)
