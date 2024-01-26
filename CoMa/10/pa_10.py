def matrix_to_string(matrix):
    '''
    Takes any matrix in 2D array format and returns it as a string
    '''
    row_strings = []
    for row in matrix:
        row_string = ' '.join(str(int(round(elem, 0))) for elem in row)
        row_strings.append(row_string)
    # "convert" floats to ints
    return ', '.join(row_strings)

def string_to_matrix(A):
    '''
    Takes any matrix A in string format and converts it to a 2D array
    '''
    rows = A.split(',')
    matrice = [list(map(int, row.split())) for row in rows]
    return(matrice)
    
def LU_decomposition(A):
    '''
    Takes a nxn matrix and returns the upper and lower triangle matrix
    '''
    A = string_to_matrix(A)
    
    # m rows and n columns
    n = len(A)
    
    # define L as id matrix and U normally
    L = [[0] * n for _ in range(n)]
    U = [[0] * n for _ in range(n)]
    result = [[0] * n for _ in range(n)]

    for i in range(n):
        L[i][i] = 1.0
    
    for i in range(n):
        # calculation for U
        for k in range(i, n):
            # define sum for calculations
            sum = 0
            
            for j in range(i):
                sum += L[i][j] * U[j][k]
            
            # Set U matrix entry
            U[i][k] = A[i][k] - sum
            
        # calculation for L
        for k in range(i + 1, n):
            # define sum for calculations
            sum = 0
            
            for j in range(i):
                sum += L[k][j] * U[j][i]
            
            # Set U matrix entry
            L[k][i] = (A[k][i] - sum) / U[i][i]
    
    # fill result matrix
    for i in range(n):
        for j in range(n):
            if j == i:
                result[i][j] = U[i][j]
            else:
                result[i][j] = U[i][j] + L[i][j]
                
            
    return matrix_to_string(result) 


def solve_LGS(A, B):
    '''
    Solves the equation Ax=B for any matrix pair A (nxn) and B (nxm)
    '''
    A = string_to_matrix(A)
    B = string_to_matrix(B)
    
    n_A = len(A)
    
    m_B = len(B[0])
    
    for i in range(n_A):
        diagonal_value = A[i][i]
        
        # adjust A
        for j in range(n_A):
            A[i][j] /= diagonal_value
        # adjust B
        for j in range(m_B):
            B[i][j] /= diagonal_value
        
        # subract all other row values
        for k in range(n_A):
            if i != k:
                temp_sub = A[k][i]
                for p in range(n_A):
                    A[k][p] -= temp_sub * A[i][p]
                for p in range(m_B):
                    B[k][p] -= temp_sub * B[i][p]
        
    return matrix_to_string(B)
    
print(solve_LGS('-5 1 -3 -3, -15 5 -13 -11, 15 -13 27 21, 20 -6 22 3','4 -1, 40 -9, -150 17, -35 83'))