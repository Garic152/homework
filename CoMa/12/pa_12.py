'''
Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
'''

def maxunimod(L):
    '''
    Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
    '''
    down = False

    length = 0
    count = 0

    for i in range(1, len(L)):
        # check if number goes up
        if L[i] >= L[i - 1]:
            # if number went down previously, start new cycle
            if down:
                down = False
                if count > length:
                    length = count
                count = 0

            # increase counter by 1
            count += 1

        # check if number goes down
        if L[i] <= L[i - 1]:
            down = True
            count += 1

        # account for decreasing numbers at the end of the array
        if i == len(L) - 1:
            if count > length:
                length = count
    return length + 1
