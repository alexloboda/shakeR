#include "dgraph/DynamicGraph.h"
#include <utility>
#include <vector>
#include "dgraph/DynamicGraph.h"
#include <random>
#include <algorithm>
#include<Rcpp.h>

namespace {
    using namespace std;
    using dgraph::DynamicGraph;
    using dgraph::EdgeToken;

    class Shuffler {
        bool changed;
        long n;
        vector<vector<unsigned>> index;
        vector<unsigned> rindex;
        vector<unsigned> permutation;
        vector<vector<bool>> edges;
        vector<EdgeToken> tokens;
        vector<pair<unsigned , unsigned>> ends;
        DynamicGraph graph;
        uniform_int_distribution<unsigned> rng_for_ends;
        uniform_int_distribution<unsigned> rng_edge;
        vector<uniform_int_distribution<unsigned>> rng_inclass;
        mt19937 mersenne;
    public:
        Shuffler(unsigned n) : graph(n), n(n), changed(true) {
            random_device rd;
            mersenne = mt19937(rd());
            edges.resize(n, vector<bool>(n, false));
            rng_for_ends = uniform_int_distribution<unsigned>(0, 3);
        }

        void add_edge(unsigned v, unsigned u, unsigned type) {
            changed = true;
            if (type >= index.size()) {
                index.resize(type + 1, vector<unsigned>());
            }
            unsigned edge_num = tokens.size();
            index[type].push_back(edge_num);
            rindex.push_back(type);
            permutation.push_back(edge_num);
            edges[v][u] = true;
            edges[u][v] = true;
            ends.emplace_back(v, u);
            tokens.push_back(std::move(graph.add(v, u)));
        }

        vector<unsigned> perm() {
            return permutation;
        }

        vector<pair<unsigned, unsigned>> get_edges(){
            return ends;
        }

        void check_rngs(){
            rng_edge = uniform_int_distribution<unsigned>(0, tokens.size() - 1);
            rng_inclass = vector<uniform_int_distribution<unsigned>>();
            for (int i = 0; i < index.size(); i++) {
                rng_inclass.emplace_back(0, index[i].size() - 1);
            }
        }

        void shake_it(){
            if (changed) {
                check_rngs();
                changed = false;
            }
            while (true) {
                unsigned e1 = rng_edge(mersenne);
                unsigned type = rindex[e1];
                unsigned e2 = index[type][rng_inclass[type](mersenne)];
                unsigned ends_direct = rng_for_ends(mersenne);
                cout << e1 << "\t" << e2 << endl;
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
                if (v == z || w == u || v == w || u == z) {
                    continue;
                }
                if (edges[v][z] || edges[w][u]) {
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
                    swap(permutation[e1], permutation[e2]);
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

using namespace Rcpp;

// [[Rcpp::export]]
Rcpp::List shake_internal(Rcpp::List& graph,
                          Rcpp::IntegerVector size,
                          Rcpp::IntegerVector permutations) {
    auto from = as<NumericVector>(graph["from"]);
    auto to = as<NumericVector>(graph["to"]);
    auto classes = as<NumericVector>(graph["class"]);

    Shuffler shuffler(size[0]);
    for (int i = 0; i < from.size(); i++) {
        shuffler.add_edge(from(i) - 1, to(i) - 1, classes(i));
    }
    for (unsigned i = 0; i < permutations[0]; i++) {
        shuffler.shake_it();
    }
    auto permutation = shuffler.perm();

    auto ends = shuffler.get_edges();
    vector<unsigned> f;
    vector<unsigned> t;
    for (auto it = ends.begin(); it != ends.end(); it++){
        f.push_back(it->first + 1);
        t.push_back(it->second + 1);
    }

    Rcpp::List ret;
    ret["edge_num"] = IntegerVector(permutation.begin(), permutation.end());
    ret["from"] = IntegerVector(f.begin(), f.end());
    ret["to"] = IntegerVector(t.begin(), t.end());
    return ret;
}
