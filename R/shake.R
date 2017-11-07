#' @import igraph
#' @export
#' @param graph accompanied by equivalence classes(1..n)
shake <- function(graph) {
  df <- as_data_frame(graph, what = "edges")[c("from", "to", "class")]
  res <- shake_internal(df, length(V(graph)), length(E(graph)) * 10)
  res
}
