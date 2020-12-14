
data = { 1,81,7,32,7,56,12,97,3  }

function createList(d)
	local head
	for _, v in ipairs(d) do
		head = {_next = head, _data = v}
	end
	return head
end

function printList(head)
	while head do
		print(head._data)
		head = head._next
	end
end

function reverse(head)
	local ret
	while head do
		ret = {_next = ret, _data = head._data}
		head = head._next
	end
	return ret
end

printList(createList(data))
print('reverse')
printList(reverse(createList(data)))

