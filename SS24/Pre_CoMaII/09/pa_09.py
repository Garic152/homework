def convert(A):
    rows = A.split(',')
    matrice = [list(map(int, row.split())) for row in rows]
    return(matrice)

def multiply(A, B):
    a = convert(A)
    b = convert(B)

    if len(a[0]) != len(b):
        return 'Error'
    
    result_matrix = [[min(a[i][k] + b[k][j] for k in range(len(b))) for j in range(len(b[0]))] for i in range(len(a))]
    
    return(', '.join([' '.join(map(str, list)) for list in result_matrix]))
    

def power(A, n):
    result = A
  
    for _ in range(1, n):
        result = multiply(result, A)
    return result

# A = '4 3, 1 7'
# B = '2 5 9, 8 6 1'

# print(multiply(A, B))
# print(power(A, 3))