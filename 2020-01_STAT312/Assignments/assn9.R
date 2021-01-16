px = c(.79, .09, .05, .04, .03)
x = 0:4
sum(px * x)

f <- function(x) {
  return(0.1875 * (4 - x^2))
}
integrate(function(x) f(x) * x, 0, 2)$value / 2

integrate(f, 0.5, 2)$value

median(c(1.9, 0.6, 1.1, 0.5, 0.6, 1.2, 0.6))
