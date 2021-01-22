import pytest
from YuTriangle import is_a_triangle, NO_TRIANGLE, SCALENE, EQUILATERAL, ISOCELES


testdata = [
    (-5, 4, 3, NO_TRIANGLE),
    (5, -4, 0, NO_TRIANGLE),
    (0, 0, 0, NO_TRIANGLE),
    (1, 2, 5, NO_TRIANGLE),
    (-3, 89, 73, NO_TRIANGLE),
    (5, 5, 5, EQUILATERAL),
    (2, 2, 2, EQUILATERAL),
    (5, 5, 1, ISOCELES),
    (5, 4, 3, SCALENE),
    (0, 5, 5, NO_TRIANGLE),
    (3, 5, 2, NO_TRIANGLE),
]


@pytest.mark.parametrize("a,b,c,expected", testdata)
def test_is_a_triangle(a, b, c, expected):
    assert expected == is_a_triangle(a, b, c)


wrongtype = [
    (5, 4, 'a'),
    ({}, 4, 2),
    (5, None, 2),
    ([], 4, []),
]


@pytest.mark.parametrize("a,b,c", wrongtype)
def test_type_error(a, b, c):
    with pytest.raises(TypeError):
        is_a_triangle(a, b, c)
