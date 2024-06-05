'''
Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
'''
def maxunimod(L):
    '''
    Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
    '''
    ranges = []

    # define start range
    start = -1
    end = 0

    # define equal count for later use
    equal_count = 0

    # define down flag
    down = False

    # if L <= 2, L must be unimodular
    if len(L) <= 2:
        return len(L)

    for i in range(len(L) - 1):
        # update range to current element
        end = i

        # set start range to 0 (maybe just delete it and define it as 0)
        if start == - 1:
            start = i

        # increase the range when elements are the same
        if L[i] == L[i + 1]:
            if down:
                # increase equal_count to accord for later starting point adjustment
                equal_count += 1

        # if next element is bigger, increase the range
        elif L[i] < L[i + 1]:
            if down:
                # "function" was previously at a turning point
                # save the length of the unimodular row and reset the starting point
                down = False
                ranges.append(end - start + 1)
                start = i - equal_count
                equal_count = 0

        # if next element is bigger, increase the range and set the down flag
        elif L[i] > L[i + 1]:
            down = True

    # account for unimodlen = L
    ranges.append(end - start + 2)

    # return longest range
    return max(ranges)