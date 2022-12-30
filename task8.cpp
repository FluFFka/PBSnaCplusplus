#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <fstream>
#include <algorithm>


struct VertexSetLess {
    bool operator()(const std::pair<uint32_t, uint32_t>& lhs, const std::pair<uint32_t, uint32_t>& rhs) const {
        return (lhs.second > rhs.second) || (lhs.second == rhs.second && lhs.first < rhs.first);
    }
};


void encoding(char* inp, char* out) {
    std::map<uint32_t, std::set<std::pair<uint32_t, uint8_t>>> adj_list; // v1 -> {v2, weight}
    std::ifstream input(inp);
    uint32_t left, right, weight;
    while (input >> left >> right >> weight) {
        adj_list[left].insert({right, weight});
        adj_list[right].insert({left, weight});
    }
    input.close();
    std::set<std::pair<uint32_t, uint32_t>, VertexSetLess> vs; // vertex, degree (current degree)
    std::map<uint32_t, uint32_t> vm;  // vertex to its degree
    std::map<uint32_t, std::vector<std::pair<uint32_t, std::vector<std::pair<uint32_t, uint8_t>>>>> edges;   // degree to {v1, {v2, weight}} - for output
    for (auto &edge : adj_list) {
        vm[edge.first] = edge.second.size();
        vs.insert({edge.first, edge.second.size()});
    }
    while (!vs.empty()) {
        auto beg = vs.begin();
        std::pair<uint32_t, uint32_t> next_v = *(beg);
        vs.erase(beg);
        std::vector<std::pair<uint32_t, uint8_t>> curr_edges;
        std::vector<std::pair<uint32_t, std::pair<uint32_t, uint8_t>>> to_erase;
        for (auto &[vertex, weight] : adj_list[next_v.first]) {
            if (vertex == next_v.first) {
                to_erase.push_back({next_v.first, {vertex, weight}});
                vm[next_v.first]--;
            } else {
                to_erase.push_back({next_v.first, {vertex, weight}});
                to_erase.push_back({vertex, {next_v.first, weight}});
                auto hint = vs.find({vertex, vm[vertex]});
                vm[vertex]--;
                if (hint != vs.end()) { // vertex isn't visited -> should decrease its degree in vs
                    if (vm[vertex] != 0) {
                        vs.insert({vertex, vm[vertex]});
                    }
                }
                vm[next_v.first]--;
            }
            curr_edges.push_back({vertex, weight});
        }
        for (auto &[f, s] : to_erase) {
            adj_list[f].erase(s);
        }
        if (curr_edges.size() > 0) {
            edges[curr_edges.size()].push_back({next_v.first, curr_edges});
        }
    }

    std::ofstream output(out, std::ios_base::binary);
    int sz = 0;
    for (auto &degrees : edges) {
        uint32_t degree = degrees.first;
        uint32_t degree_sz = degrees.second.size();
        output.write(reinterpret_cast<char*>(&degree), sizeof(degree));
        output.write(reinterpret_cast<char*>(&degree_sz), sizeof(degree_sz));
        sz += 4 + 4 + degree_sz * (4 + 5 * degree);
        for (auto &vertex : degrees.second) {
            output.write(reinterpret_cast<char*>(&vertex.first), sizeof(vertex.first));
            for (auto &sm : vertex.second) {
                output.write(reinterpret_cast<char*>(&sm.first), sizeof(sm.first));
                output.write(reinterpret_cast<char*>(&sm.second), sizeof(sm.second));
            }
        }
    }
    output.close();
    std::cout << "total size: " << sz << " bytes" << std::endl;
}


void decoding(char* inp, char* out) {
    std::vector<std::pair<std::pair<uint32_t, uint32_t>, uint8_t>> edges;
    std::ifstream input(inp, std::ios_base::binary);
    uint32_t degree;
    while (input.read(reinterpret_cast<char*>(&degree), sizeof(degree))) {
        uint32_t num;
        input.read(reinterpret_cast<char*>(&num), sizeof(num));
        for (uint32_t i = 0; i < num; ++i) {
            uint32_t vertex;
            input.read(reinterpret_cast<char*>(&vertex), sizeof(vertex));
            for (uint32_t j = 0; j < degree; ++j) {
                uint32_t sm;
                uint8_t weight;
                input.read(reinterpret_cast<char*>(&sm), sizeof(sm));
                input.read(reinterpret_cast<char*>(&weight), sizeof(weight));
                edges.push_back({{vertex, sm}, weight});
            }
        }
    }
    input.close();

    std::ofstream output(out);
    for (auto &edge : edges) {
        output << edge.first.first << "\t" << edge.first.second << "\t" << (uint32_t)edge.second << "\n";
    }
    output.close();
}


int main(int argc, char **argv) {
    time(NULL);
    if (argc < 6) {
        std::cerr << "Not enough arguments\n";
        return 1;
    }
    char *inp, *out;
    uint32_t scenario = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-i") && i < argc - 1) {
            scenario |= 1;
            inp = argv[i + 1];
        }
        if (!strcmp(argv[i], "-o") && i < argc - 1) {
            scenario |= 2;
            out = argv[i + 1];
        }
        if (!strcmp(argv[i], "-s")) {
            scenario |= 4;
        }
        if (!strcmp(argv[i], "-d")) {
            scenario |= 8;
       }
    }
    if (scenario == 7) {
        encoding(inp, out);
    } else if (scenario == 11) {
        decoding(inp, out);
    } else {
        std::cerr << "Wrong arguments\n";
        exit(1);        
    }

    return 0;
}