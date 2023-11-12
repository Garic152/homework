def convert_to_standart(P, Q):
    lower_left_x = min(P[0], Q[0])
    lower_left_y = min(P[1], Q[1])

    upper_right_x = max(P[0], Q[0])
    upper_right_y = max(P[1], Q[1])

    normal_P = (lower_left_x, lower_left_y)
    normal_Q = (upper_right_x, upper_right_y)

    return normal_P, normal_Q


def intersects(P, Q, T):
    pass


def get_delta_x1(p1, q1, t1):
    pass


def get_delta_x2(p2, q2, t2):
    pass


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
        return ("Die Anzahl der Gitterpunkte im Rechteck betraegt " + str(lattice_points) + ".")
    elif lattice_points == 0:
        return ("Der Schnitt der gegebenen Rechtecke ist leer.")
    else:
        print("Invalid lattice_point number: " + str(lattice_points))


# example
P = (-6, -4)
Q = (2, 1)
T = (6, 5)

answer = get_lattice_point_number(P, Q, T)
print(answer + "\n")

P = (3, 5)
Q = (-3, 7)
T = (6, 5)

P, Q = convert_to_standart(P, Q)
print(P, Q)

print(intersects(P, Q, T))
