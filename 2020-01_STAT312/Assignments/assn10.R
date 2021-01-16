x = 0:4
px = c(.79, .09, .05, .04, .03)
u = 0.43

s2 = sum(px * (x - u)^2)
s2
s = sqrt(s2)
s

v = c(7, 8, 11, 13)
sqrt(sum((v - mean(v))^2) / 3)

f <- function(x) 90 * x^8 * (1 - x)
u = 9/11
integrate(function(x) (x - u)^2 * f(x), 0, 1)$value * 242
