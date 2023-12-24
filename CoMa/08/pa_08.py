import random
import time

"""
CoMa PA08
"""


def updatePosition(rows, columns, pos, rnd):
    """
    Updating the position of a field based on a random number
    """

    new_position = 0

    if 0 <= rnd < 0.25:
        if pos % columns == columns - 1:
            new_position = pos - columns + 1
        else:
            new_position = pos + 1

    elif 0.25 <= rnd < 0.5:
        if pos % columns == 0:
            new_position = pos + columns - 1
        else:
            new_position = pos - 1

    elif 0.5 <= rnd < 0.75:
        if pos >= (rows - 1) * columns:
            new_position = pos % columns
        else:
            new_position = pos + columns

    elif 0.75 <= rnd < 1.0:
        if pos < columns:
            new_position = pos + (rows - 1) * columns
        else:
            new_position = pos - columns

    return new_position


def updatePositions(rows, columns, positions):
    """
    Updates given positions using updatePosition
    """
    for position in positions:
        position[1] = updatePosition(
            rows, columns, position[1], random.random())


def sortPositions(positions):
    """
    Sorts a given array by its individual positions
    """
    positions.sort(key=lambda x: x[1])


def extractSquare(positions):
    """
    Extracts the positions with the highest position from a given array
    """
    max_value = positions[len(positions) - 1][1]

    square = [position for position in positions if position[1] == max_value]
    positions = [
        position for position in positions if position[1] != max_value]

    return positions, square


def giftExchange(square):
    """
    Handles multiple figures on the same square
    """

    # Z, ZH, H, HH
    figures = [0, 0, 0, 0]
    key_to_index = {'Z': 0, 'ZH': 1, 'H': 2, 'HH': 3}

    for position in square:
        key = position[0]

        if key in key_to_index:
            figures[key_to_index[key]] += 1

    if figures[1] >= 1 and figures[2] >= 1:
        for position in square:
            if position[0] == 'H':
                position[0] = 'HH'

                figures[2] -= 1
                figures[3] += 1

    if figures[0] >= (2 * figures[3]):
        for position in square:
            if position[0] == 'H' or position[0] == 'HH':
                position[0] = 'Z'

    elif figures[0] < (2 * figures[3]):
        for position in square:
            if position[0] == 'Z':
                position[0] = 'ZH'


def mergeSquare(square, intermediate):
    """
    Appends items from square list to intermediate list
    """
    for element in square:
        intermediate.append(element)

    return intermediate


def christmasFated(positions):
    """
    Goes through positions and checks for only humans/only zombies
    """
    zombie = False
    human = False

    for position in positions:
        if position[0] == 'Z':
            zombie = True
        elif position[0] == 'ZH':
            pass
        else:
            human = True

    if (not human or not zombie):
        return True
    return False


def christmasFate(positions):
    """
    Goes through positions and checks for christmas fate
    """
    zombie = False
    human = False

    for position in positions:
        if position[0] == 'Z':
            zombie = True
        elif position[0] == 'ZH':
            pass
        else:
            human = True

    if not human:
        print("Zombies ate my Christmas!")
    elif human and not zombie:
        print("Ho, ho, ho, and a merry Zombie-Christmas!")


def print_board(rows, columns, positions):
    """
    Prints the whole board with characters representing different entities.
    """
    board = [['.' for _ in range(columns)] for _ in range(rows)]
    for pos in positions:
        x, y = divmod(pos[1], columns)
        if pos[0] == 'Z':
            board[x][y] = '☠'  # Zombie
        elif pos[0] == 'ZH':
            board[x][y] = '🧟'  # Zombie Elf
        elif pos[0] == 'H':
            board[x][y] = '🙂'  # Human
        else:
            board[x][y] = '🎅'

    for row in board:
        print(' '.join(row))
    print()


def zombieChristmas(rows, columns, positions):
    print_board(rows, columns, positions)

    while not christmasFated(positions):
        updatePositions(rows, columns, positions)
        sortPositions(positions)

        intermediate = []

        while positions:
            positions, square = extractSquare(positions)

            giftExchange(square)
            intermediate = mergeSquare(square, intermediate)

        positions = intermediate

        print_board(rows, columns, positions)

    return christmasFate(positions)


n, m = 5, 5

# Initialize positions
# Format: ('Type', Position Index), where Type can be 'Z', 'ZH', 'H', or 'HH'
positions = []

# Randomly assign positions to 20 players
for _ in range(5):
    # Randomly choose a type
    type_choice = random.choice(['Z', 'ZH', 'H', 'HH'])

    # Randomly choose a position
    position = random.randint(0, n * m - 1)

    # Add to positio
    positions.append([type_choice, position])

zombieChristmas(n, m, positions)
