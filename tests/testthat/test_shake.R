context("shaker")

test_that("shaker doesn't crash on simple graph", {
  g <- make_ring(4)
  V(g)$name <- letters[1:4]
  E(g)$class <- 1
  shake(g)
})

test_that("function cannot shuffle specific diamond", {
  g <- make_ring(4)
  E(g)$class <- c(1, 1, 2, 2)
  expect_warning(shake(g))
})
