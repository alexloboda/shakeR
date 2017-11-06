#include "dgraph/DynamicGraph.h"
#include <Rcpp.h>
#include <utility>
#include <vector>
#include "DynamicGraph.h"
#include <random>
#include <algorithm>

namespace {
    using namespace std;
    using dgraph::DynamicGraph;
    using dgraph::EdgeToken;

    class Shuffler {
        long n;
        vector<vector<bool>> edges;
        vector<EdgeToken> tokens;
        vector<pair<unsigned , unsigned>> ends;
        DynamicGraph graph;
        std::uniform_int_distribution<unsigned> rng_coin;
        std::uniform_int_distribution<unsigned> rng_for_ends;
        mt19937 mersenne;
    public:
        Shuffler(unsigned n) : graph(n), n(n) {
            random_device rd;
            mersenne = mt19937(rd());
            edges.resize(n, vector<bool>(n, false));
            rng_coin = uniform_int_distribution<unsigned>(0, tokens.size() - 1);
            rng_for_ends = uniform_int_distribution<unsigned>(0, 3);
        }

        void shakeit(){
            while (true) {
                unsigned e1 = rng_coin(mersenne);
                unsigned e2 = rng_coin(mersenne);
                unsigned ends_direct = rng_for_ends(mersenne);
                auto edge = ends[e1];
                auto other = ends[e2];
                unsigned v = edge.first;
                unsigned u = edge.second;
                unsigned w = other.first;
                unsigned z = other.second;

                if (ends_direct / 2 == 0) {
                    std::swap(v, u);
                }

                if (ends_direct % 2 == 0) {
                    std::swap(w, z);
                }

                if (e1 == e2)
                    continue;
                if (v == z || w == u || v == w || u == z || edges[v][z] || edges[w][u]) {
                    continue;
                }
                graph.remove(std::move(tokens[e1]));
                graph.remove(std::move(tokens[e2]));
                tokens[e1] = std::move(graph.add(v, z));
                tokens[e2] = std::move(graph.add(w, u));
                if (graph.is_connected()) {
                    edges[v][u] = false;
                    edges[u][v] = false;
                    edges[w][z] = false;
                    edges[z][w] = false;
                    edges[v][z] = true;
                    edges[z][v] = true;
                    edges[w][u] = true;
                    edges[u][w] = true;
                    ends[e1] = make_pair(v, z);
                    ends[e2] = make_pair(w, u);
                    return;
                }
                graph.remove(std::move(tokens[e1]));
                graph.remove(std::move(tokens[e2]));
                tokens[e1] = std::move(graph.add(v, u));
                tokens[e2] = std::move(graph.add(w, z));
            }
        }
    };
}

// [[Rcpp::export]]
Rcpp::List shake_internal(Rcpp::List& graph) {
    
}
