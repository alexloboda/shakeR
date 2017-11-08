#' @import igraph
#' @export
#' @param graph accompanied by edge attribute `class` (1..n)
shake <- function(graph, number_of_permutations = length(E(graph)) * 10,
                  hard_stop = number_of_permutations * 10) {
  stopifnot("class" %in% list.edge.attributes(graph))
  stopifnot(all(E(graph)$class > 0))

  df <- as_data_frame(graph, what = "edges")[c("from", "to", "class")]
  num_nodes <- length(V(graph))
  res <- shake_internal(df, num_nodes, number_of_permutations, hard_stop)
  g <- graph_from_edgelist(as.matrix(res))
  edge.attributes(g) <- edge.attributes(graph)
  g
}
