# 1
f <- function(x) x * .85609 - .52818
f(8)

# 2
f(13) - 9.84

# 7
f <- function(x) exp(log(x) * -1.03079 + 4.57739)
f(4)

# 8
log(10) < 4.5
f(10)

# 9
f <- function(x) 844.82143 - 15.875 * x + .17679 * x ^ 2
f(35)

# 10
px <- c(.00, .09, .21, .28, .32, .1)
x <- seq(5, 30, by=5)
ex <- sum(x * px)
ex

# 11
py <- c(.17, .53, .16, .14, 0, 0)
y <- seq(5, 30, by=5)
ey <- sum(y * py)
ey

# 12
pboth <- convolve(px, rev(py), type = "open")  # I was actually supposed to reverse one of the distributions
pboth
eboth <- sum(seq(10, 60, by=5) * pboth)
eboth - (ex + ey)  # check to make this distribution is right
sum(pboth[1:8])

# 13
pboth[1]

# 14
pboth[7]

# 15 = p(t)
carrier <- .02  # p(c)
testGivenCarrier <- .90 # p(t | c)
testGivenNonCarrier <- .05 # p(t | !c)
test <- testGivenCarrier * carrier + testGivenNonCarrier * (1 - carrier)
test

# 16 = p(c | t)
carrier * testGivenCarrier / test  # p(c) * p(t | c) / p(t)

# 17 = p(c) & (p(t1 = t2 | c)) | p(!c) & (p(t1 = t2 | !c))
twiceIsSame <- function(p) 1 - 2 * p * (1 - p) # not both are different
carrier * twiceIsSame(testGivenCarrier) + (1 - carrier) * twiceIsSame(testGivenNonCarrier)

# 18
dbinom(1, 2, carrier)

# 19
1 - pbinom(0, 2, carrier)

# 20
bags <- 81
uweight <- 20
sweight <- 0.9
pnorm(20.2, uweight, sweight) - pnorm(19.8, uweight, sweight)
