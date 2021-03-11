ci <- function (n, mean, sd, l) {
  se <- sd / sqrt(n)
  left <- (1 - l) / 2
  c(qnorm(left, mean, se), qnorm(1 - left, mean, se))
} 

nforwidth <- function(sd, l, w) {
  (qnorm((1 - l) / 2) * sd / (w / 2)) ^ 2
}

population_proportion <- function(p, n, conf) {
  z <- qnorm(1 - (1 - l) / 2)
  dev <- z * sqrt(p * (1 - p) / n + (z ^ 2) / (4 * (n ^ 2)))
  mid <- p + (z ^ 2) / (2 * n)
  scl <- 1 + (z ^ 2) / n
  (mid + dev * c(-1, 1)) / scl
}

diffinmeans.df <- function(n1, n2, s1, s2) {
  se1 <- s1 / sqrt(n1)
  se2 <- s2 / sqrt(n2)
  (se1^2 + se2^2) ^ 2 / (se1^4 / (n1 - 1) + se2^4 / (n2 - 1))
}
