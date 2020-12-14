list = nil
for line in io.lines() do
  list = {next=list, value=line}
end

l = list
while l do
  print(l.value)
  l = l.next
end
