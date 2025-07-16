#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <tuple>
#include <stack>
#include <algorithm>

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

// Directions: up, down, left, right
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

        // Move in 2D directions
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

        // Move to layer above using via
        if (l + 1 < layers && vias[x][y][l]) {
            int newCost = dist[l][x][y] + VIA_COST + grid[l + 1][x][y];
            if (newCost < dist[l + 1][x][y]) {
                dist[l + 1][x][y] = newCost;
                parent[l + 1][x][y] = {x, y, l};
                pq.push(Node(x, y, l + 1, newCost));
            }
        }
    }

    // Reconstruct path
    vector<tuple<int, int, int>> path;
    if (dist[tl][tx][ty] == INF) {
        cout << "No path found!\n";
        return path;
    }

    tuple<int, int, int> p = target;
    while (p != make_tuple(-1, -1, -1)) {
        path.push_back(p);
        p = parent[get<2>(p)][get<0>(p)][get<1>(p)];
    }
    reverse(path.begin(), path.end());
    return path;
}

char getCellSymbol(int x, int y, int l,
                   const vector<tuple<int, int, int>>& path,
                   tuple<int, int, int> start,
                   tuple<int, int, int> target,
                   const vector<vector<vector<bool>>>& vias) {
    if (make_tuple(x, y, l) == start) return 'S';
    if (make_tuple(x, y, l) == target) return 'T';
    for (size_t i = 0; i < path.size(); ++i) {
        auto [px, py, pl] = path[i];
        if (px == x && py == y && pl == l) {
            if (i > 0) {
                auto [prevx, prevy, prevl] = path[i - 1];
                if (prevx == x && prevy == y && prevl != pl)
                    return '#';  // VIA
            }
            return '*';  // Routed path
        }
    }
    return '.';
}

void printGridLayers(const vector<vector<vector<int>>>& grid,
                     const vector<vector<vector<bool>>>& vias,
                     const vector<tuple<int, int, int>>& path,
                     tuple<int, int, int> start,
                     tuple<int, int, int> target) {
    int layers = grid.size(), rows = grid[0].size(), cols = grid[0][0].size();

    for (int l = 0; l < layers; ++l) {
        cout << "\nLayer " << l << ":\n";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                char symbol = getCellSymbol(i, j, l, path, start, target, vias);
                cout << symbol << " ";
            }
            cout << "\n";
        }
    }
}

int computeTotalCost(const vector<tuple<int, int, int>>& path,
                     const vector<vector<vector<int>>>& grid) {
    int totalCost = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        auto [x, y, l] = path[i];
        totalCost += grid[l][x][y];

        // Check if a VIA was used
        if (i > 0) {
            auto [px, py, pl] = path[i - 1];
            if (pl != l)
                totalCost += VIA_COST;
        }
    }
    return totalCost;
}

int main() {
    int rows = 4, cols = 4, layers = 2;
    vector<vector<vector<int>>> grid = {
        { // Layer 0
            {1, 3, 1, 2},
            {2, 4, 5, 7},
            {3, 1, 2, 1},
            {1, 2, 1, 6}
        },
        { // Layer 1
            {4, 2, 1, 1},
            {3, 6, 2, 3},
            {2, 1, 2, 5},
            {1, 4, 3, 1}
        }
    };

    vector<vector<vector<bool>>> vias(rows, vector<vector<bool>>(cols, vector<bool>(layers, false)));
    vias[1][3][0] = true;
    vias[3][1][0] = true;

    int x1, y1, l1, x2, y2, l2;

    cout << "Enter START coordinates (x y layer): ";
    cin >> x1 >> y1 >> l1;

    cout << "Enter TARGET coordinates (x y layer): ";
    cin >> x2 >> y2 >> l2;

    if (!isValid(x1, y1, rows, cols) || l1 < 0 || l1 >= layers ||
        !isValid(x2, y2, rows, cols) || l2 < 0 || l2 >= layers) {
        cout << "Invalid input! Coordinates out of bounds.\n";
        return 1;
    }

    tuple<int, int, int> start = {x1, y1, l1};
    tuple<int, int, int> target = {x2, y2, l2};

    auto path = dijkstra3D(grid, vias, start, target);

    if (!path.empty()) {
        cout << "\nRouted Path:\n";
        for (auto [x, y, l] : path) {
            cout << "(" << x << "," << y << ") [L" << l << "] -> ";
        }
        cout << "END\n";

        int cost = computeTotalCost(path, grid);
        cout << "Total Routing Cost: " << cost << endl;

        printGridLayers(grid, vias, path, start, target);
    }

    return 0;
}

