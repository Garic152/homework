def matrix_to_string(matrix):
    row_strings = []
    for row in matrix:
        row_string = ' '.join(str(elem) for elem in row)
        row_strings.append(row_string)
    return ', '.join(row_strings).replace(".0", "")

def string_to_matrix(A):
    rows = A.split(',')
    matrice = [list(map(int, row.split())) for row in rows]
    return(matrice)
    
def LU_decomposition(A):
    A = string_to_matrix(A)
    
    # m rows and n columns
    n = len(A)
    
    # define L as id matrix and U normally
    L = [[1 if i == j else 0 for j in range(n)] for i in range(n)]
    U = [[0] * n for _ in range(n)]
    result = [[0] * n for _ in range(n)]
    
    for i in range(n):
        # calculation for U
        for j in range(i, n):
            # define sum for calculations
            sum = 0
            
            for k in range(i):
                sum += L[i][k] * U[k][j]
            
            # Set U matrix entry
            U[i][j] = A[i][j] - sum
            
        # calculation for L
        for j in range(i, n):
            # define sum for calculations
            sum = 0
            
            for k in range(i):
                sum += L[j][k] * U[k][i]
            
            # Set U matrix entry
            L[j][i] = (A[j][i] - sum) / U[i][i]
    
    # fill result matrix
    for i in range(n):
        for j in range(n):
            if j == i:
                result[i][j] = U[i][j]
            else:
                result[i][j] = U[i][j] + L[i][j]
                
            
    return matrix_to_string(result) 


def solve_LGS(A, B):
    A = string_to_matrix(A)
    B = string_to_matrix(B)
    
    n_A = len(A)
    
    n_B = len(B)
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