nonconfs <- matrix(
  c(
    34, 65, 17, 21, 13,
    23, 52, 25, 19, 6,
    32, 28, 16, 14, 10
  ),
  ncol = 5,
  byrow = TRUE
)
colnames(nonconfs) <- c("blemish", "crack", "location", "missing", "other")
nonconfs

# 1
(150 - 13) * 89 / (375 - 29)

# 2
(100 - 10) * 58 / (375 - 29)

# 3
nc2 <- nonconfs[1:3,  c("blemish", "crack", "location", "missing")]
chisq.test(nc2)
