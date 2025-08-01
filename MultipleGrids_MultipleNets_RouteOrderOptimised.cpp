#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <tuple>
#include <algorithm>
#include <string>
#include <set>
#include <chrono>
#include <iomanip>

using namespace std;

const int INF = INT_MAX;
const int VIA_COST = 20;
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

struct RoutedNetInfo {
    Net net;
    vector<tuple<int, int, int>> path;
    int cost;
    char symbol;
};

bool isValid(int x, int y, int rows, int cols) {
    return x >= 0 && y >= 0 && x < rows && y < cols;
}

vector<tuple<int, int, int>> dijkstra3D(
    const vector<vector<vector<int>>>& grid,
    const vector<vector<bool>>& hasVia,
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

        // Via transitions (stacked vias: available at (x,y) across all layers)
        if (hasVia[x][y]) {
            if (l + 1 < layers && !blocked[l + 1][x][y]) {
                int newCost = dist[l][x][y] + VIA_COST + grid[l + 1][x][y];
                if (newCost < dist[l + 1][x][y]) {
                    dist[l + 1][x][y] = newCost;
                    parent[l + 1][x][y] = {x, y, l};
                    pq.push(Node(x, y, l + 1, newCost));
                }
            }
            if (l - 1 >= 0 && !blocked[l - 1][x][y]) {
                int newCost = dist[l][x][y] + VIA_COST + grid[l - 1][x][y];
                if (newCost < dist[l - 1][x][y]) {
                    dist[l - 1][x][y] = newCost;
                    parent[l - 1][x][y] = {x, y, l};
                    pq.push(Node(x, y, l - 1, newCost));
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
            if (pl != l) totalCost += VIA_COST;
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
    int rows = 6, cols = 6, layers = 3;

    vector<vector<vector<int>>> grid = {
        {
            {1, 2, 3, 4, 5, 1},
            {3, 6, 5, 2, 2, 1},
            {2, 1, 1, 7, 6, 3},
            {1, 4, 2, 3, 1, 3},
            {4, 1, 1, 9, 6, 1},
            {1, 3, 4, 3, 2, 5}
        },
        {
            {3, 2, 1, 4, 3, 5},
            {1, 3, 1, 5, 2, 6},
            {1, 4, 2, 1, 1, 2},
            {2, 1, 1, 1, 7, 3},
            {1, 4, 2, 1, 3, 2},
            {1, 5, 6, 1, 7, 3}
        },
        {
            {1, 4, 1, 1, 3, 1},
            {2, 1, 2, 1, 2, 2},
            {1, 1, 2, 1, 4, 1},
            {1, 2, 3, 1, 3, 1},
            {5, 6, 3, 1, 4, 1},
            {5, 1, 2, 1, 3, 1}
        }
    };

    vector<vector<bool>> hasVia(rows, vector<bool>(cols, false));
    hasVia[1][1] = true;
    hasVia[2][2] = true;
    hasVia[3][3] = true;
    hasVia[4][4] = true;

    int n;
    cout << "Enter number of nets to route: ";
    cin >> n;

    vector<Net> nets;
    int validNetCount = 0;
    while (validNetCount < n) {
        int x1, y1, l1, x2, y2, l2;
        cout << "Net " << validNetCount + 1 << " Start (x y layer): ";
        cin >> x1 >> y1 >> l1;
        cout << "Net " << validNetCount + 1 << " Target (x y layer): ";
        cin >> x2 >> y2 >> l2;

        if (!isValid(x1, y1, rows, cols) || !isValid(x2, y2, rows, cols) ||
            l1 < 0 || l2 < 0 || l1 >= layers || l2 >= layers) {
            cout << "Invalid net coordinates. Please re-enter.\n";
            continue;
        }

        Net net;
        net.name = "Net" + to_string(validNetCount + 1);
        net.start = {x1, y1, l1};
        net.target = {x2, y2, l2};
        nets.push_back(net);
        validNetCount++;
    }

    struct RoutedNetInfo {
        Net net;
        vector<tuple<int, int, int>> path;
        int cost;
        char symbol;
    };

    vector<int> indices(nets.size());
    for (int i = 0; i < indices.size(); ++i) indices[i] = i;

    int bestRouted = -1;
    int minCost = INF;
    vector<vector<vector<char>>> bestLayout;
    vector<RoutedNetInfo> bestInfos;

    do {
        vector<vector<vector<bool>>> blocked(layers, vector<vector<bool>>(rows, vector<bool>(cols, false)));
        vector<vector<vector<char>>> layout(layers, vector<vector<char>>(rows, vector<char>(cols, '.')));

        int totalRoutingCost = 0;
        int routedNets = 0;
        vector<RoutedNetInfo> routedInfos;

        for (int idx = 0; idx < indices.size(); ++idx) {
            Net net = nets[indices[idx]];
            char symbol = (idx < SYMBOLS.size()) ? SYMBOLS[idx] : '*';

            auto path = dijkstra3D(grid, hasVia, blocked, net.start, net.target);
            if (path.empty()) continue;

            int cost = computeTotalCost(path, grid);
            totalRoutingCost += cost;
            routedNets++;

            for (auto [x, y, l] : path) {
                blocked[l][x][y] = true;
                layout[l][x][y] = symbol;
            }

            RoutedNetInfo info;
            info.net = net;
            info.path = path;
            info.cost = cost;
            info.symbol = symbol;
            routedInfos.push_back(info);
        }

        if (routedNets > bestRouted || (routedNets == bestRouted && totalRoutingCost < minCost)) {
            bestRouted = routedNets;
            minCost = totalRoutingCost;
            bestLayout = layout;
            bestInfos = routedInfos;
        }

    } while (next_permutation(indices.begin(), indices.end()));

    cout << "Best order of routing:\n";
    for (const auto& info : bestInfos) {
        cout << info.net.name << " ";
    }
    cout << "\n";
    
        for (const auto& info : bestInfos) {
        cout << info.net.name << " routed with cost " << info.cost << " using symbol '" << info.symbol << "':\n";
        for (size_t i = 0; i < info.path.size(); ++i) {
            auto [x, y, l] = info.path[i];
            cout << "(" << x << "," << y << ") [L" << l << "]";
            if (i < info.path.size() - 1) cout << " -> ";
        }
        cout << " -> END\n\n";
    }
    
        printGridLayers(bestLayout, layers);

    cout << "\n✅ Routed " << bestRouted << " nets with total cost: " << minCost << "\n\n";

    return 0;
}
