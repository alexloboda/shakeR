#include "dgraph/DynamicGraph.h"
#include <random>

namespace shuffler {
    using namespace std;
    using dgraph::DynamicGraph;
    using dgraph::EdgeToken;

    class Shuffler {
        bool changed;
        long n;
        unsigned hard_stop;
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

        void check_rngs();
        bool do_flip(unsigned, unsigned, unsigned, 
                     unsigned, unsigned, unsigned);
    public:
        Shuffler(unsigned n, unsigned hard_stop);
        
        void add_edge(unsigned v, unsigned u, unsigned type);
        vector<unsigned> perm();
        vector<pair<unsigned, unsigned>> get_edges();
        bool is_connected();
        bool shake_it();
    };
}
