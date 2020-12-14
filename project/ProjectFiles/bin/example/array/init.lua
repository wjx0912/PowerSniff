a = {} -- new array
for i=1, 1000 do
  a[i] = 0
end 

-- creates an array with indices from -5 to 5
a = {}
for i=-5, 5 do
  a[i] = 0
end

squares = {1, 4, 9, 16, 25, 36, 49, 64, 81}

--n * m matrix
mt = {} -- create the matrix
for i=1,N do
  mt[i] = {} -- create a new row
  for j=1,M do
    mt[i][j] = 0
  end
end

mt = {} -- create the matrix
for i=1,N do
  for j=1,M do
    mt[i*M + j] = 0
  end
end
