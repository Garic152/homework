import pytest
from pa_12 import maxunimod  # Assuming the function is in the pa_12 module

# Define the matrices and expected results
matrices =  [[4,5,3,2,1,3,6,4,7],
             [4,5,3,2,1,3,6,4,7],
             [10,9,8,10,6,5,4,3,2,3],
             [10,9,8,7,6,5,4,3,2,3],
             [10,9,8,7,6,5,4,3,2,1],
             [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
             [7, 2, 3, 0, 5, 4, 6, 5, 5, 9, 5, 1, 0, 5, 7, 8, 1, 7, 8, 3, 4, 8, 1, 4, 3, 3, 1, 9, 0, 0],
             [4, 2, 2, 2, 1],
             [3, 3, 3, 4, 2],
             [1, 1, 9, 9, 0, 5, 2, 7, 2, 9],
             [2, 2, 7, 0, 3, 8, 1, 8, 8, 1, 9, 6, 6, 8, 7]]

result = [5, 5, 7, 9, 10, 10, 6, 5, 5, 5, 4]

# Test each matrix
@pytest.mark.parametrize("matrix, expected_result", zip(matrices, result))
def test_maxunimod(matrix, expected_result):
    assert maxunimod(matrix) == expected_result
