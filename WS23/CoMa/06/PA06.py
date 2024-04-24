def read_labyrinth(dateiname):
    # read the file and create a matrice with all the entries
    with open(dateiname, 'r') as file:
        labyrinth = [list(line.strip()) for line in file]
    return labyrinth


def search(labyrinth, s, t):
    # define needed variables
    # set for avoiding multiple entries
    # queue for looping through new positions
    # directions array for looping through the directions to add new positions
    rows, cols = len(labyrinth), len(labyrinth[0])
    visited = set()
    queue = [(s, 0)]
    directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]

    # now loop through all the elements in queue
    while queue:
        # pick the element in the queue that got added first
        (x, y), schritte = queue.pop(0)
        if (x, y) == t:
            # found the connection to t, return the amount of steps it took
            return schritte
        if (x, y) in visited:
            continue
        # add the current field to visited
        visited.add((x, y))

        # loop through all directions
        for dx, dy in directions:
            # set new positions
            nx, ny = x + dx, y + dy
            # check if new position is on the labyrinth
            # if the new position is not a border
            # and if it is not in the visited set
            if 0 <= nx < rows and 0 <= ny < cols and labyrinth[nx][ny] != 'U' and (nx, ny) not in visited:
                # add the new position to the queue to go from there
                queue.append(((nx, ny), schritte + 1))

    # return -1 if no connection from s to t was found
    return -1


def abstand(s, t, dateiname="labyrinth.dat"):
    labyrinth = read_labyrinth(dateiname)
    return search(labyrinth, s, t)
