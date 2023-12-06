def get_optimal_line(points):
    # calculate average
    x_average = 0
    y_average = 0
    count = 0

    for point in points:
        x_average += point[0]
        y_average += point[1]
        count += 1

    x_average /= count
    y_average /= count

    # calculate sums
    top_sum = 0
    bottom_sum = 0

    for point in points:
        top_sum += (point[0] - x_average) * (point[1] - y_average)
        bottom_sum += (point[0] - x_average)**2

    # calculate a and b
    a = top_sum / bottom_sum

    b = y_average - a * x_average

    return a, b


def get_linedistance(points, line):
    linedistance = 0

    # calculate linedistance
    for point in points:
        linedistance += (line[0] * point[0] + line[1] - point[1])**2

    return linedistance


def distance_to_opt(points, lines):
    # calculate the optimal line
    (a, b) = get_optimal_line(points)

    distances = []

    # calculate the distance from the non optimal line and append them to the array
    for line in lines:
        distances.append(get_linedistance(points, line))

    d1 = get_linedistance(points, (a, b))

    # return the difference between the optimal line and the smallest non optimal line
    return min(distances) - d1
