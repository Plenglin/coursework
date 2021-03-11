# 1
u0 <- 10
n <- 36
x <- 10.18
s <- 0.32
Z <- (x - u0) / (s / sqrt(n))
1 - pnorm(Z)

# 2
n <- 64
x <- 10.07
Z <- (x - u0) / (s / sqrt(n))
1 - pnorm(Z)
