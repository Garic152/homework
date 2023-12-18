import pytest
from pa_07 import unique, find_first_larger

# Test cases for the unique function


@pytest.mark.parametrize("input_list, expected_output", [
    ([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], [1, 3, 5, 7, 9, 11, 13, 15, 17, 19]),
    ([1, 1, 1, 1, 1, 1, 1, 1, 1, 1], [1]),
    ([1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 6], [1, 2, 3, 4, 5, 6])
])
def test_unique(input_list, expected_output):
    assert unique(input_list) == expected_output

# Test cases for the find_first_larger function


@pytest.mark.parametrize("input_list, x, expected_index", [
    ([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], 0, 0),
    ([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], 10, 5),
    ([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], 20, 10)
])
def test_find_first_larger(input_list, x, expected_index):
    assert find_first_larger(input_list, x) == expected_index
