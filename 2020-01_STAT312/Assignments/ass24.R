source('conf_interval.R')

qnorm(.975)
population_proportion(.18, 1025, .95)

interval <- population_proportion(.18, 611, .95); interval[2] - interval[1]

prop.test(.18 * 1025, 1025, conf.level = .95)

propnforwidth(l = .95, w = .08)

n1 <- 68
n2 <- 74
m1 <- 26.99
m2 <- 35.76
s1 <- 4.89
s2 <- 6.43

se <- sqrt(s1 ^ 2 / n1 + s2 ^ 2 / n2)
dev <- qnorm(.995) * se
(m2 - m1) + dev * c(-1, 1)

n <- 250
p <- 1 - 4 / 250
propci(n, p, .95)

