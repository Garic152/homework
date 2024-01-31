'''
Returns the maximum length of a unimodular sequence for a given int-list L of length n ≥ 1.  
'''

def maxunimod_old(L):
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

        # check if number goes up
        elif L[i] > L[i - 1]:
            if i == 1 or i == len(L) - 1:
                count += 1
            
            # if number went down previously, start new cycle
            if down:
                down = False
                if count > length:
                    length = count
                count = 0

            # increase counter by 1
            count += 1

        # check if number goes down
        elif L[i] < L[i - 1]:
            if i == 1 or i == len(L) - 1:
                count += 1

            down = True
            count += 1

            if L[i - 1] > L[i - 2]:
                count += equal_count
                equal_count = 0

        # account for decreasing numbers at the end of the array
        if i == len(L) - 1:
            if count > length:
                length = count

    return length

def maxunimod(L):
    ranges = []
    start = -1
    end = 0
    equal_count = 0

    down = False

    if len(L) <= 2:
        return len(L)

    for i in range(len(L) - 1):
        end = i

        if start == - 1:
            start = i

        if L[i] == L[i + 1]:
            if down:
                equal_count += 1

        elif L[i] < L[i + 1]:
            if down:
                # print("BIGGER")
                down = False
                # print(list([start, end]))
                # print("Equal Count:", equal_count)
                ranges.append(end - start + 1)
                # print("Appending ", end - start + 1)
                start = i - equal_count
                equal_count = 0

        elif L[i] > L[i + 1]:
            down = True

    # print(list([start, end + 1]))
    ranges.append(end - start + 2)
    # print(end - start + 2)

    return max(ranges)

# print(maxunimod([7, 1, 0]))