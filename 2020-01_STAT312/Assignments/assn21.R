u <- 20
s <- 1.8
n <- 5
1 - pnorm(21, u, s / sqrt(n))

pnorm(21, u, s / sqrt(n))

s / sqrt(n)

pnorm(u + .5, u, s / sqrt(n)) - pnorm(u - .5, u, s / sqrt(n))
