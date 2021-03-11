# 4
expected <- c(8.2, 25.2, 65.8, 100.8)
observed <- c(16, 33, 52, 88) # hw
# observed <- c(11, 24, 69, 96) # example

expected <- expected * sum(observed) / sum(expected)
chisq.test(observed, p = expected, rescale.p = TRUE)
