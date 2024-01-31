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

    for i in range(1, len(L)):
        # just increase count if number is same
        if L[i] == L[i - 1]:
            if (i == 1) or (i == len(L) - 1) and not edge:
                edge = True
                count += 1
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

        # account for decreasing numbers at the end of the array
        if i == len(L) - 1:
            if count > length:
                length = count
    return length

# matrix = [4,5,3,2,1,3,6,4,7]


# print(maxunimod(matrix))