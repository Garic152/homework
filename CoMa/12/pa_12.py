'''
Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
'''

def maxunimod(L):
    '''
    Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
    '''
    down = False
    edge = False

    length = 0
    count = 0
    equal_count = 0

    for i in range(1, len(L)):
        # just increase count if number is same and check for edge cases
        if L[i] == L[i - 1]:
            equal_count += 1
            count += 1
            if (i == 1) or (i == len(L) - 1) and not edge:
                equal_count -= 2
                edge = True
                count += 1

        # check if number goes up
        elif L[i] > L[i - 1]:
            # if number went down previously, start new cycle
            if down:
                down = False
                if count > length:
                    length = count
                count = 1

            # increase counter by 1
            count += 1
            
            if L[i - 1] < L[i - 2]:
                count += equal_count
                equal_count = 0

            if (i == 1) or (i == len(L) - 1) and not edge:
                edge = True
                count += 1

        # check if number goes down
        elif L[i] < L[i - 1]:
            down = True
            count += 1

            if (i == 1) or (i == len(L) - 1) and not edge:
                edge = True
                count += 1
            
            if L[i - 1] > L[i - 2]:
                count += equal_count
                equal_count = 0

        # account for decreasing numbers at the end of the array
        if i == len(L) - 1:
            if count > length:
                length = count
    return length