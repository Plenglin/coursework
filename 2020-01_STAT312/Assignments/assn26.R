# 1
n <- 60
dx <- 4.47
ds <- 8.77
dx + qt(.975, df = n - 1) * ds * c(-1, 1) / sqrt(n)

# 3
no <- 28
xo <- 801
so <- 117
ny <- 16
xy <- 780
sy <- 72
df <- diffinmeans.df(no, ny, so, sy)
(xo - xy) + c(-1, 1) * qt(.975, df = df) * sqrt(so^2 / no + sy^2 / ny)

# 5
before <-	c(21, 	31, 	70, 	118, 	64, 	65)
after <- c(20, 	22, 	44, 	80, 	88, 	78)
diff <- after - before
mean(diff) + c(-1, 1) * sd(diff) * qt(.975, df=5) / sqrt(6)
