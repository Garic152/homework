import pytest
from PA06 import abstand


def test_abstand_1():
    assert abstand((0, 9), (2, 2)) == 11


def test_abstand_2():
    assert abstand((0, 1), (0, 7)) == 8


def test_abstand_3():
    assert abstand((0, 9), (0, 7)) == -1
