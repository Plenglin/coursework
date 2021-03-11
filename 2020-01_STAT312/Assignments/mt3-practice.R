# 2
lower <- 50.4321 
upper <- 53.5861
mean(upper, lower)

# 3
n <- 25
s <- 3.2
42.06 + s / sqrt(n)

# 8
n <- 16
x <- 94.56
u <- 96
s <- 2.87
pt((x - u) / (s / sqrt(n)), df = n - 1)

# 11
u <- 60
x <- 64.12
n <- 81
s <- 9.02
se <- s / sqrt(n)
z <- (x - u) / se
z # should have been test stat
# 1 - abs(pnorm(z))

# 12
x + c(-1, 1) * qnorm(.975) * se

# 13
err <- qt(.975, df = n - 1) * s * sqrt(1 + 1 / n)
x + err * c(-1, 1)

# 16
red.fail <- 53
red.survive <- 47
red.n <- red.fail + red.survive
black.fail <- 26
black.survive <- 16
black.n <- black.fail + black.survive
failprop <- (red.fail + black.fail) / (red.n + black.n)
red.n * failprop

# 17
black.n * (1 - failprop)

# 18
m <- matrix(
  c(red.fail, red.survive,
    black.fail, black.survive),
  nrow = 2,
  byrow = TRUE
)
m
(red.fail - failprop * red.n) ^ 2 / red.n + (black.fail - failprop * black.n) ^ 2 / black.n
chisq.test(m)
chisq.test(c(red.fail, black.fail))


# 19
moe <- .034
s <- .25
z <- qnorm(.975)
# moe = z * s / sqrt(n)
(z * s / moe) ^ 2  # = n
