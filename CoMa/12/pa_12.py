'''
Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
'''

def maxunimod(L):
    '''
    Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
    '''
    down = False

    length = 0
    count = 1
    equal_count = 0

    for i in range(1, len(L)):
        # just increase count if number is same
        if L[i] == L[i - 1]:
            equal_count += 1
            print("{} is equal to {}".format(L[i], L[i - 1]))
            if (i == 1) or (i == len(L) - 1):
                count += 1
                print("Adding 1 now count at", count)
            count += 1
            print("Adding 1 now count at", count)

        # check if number goes up
        elif L[i] > L[i - 1]:
            print("{} is bigger than {}".format(L[i], L[i - 1]))
            # if number went down previously, start new cycle
            if down:
                down = False
                if count > length:
                    length = count
                print("Resetting count to 1")
                count = 1

            # increase counter by 1
            count += 1
            print("Adding 1 now count at", count)

            count += equal_count
            equal_count = 0

            if (i == 1) or (i == len(L) - 1):
                count += 1
                print("Adding 1 now count at", count)

        # check if number goes down
        elif L[i] < L[i - 1]:
            print("{} is less than {}".format(L[i], L[i - 1]))
            down = True
            count += 1
            print("Adding 1 now count at", count)

            if (i == 1) or (i == len(L) - 1):
                count += 1
                print("Adding 1 now count at", count)
            
            count += equal_count
            equal_count = 0

        # account for decreasing numbers at the end of the array
        if i == len(L) - 1:
            if count > length:
                length = count
    return length

matrix = [7, 2, 3, 0, 5, 4, 6, 5, 5, 9, 5, 1, 0, 5, 7, 8, 1, 7, 8, 3, 4, 8, 1, 4, 3, 3, 1, 9, 0, 0]


print(maxunimod(matrix))