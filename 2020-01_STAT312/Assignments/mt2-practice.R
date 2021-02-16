# 1
n <- 32
(1 - .25) ^ 16
dbinom(16, 32, .25)

# 2
j <- c(.64, .27, .09)
k <- c(.81, .11, .08)
n <- 0:2
convolve(j, k, type = "open")

.27 * .08 + .11 * .09

# 3
carrier <- .2   # P(c)
posTestNonCarrier <- 0.1  # P(t | c)
posTestCarrier <- 0.85  # P(t | !c)
allPositive <- posTestCarrier * carrier + posTestNonCarrier * (1 - carrier)  # P(t)
allPositive

# 4
posTestCarrier * carrier / allPositive  # P(c | t)

# 5
a <- .45
b <- .55
c <- .65
ab <- (a * b)
ab + c - ab * c

# 6
pdefect <- .04
(1 - pdefect) ^ 10

# 7
dbinom(1, 10, pdefect)

# 8
purple <- .64
orange <- .15
purpleAndOrange <- .08
atLeastOne <- purple + orange - purpleAndOrange
atLeastOne

# 9
orange - purpleAndOrange

# 10
1 - atLeastOne

# 16
fit <- function(x) 626.99 + 12.3 * x
fit(5) - 690

# 17
lambda <- 1 / 500
failure <- pexp(40, lambda)
(1 - failure) ^ 2

# 18
failure ^ 2

