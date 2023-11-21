def convert_to_standard(P, Q):
    lower_left_x = min(P[0], Q[0])
    lower_left_y = min(P[1], Q[1])

    upper_right_x = max(P[0], Q[0])
    upper_right_y = max(P[1], Q[1])

    normal_P = (lower_left_x, lower_left_y)
    normal_Q = (upper_right_x, upper_right_y)

    return normal_P, normal_Q


def intersects(P, Q, T):
    return not (Q[0] < 0 or P[0] > T[0] or Q[1] < 0 or P[1] > T[1])


def get_delta_x1(p1, q1, t1):
    start = max(0, min(p1, q1))
    end = min(t1, max(p1, q1))
    return end - start


def get_delta_x2(p2, q2, t2):
    start = max(0, min(p2, q2))
    end = min(t2, max(p2, q2))
    return end - start


def get_lattice_point_number(P, Q, T):
    if T[0] < 0 or T[1] < 0:
        return "Die Eingabe ist fehlerhaft."

    if not intersects(P, Q, T):
        return "Der Schnitt der gegebenen Rechtecke ist leer."

    lower_left_x = max(0, min(P[0], Q[0]))
    lower_left_y = max(0, min(P[1], Q[1]))
    upper_right_x = min(T[0], max(P[0], Q[0]))
    upper_right_y = min(T[1], max(P[1], Q[1]))

    width = upper_right_x - lower_left_x + 1
    height = upper_right_y - lower_left_y + 1

    if width <= 0 or height <= 0:
        return "Der Schnitt der gegebenen Rechtecke ist leer."

    return f"Die Anzahl der Gitterpunkte im Rechteck betraegt {width * height}."


# example1
P, Q, T = (2, -1), (5, 8), (6, 4)

print(get_lattice_point_number(P, Q, T))
P, Q = convert_to_standard(P, Q)
print(P, Q)

print(intersects(P, Q, T))

print(get_delta_x1(P[0], Q[0], T[0]))
print(get_delta_x2(P[1], Q[1], T[1]))
