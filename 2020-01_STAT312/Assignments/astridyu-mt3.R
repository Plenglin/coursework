# 2 (margin of error)
n.total <- 100
n.red <- 46
prop.red <- n.red / n.total
moe <- qnorm(.975) * prop.red * (1 - prop.red) / sqrt(n.total)  # norm because n >= 30
moe

# 3 (sample size)
moe <- 0.045
sample_size_needed <- function(conf, s, moe) {
  (qnorm(1 - (1 - conf) / 2) * s / moe) ^ 2
}
sample_size_needed(.95, .25, moe)

# 4 (test stat), 5 (p-val)
n.wires <- c(71, 59, 52, 60, 58)
chisq.test(n.wires)

# 7 (upper bound)
drink.n <- 121
drink.x <- 8.7
drink.s <- .22
drink.x + qnorm(.995) * drink.s / sqrt(drink.n)

# 8 (sample size)
sample_size_needed(
  conf = .95, 
  s = .16, 
  moe = .02)

# 9 (test statistic)
meandiff.n <- 9
meandiff.mean <- 4.140
meandiff.s <- 6.9
meandiff.t <- meandiff.mean / (meandiff.s / sqrt(n))  # t because n < 30
meandiff.t

# 10 (p-value)
1 - pt(meandiff.t, df = meandiff.n - 1)  # n < 30

# 12
upper <- .8188
lower <- .4330
(upper - lower) / 2  # margin of error

# 14
bridge.u <- 140000
bridge.x <- 122631
bridge.s <- 17670
bridge.n <- 7
bridge.moe <- bridge.s / sqrt(bridge.n)
bridge.t <- (bridge.x - bridge.u) / bridge.moe  # t because n < 30
bridge.t

# 15 (p-value)
pt(bridge.t, df = bridge.n - 1)

# 19 (confidence interval)
bridge.x + c(-1, 1) * qt(.975, df = bridge.n - 1) * bridge.moe
