x <- 438.29
n <- 17
s <- 15.14

# 1: true average interval
err <- qt(.995, df = n - 1) * s / sqrt(n) 
x + err * c(-1, 1)

# 2: prediction (of next sample) interval
err <- qt(.995, df = n - 1) * s * sqrt(1 + 1 / n)
x + err * c(-1, 1)

# 3: tolerance interval
x + qt(.995, df = n - 1) * s * c(-1, 1)
tol <- 4.393 # for n = 17, 99 conf, 99 capture
x + tol * s * c(-1, 1)

n <- 16
s <- 116115.58
x <- 90700.5
err <- qt(.995, df = n - 1) * s * sqrt(1 + 1 / n)
x + err * c(-1, 1)
