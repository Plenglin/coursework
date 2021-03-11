# 3
xs <- c(2006.1, 2065.2, 2118.9, 1686.6, 1966.9, 1792.5)
t.test(x = xs, mu = 1800, alternative = "greater")

# 4
n1 <- 15
s1 <- 7.76
n2 <- 20 
s2 <- 1.52
x1 <- 138.52
x2 <- 149.07
df <- diffinmeans.df(n1, n2, s1, s2)
t <- (x2 - x1) / sqrt(s1 ^ 2 / n1 + s2 ^ 2 / n2)

# 5
(1 - pt(t, df = df)) * 2
