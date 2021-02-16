u <- 3
si <- .5
n <- 100
s <- si / sqrt(n)
pnorm(3.06, u, s)

1 - pnorm(3.06, u, s)

u <- 50
si <- 1
n <- 100
s <- si / sqrt(n)
pnorm(u + .25, u, s) - pnorm(u - .25, u, s)
