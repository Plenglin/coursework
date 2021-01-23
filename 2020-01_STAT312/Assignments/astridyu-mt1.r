# 1
lambda <- 6
dpois(6, lambda)  # exactly 6

# 2
dpois(0, lambda)  # exactly 0

# 3 #Changed bulbs from 25 to 20
bulbs <- 20
p_red <- 0.01
dbinom(0, bulbs, p_red)  # exactly 0

# 4
1 - pbinom(0, bulbs, p_red)  # at least 1

# 5
failed <- 0:3
prob <- c(.93, .04, 0.02, 0.01)
ev <- sum(failed * prob)  # exp. value
ev

# 6 #changed mean to sum
sum(prob * (failed - ev) ^ 2)  # variance

# 7
pink <- 18
gray <- 12
dhyper(3, pink, gray, 3)  # exactly 3 

# (sanity check to make sure i got the order right)
(pink / (pink + gray)) ^ 3
(gray / (pink + gray)) ^ 3

# 11
functional <- 11
broken <- 1
dhyper(8, functional, broken, 8)  # exactly 8

# 12
u <- 600
s <- 180
1 - pnorm(555, u, s)  # at least 555

# 13
lambda <- 1.051
1 - pexp(2, lambda)  # at least 2

# 14
f <- (function(x) .012 * (25 - x^2))
ev <- integrate(function(x) x * f(x), 0, 5)$value  # expected value
ev

# 15
integrate(f, 4, 5)  # between 4, 5

# 16
variance <- integrate(function(x) f(x) * (x - ev) ^ 2 , 0, 5)$value 
sqrt(variance)  # standard deviation
