// Modified C++ Router for ML Optimizer Integration
// Reads netlist input from file and writes results to output file

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <tuple>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std;

const int INF = INT_MAX;
const int VIA_COST = 20;

struct Node {
    int x, y, layer, cost;
    Node(int _x, int _y, int _layer, int _cost)
        : x(_x), y(_y), layer(_layer), cost(_cost) {}
    bool operator>(const Node& other) const {
        return cost > other.cost;
    }
};

vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {-1, 0}, {0, -1}};

bool isValid(int x, int y, int rows, int cols) {
    return x >= 0 && y >= 0 && x < rows && y < cols;
}

vector<tuple<int, int, int>> dijkstra3D(
    vector<vector<vector<int>>>& grid,
    vector<vector<vector<bool>>>& vias,
    tuple<int, int, int> start,
    tuple<int, int, int> target
) {
    int layers = grid.size();
    int rows = grid[0].size();
    int cols = grid[0][0].size();

    vector<vector<vector<int>>> dist(layers, vector<vector<int>>(rows, vector<int>(cols, INF)));
    vector<vector<vector<tuple<int, int, int>>>> parent(layers, vector<vector<tuple<int, int, int>>>(rows, vector<tuple<int, int, int>>(cols, {-1, -1, -1})));

    auto [sx, sy, sl] = start;
    auto [tx, ty, tl] = target;

    dist[sl][sx][sy] = grid[sl][sx][sy];
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    pq.push(Node(sx, sy, sl, grid[sl][sx][sy]));

    while (!pq.empty()) {
        Node current = pq.top(); pq.pop();
        int x = current.x, y = current.y, l = current.layer;

        if (make_tuple(x, y, l) == target) break;

        for (auto dir : directions) {
            int nx = x + dir.first, ny = y + dir.second;
            if (isValid(nx, ny, rows, cols)) {
                int newCost = dist[l][x][y] + grid[l][nx][ny];
                if (newCost < dist[l][nx][ny]) {
                    dist[l][nx][ny] = newCost;
                    parent[l][nx][ny] = {x, y, l};
                    pq.push(Node(nx, ny, l, newCost));
                }
            }
        }

        if (l + 1 < layers && vias[x][y][l]) {
            int newCost = dist[l][x][y] + VIA_COST + grid[l + 1][x][y];
            if (newCost < dist[l + 1][x][y]) {
                dist[l + 1][x][y] = newCost;
                parent[l + 1][x][y] = {x, y, l};
                pq.push(Node(x, y, l + 1, newCost));
            }
        }
    }

    vector<tuple<int, int, int>> path;
    if (dist[tl][tx][ty] == INF) return path;

    tuple<int, int, int> p = target;
    while (p != make_tuple(-1, -1, -1)) {
        path.push_back(p);
        p = parent[get<2>(p)][get<0>(p)][get<1>(p)];
    }
    reverse(path.begin(), path.end());
    return path;
}

int computeTotalCost(const vector<tuple<int, int, int>>& path, const vector<vector<vector<int>>>& grid) {
    int totalCost = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        auto [x, y, l] = path[i];
        totalCost += grid[l][x][y];
        if (i > 0) {
            auto [px, py, pl] = path[i - 1];
            if (pl != l)
                totalCost += VIA_COST;
        }
    }
    return totalCost;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./router_exec <input_file> <output_file>\n";
        return 1;
    }

    ifstream fin(argv[1]);
    ofstream fout(argv[2]);

    int rows, cols, layers, num_nets;
    fin >> cols >> rows >> layers;
    fin >> num_nets;

    vector<vector<vector<int>>> grid(layers, vector<vector<int>>(rows, vector<int>(cols, 1)));
    vector<vector<vector<bool>>> vias(rows, vector<vector<bool>>(cols, vector<bool>(layers, true)));

    int routed = 0, total_cost = 0;

    for (int i = 0; i < num_nets; ++i) {
        int x1, y1, l1, x2, y2, l2;
        fin >> x1 >> y1 >> l1 >> x2 >> y2 >> l2;
        auto path = dijkstra3D(grid, vias, {x1, y1, l1}, {x2, y2, l2});
        if (!path.empty()) {
            ++routed;
            total_cost += computeTotalCost(path, grid);
            for (auto [x, y, l] : path) grid[l][x][y] += 1;
        }
    }

    fout << routed << "\n" << total_cost << "\n";
    return 0;
}
