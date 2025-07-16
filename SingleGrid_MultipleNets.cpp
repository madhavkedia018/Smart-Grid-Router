#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <tuple>
#include <algorithm>
#include <string>
using namespace std;

const int INF = INT_MAX;
const string SYMBOLS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct Node {
    int x, y, layer, cost;
    Node(int _x, int _y, int _layer, int _cost)
        : x(_x), y(_y), layer(_layer), cost(_cost) {}
    bool operator>(const Node& other) const {
        return cost > other.cost;
    }
};

struct Net {
    string name;
    tuple<int, int, int> start;
    tuple<int, int, int> target;
};

bool isValid(int x, int y, int rows, int cols) {
    return x >= 0 && y >= 0 && x < rows && y < cols;
}

vector<tuple<int, int, int>> dijkstra3D(
    const vector<vector<vector<int>>>& grid,
    const vector<vector<vector<bool>>>& blocked,
    tuple<int, int, int> start,
    tuple<int, int, int> target
) {
    int layers = grid.size(), rows = grid[0].size(), cols = grid[0][0].size();

    vector<vector<vector<int>>> dist(layers, vector<vector<int>>(rows, vector<int>(cols, INF)));
    vector<vector<vector<tuple<int, int, int>>>> parent(layers, vector<vector<tuple<int, int, int>>>(rows, vector<tuple<int, int, int>>(cols, {-1, -1, -1})));

    auto [sx, sy, sl] = start;
    auto [tx, ty, tl] = target;

    if (blocked[sl][sx][sy] || blocked[tl][tx][ty])
        return {};

    dist[sl][sx][sy] = grid[sl][sx][sy];
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    pq.push(Node(sx, sy, sl, grid[sl][sx][sy]));

    vector<pair<int, int>> directions = {{0,1},{1,0},{-1,0},{0,-1}};

    while (!pq.empty()) {
        Node current = pq.top(); pq.pop();
        int x = current.x, y = current.y, l = current.layer;

        if (make_tuple(x, y, l) == target) break;

        for (auto dir : directions) {
            int nx = x + dir.first, ny = y + dir.second;
            if (isValid(nx, ny, rows, cols) && !blocked[l][nx][ny]) {
                int newCost = dist[l][x][y] + grid[l][nx][ny];
                if (newCost < dist[l][nx][ny]) {
                    dist[l][nx][ny] = newCost;
                    parent[l][nx][ny] = {x, y, l};
                    pq.push(Node(nx, ny, l, newCost));
                }
            }
        }
    
    }

    vector<tuple<int, int, int>> path;
    if (dist[tl][tx][ty] == INF) return {};

    tuple<int, int, int> p = target;
    while (p != make_tuple(-1, -1, -1)) {
        path.push_back(p);
        p = parent[get<2>(p)][get<0>(p)][get<1>(p)];
    }
    reverse(path.begin(), path.end());
    return path;
}

int computeTotalCost(const vector<tuple<int, int, int>>& path,
                     const vector<vector<vector<int>>>& grid) {
    int totalCost = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        auto [x, y, l] = path[i];
        totalCost += grid[l][x][y];
        if (i > 0) {
            auto [px, py, pl] = path[i - 1];
        }
    }
    return totalCost;
}

void printGridLayers(const vector<vector<vector<char>>>& layout, int layers) {
    for (int l = 0; l < layers; ++l) {
        cout << "\nLayer " << l << ":\n";
        for (int i = 0; i < layout[0].size(); ++i) {
            for (int j = 0; j < layout[0][0].size(); ++j) {
                cout << layout[l][i][j] << " ";
            }
            cout << "\n";
        }
    }
}

int main() {
    int rows = 8, cols = 8, layers = 1;

    vector<vector<vector<int>>> grid = {
        {
            {1, 2, 3, 4,5,1,2,6},
            {3, 6, 5, 2,2,1,4,5},
            {2, 1, 1, 7,6,3,8,1},
            {1, 4, 2, 3,1,3,4,2},
            {4, 1, 1, 9,6,1,1,3},
            {1, 3, 4, 3,2,5,6,7},
            {2, 1, 3, 9,5,1,2,3},
            {2, 3, 1, 3,4,5,5,7}
        }
    };



    vector<vector<vector<bool>>> blocked(layers, vector<vector<bool>>(rows, vector<bool>(cols, false)));
    vector<vector<vector<char>>> layout(layers, vector<vector<char>>(rows, vector<char>(cols, '.')));

    int n;
    cout << "Enter number of nets to route: ";
    cin >> n;

    vector<Net> nets;
    for (int i = 0; i < n; ++i) {
        int x1, y1, l1, x2, y2, l2;
        cout << "Net " << i+1 << " Start (x y layer): ";
        cin >> x1 >> y1 >> l1;
        cout << "Net " << i+1 << " Target (x y layer): ";
        cin >> x2 >> y2 >> l2;

        if (!isValid(x1, y1, rows, cols) || !isValid(x2, y2, rows, cols) || l1 < 0 || l2 < 0 || l1 >= layers || l2 >= layers) {
            cout << "Invalid net coordinates. Skipping...\n";
            continue;
        }

        Net net;
        net.name = "Net" + to_string(i + 1);
        net.start = {x1, y1, l1};
        net.target = {x2, y2, l2};
        nets.push_back(net);
    }

    int totalRoutingCost = 0;

    for (int i = 0; i < nets.size(); ++i) {
        auto& net = nets[i];
        char symbol = (i < SYMBOLS.size()) ? SYMBOLS[i] : '*';
        cout << "\nRouting " << net.name << "...\n";

        auto path = dijkstra3D(grid, blocked, net.start, net.target);

        if (path.empty()) {
            cout << "No path found for " << net.name << "!\n";
            continue;
        }

        int cost = computeTotalCost(path, grid);
        totalRoutingCost += cost;

        cout << net.name << " routed with cost " << cost << ":\n";
        for (auto [x, y, l] : path) {
            blocked[l][x][y] = true;
            layout[l][x][y] = symbol;
            cout << "(" << x << "," << y << ") [L" << l << "] -> ";
        }
        cout << "END\n";
    }

    printGridLayers(layout, layers);

    cout << "\n✅ Total Routing Cost across all nets: " << totalRoutingCost << endl;

    return 0;
}
