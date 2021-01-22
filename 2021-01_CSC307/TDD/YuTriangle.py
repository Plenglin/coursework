
NO_TRIANGLE = 0
EQUILATERAL = 1
ISOCELES = 2
SCALENE = 3


def is_a_triangle(a, b, c):
    l = list(sorted((a, b, c)))
    a, b, c = l
    if any(x <= 0 for x in l) or a + b <= c:
        return NO_TRIANGLE
    return len(set(l))
