"""
find_first_larger(L, e): the first larger element in a sortet input_list L
using binary search.
unique(L): Implement list(set(L)) using binary search
"""

def find_first_larger(input_list, element):
    """
    Takes an input_list and a element and returns the index
    of the first larger element.
    """

    if not input_list:
        return 0

    # Define boundaries of the array
    high = len(input_list) - 1
    low = 0

    # Loop until the boundaries cross eachother
    # which means there is no larger number
    while high >= low:
        # Find the middle index of the array
        mid = (high + low) // 2

        # Redefine new boundaries depending on the size of the middle element
        if input_list[mid] > element:
            if mid == 0 or input_list[mid - 1] <= element:
                # If mid is bigger than the element and the element before mid
                # is not, return mid
                return mid

            high = mid - 1

        else:
            low = mid + 1

    return len(input_list)


def unique(input_list):
    """
    Takes an input_list and uses the find_first_larger()
    to return the input_list without duplicates
    """

    if not input_list:
        return 0

    # Create new list containing the first/smallest entry of the input_list
    index = 0
    list_sorted = [input_list[0]]

    # Loop until the biggest element in input_list is reached
    while index < len(input_list) - 1:
        # Use find_first_larger() to find next biggest element
        next_index = find_first_larger(input_list, input_list[index])

        # Make sure index progresses
        next_index = max(index + 1, next_index)

        if next_index >= len(input_list):
            break

        # Append next biggest element to sorted list
        list_sorted.append(input_list[next_index])
        index = next_index

    return list_sorted

# def unique(L):
    # return input_list(set(L))
