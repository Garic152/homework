def convert_to_standard(P, Q):
    lower_left_x = min(P[0], Q[0])
    lower_left_y = min(P[1], Q[1])

    upper_right_x = max(P[0], Q[0])
    upper_right_y = max(P[1], Q[1])

    normal_P = (lower_left_x, lower_left_y)
    normal_Q = (upper_right_x, upper_right_y)

    return normal_P, normal_Q


def intersects(P, Q, T):
    return Q[0] >= 0 and P[0] <= T[0] and Q[1] >= 0 and P[1] <= T[1]


def get_delta_x1(p1, q1, t1):
    count = 0

    for i in range(min(p1, q1), max(p1, q1)):
        if i >= 0 and i < t1:
            count += 1

    return count


def get_delta_x2(p2, q2, t2):
    count = 0

    for i in range(min(p2, q2), max(p2, q2)):
        if i >= 0 and i < t2:
            count += 1

    return count


def get_lattice_point_number(P, Q, T):
    if T[0] < 0 or T[1] < 0:
        return ("Die Eingabe ist fehlerhaft.")

    lattice_points = 0

    for row in range(P[0], Q[0] + 1):
        for entry in range(P[1], Q[1] + 1):
            current_coord = (row, entry)

            if (current_coord[0] >= 0 and current_coord[1] >= 0) and (current_coord[0] <= T[0] and current_coord[1] <= T[1]):
                lattice_points += 1

    if lattice_points > 0:
        return ("Die Anzahl der Gitterpunkte im Rechteck betraegt {}.".format(lattice_points))
    elif lattice_points == 0:
        return ("Der Schnitt der gegebenen Rechtecke ist leer.")
    else:
        print("Invalid lattice_point number: " + str(lattice_points))


# example1
P = (2, -1)
Q = (5, 8)
T = (6, 4)

print(get_lattice_point_number(P, Q, T))
P, Q = convert_to_standard(P, Q)
print(P, Q)

print(intersects(P, Q, T))

print(get_delta_x1(P[0], Q[0], T[0]))
print(get_delta_x2(P[1], Q[1], T[1]))
