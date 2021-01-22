# 1 
sample = 10
def = 18
ndef = 36
dhyper(2, def, ndef, sample)

# 2
dhyper(5, def, ndef, sample)

# 3
10 * ndef / (def + ndef)

# 4
sample = c(21, 13, 18, 8)
mean(sample)

# 5
var(sample)

# 6
gre_mean = 500
gre_std = 100
pnorm(780, gre_mean, gre_std) - pnorm(600, gre_mean, gre_std)

# 7
1 - pnorm(780, gre_mean, gre_std)

# 8
qnorm(0.99, gre_mean, gre_std)

# 10
eggs = 6
1 - pbinom(0, eggs, 0.1)

# 11
dbinom(6, eggs, 0.4)

# 12
prob = c(.75, .18, .04, .03)
fails = 0:3
efail = sum(prob * fails)
efail

# 13
mean(prob * (fails - efail) ^ 2)

# 14
freq = c(1, 2, 3, 12, 11, 15, 18, 10)
sum(freq[4:7]) / 72
(11 + 15 + 18 + 10) / 72

# 15
beta = 90
alpha = 4.5
pweibull(60, alpha, beta)

# 16
pweibull(90, alpha, beta) - pweibull(60, alpha, beta) 

