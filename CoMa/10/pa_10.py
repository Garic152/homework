def matrix_to_string(matrix):
    '''
    Takes any matrix in 2D array format and returns it as a string
    '''
    return [list(map(float, row.split())) for row in matrix.split(",")]

def string_to_matrix(A):
    '''
    Takes any matrix A in string format and converts it to a 2D array
    '''
    rows = A.split(',')
    matrice = [list(map(int, row.split())) for row in rows]
    return(matrice)

def LU_decomposition(A):

    A = string_to_matrix(A)

    n = len(A)

    L = [[0.0] * n for _ in range(n)]
    U = [[0.0] * n for _ in range(n)]
    result = [[0.0] * n for _ in range(n)]

    for j in range(n):
        L[j][j] = 1.0
        for i in range(j+1):
            s1 = sum(U[k][j] * L[i][k] for k in range(i))
            U[i][j] = A[i][j] - s1

        for i in range(j, n):
            s2 = sum(U[k][j] * L[i][k] for k in range(j))
            L[i][j] = (A[i][j] - s2) / U[j][j]
    
    for i in range(n):
        for j in range(n):
            if j == i:
                result[i][j] = U[i][j]
            else:
                result[i][j] = U[i][j] + L[i][j]

    return(', '.join([' '.join(map(lambda x: str(int(round(x))), list)) for list in result]))


def solve_LGS(A, B):
    A = LU_decomposition(A)
    B = string_to_matrix(B)
    A = string_to_matrix(A)

    n = len(A)

    L = [[0.0] * n for _ in range(n)]
    U = [[0.0] * n for _ in range(n)]

    for i in range(n):
        for j in range(n):
            if j >= i:
                U[i][j] = A[i][j]
            else:
                L[i][j] = A[i][j]

    solutions = [[] for _ in range(n)]

    for b in zip(*B):
        y = [0 for _ in range(n)]
        for i in range(n):
            y[i] = b[i] - sum(L[i][j] * y[j] for j in range(i))

        x = [0 for _ in range(n)]
        for i in range(n - 1, -1, -1):
            x[i] = (y[i] - sum(U[i][j] * x[j] for j in range(i + 1, n))) / U[i][i]

        for i in range(n):
            solutions[i].append(x[i])

    return(', '.join([' '.join(map(lambda x: str(int(round(x))), list)) for list in solutions]))
    
print(solve_LGS('2 4 -7, -4 -7 13, 34 71 -131','-1 7, 2 -18, -26 116'))