source('conf_interval.R')

ci(n = 56, mean = 9.15, sd = 1.27, .99)

ci(n = 61, mean = 3.08, sd = 1.71, .95)

sd <- .14
# qnorm(.995, 0) * se = .02
# se = sd / sqrt(n)
# qnorm(.995, 0) * sd / sqrt(n) = .02
(qnorm(.995) * sd / .02) ^ 2 # = n

ci(32, 64.3, 6, .90)
