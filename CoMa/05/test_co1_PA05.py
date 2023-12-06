import pytest
from co1_PA05 import get_linedistance, get_optimal_line, distance_to_opt


def test_linedistance():
    points = [(-2, 2), (1, 3), (2, 2), (6, -2)]
    line = [(1, 1), (-1, 4)]

    d0 = get_linedistance(points, line[0])
    d1 = get_linedistance(points, line[1])

    assert d0 == 92
    assert d1 == 16


def test_optimal_line():
    points = [(-2, 2), (1, 3), (2, 2), (6, -2)]
    (a, b) = get_optimal_line(points)

    assert a == pytest.approx(-0.5419847328244275, rel=1e-4)
    assert b == pytest.approx(2.198473282442748, rel=1e-4)


def test_distance_to_optimal_line():
    points = [(-2, 2), (1, 3), (2, 2), (6, -2)]
    a, b = get_optimal_line(points)

    distance = get_linedistance(points, (a, b))
    assert distance == pytest.approx(5.129770992366414, rel=1e-4)


def test_distance_to_opt():
    points = [(-2, 2), (1, 3), (2, 2), (6, -2)]
    line = [(1, 1), (-1, 4)]

    distance = distance_to_opt(points, line)
    assert distance == pytest.approx(10.870229007633586, rel=1e-4)
