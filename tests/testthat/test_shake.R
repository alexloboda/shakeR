context("shaker")

test_that("pkg works on simple graph", {
  g <- make_ring(4)
  E(g)$class <- 1
  shake(g)
})
