s <- .2
ux <- 30
uy <- 50
(1 - pnorm(29.8, ux, s)) * pnorm(50.5, uy, s)

(1 - pnorm(29.8, ux, s)) * (1 - pnorm(50.5, uy, s))

xs <- c(1, 2, 3, 4)
(sum(xs) / 4 * 2)

.75 * .75

ps <- c(.25, .25, .25, .25)
x2s <- c(0, convolve(ps, ps, type = "open"))
sum(x2s[6:8])
