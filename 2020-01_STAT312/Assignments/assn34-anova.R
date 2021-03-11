anova_table <- function(avgs, ns, sds) {
  k <- length(avgs)
  n <- sum(ns)
  grand_avg <- mean((ns / n) * avgs)
  
  df.between <- k - 1
  df.within <- n - k
  df.total <- n - 1
  
  sstr <- sum(ns * (avgs - grand_avg) ^ 2)
  sse <- sum((ns - 1) * sds ^ 2)
  sst <- sstr + sse
  
  mstr <- sse / df.between
  mse <- sse / df.within
  
  f <- mstr / mse
  p <- 1 - pf(f, df.between, df.within)
  
  list(
    grand_avg = grand_avg,
    
    df.between = df.between,
    df.within = df.within,
    df.total = df.total,
    
    sstr = sstr,
    sse = sse,
    sst = sst,
    
    mstr = mstr,
    mse = mse,
    
    f = f,
    p = p
  )
}

# 1
n <- 10
k <- 5
mse <- .64
f <- 15.32
mse * f

# 2
anova_table(rep(1, k), rep(n, k), rep(1, k)) # just for df

# 3
toughnesses <- matrix(
  c(39, 47, 44,  # 100
    52, 56, 53,  # 120
    40, 46, 42), # 140
  nrow = 3,
)
means <- apply(toughnesses, 1, mean)
sds <- apply(toughnesses, 1, sd)
ns <- apply(toughnesses, 1, length)
weld_anova <- anova_table(means, ns, sds)
weld_anova

# 4
1 - pf(11.411, df1 = weld_anova$df.between, df2 = weld_anova$df.within)

# 5
weld_anova$grand_avg
