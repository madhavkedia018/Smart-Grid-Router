#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <climits>
#include <stack>
#include <bits/stdc++.h>
using namespace std;

struct Cell {
    int x, y;
    int cost;
    Cell(int _x, int _y, int _cost) : x(_x), y(_y), cost(_cost) {}
    bool operator>(const Cell& other) const {
        return cost > other.cost;
    }
};

bool isValid(int x, int y, int rows, int cols) {
    return x >= 0 && y >= 0 && x < rows && y < cols;
}

vector<pair<int, int>> dijkstra(vector<vector<int>>& grid, pair<int, int> start, pair<int, int> target) {
    int rows = grid.size();
    int cols = grid[0].size();

    vector<vector<int>> dist(rows, vector<int>(cols, INT_MAX));
    vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols, {-1, -1}));

    priority_queue<Cell, vector<Cell>, greater<Cell>> pq;
    pq.push(Cell(start.first, start.second, grid[start.first][start.second]));
    dist[start.first][start.second] = grid[start.first][start.second];

    vector<pair<int, int>> directions = {{0,1},{1,0},{-1,0},{0,-1}};

    while (!pq.empty()) {
        Cell current = pq.top(); pq.pop();
        int x = current.x, y = current.y;

        for (auto dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;

            if (isValid(nx, ny, rows, cols)) {
                int newCost = dist[x][y] + grid[nx][ny];
                if (newCost < dist[nx][ny]) {
                    dist[nx][ny] = newCost;
                    parent[nx][ny] = {x, y};
                    pq.push(Cell(nx, ny, newCost));
                }
            }
        }
    }

    // Reconstruct path
    vector<pair<int, int>> path;
    if (dist[target.first][target.second] == INT_MAX) {
        cout << "No path found!\n";
        return path;
    }

    pair<int, int> p = target;
    while (p != make_pair(-1, -1)) {
        path.push_back(p);
        p = parent[p.first][p.second];
    }
    reverse(path.begin(), path.end());
    return path;
}


int main() {
    int rows = 5, cols = 5;
    vector<vector<int>> grid = {
        {1, 1, 1, 1, 1},
        {1, 99, 99, 99, 1},
        {1, 1, 1, 99, 1},
        {99, 99, 1, 99, 1},
        {1, 1, 1, 1, 1}
    };

    pair<int, int> start = {0, 0};
    pair<int, int> target = {4, 4};

    auto path = dijkstra(grid, start, target);

    if (!path.empty()) {
        cout << "Shortest path (cost = " << grid[start.first][start.second];
        for (size_t i = 1; i < path.size(); i++)
            cout << " + " << grid[path[i].first][path[i].second];
        cout << "):\n";

        for (auto [x, y] : path)
            cout << "(" << x << ", " << y << ") ";
        cout << endl;
    }

    return 0;
}
