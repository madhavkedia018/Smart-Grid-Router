import itertools
import tkinter as tk
from tkinter import messagebox, simpledialog, ttk
import matplotlib.pyplot as plt
import numpy as np

# Constants
INF = float('inf')
VIA_COST = 20
SYMBOLS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

# Grid definition (3 layers, 6x6)
grid = [
    [
        [1, 2, 3, 4, 5, 1],
        [3, 6, 5, 2, 2, 1],
        [2, 1, 1, 7, 6, 3],
        [1, 4, 2, 3, 1, 3],
        [4, 1, 1, 9, 6, 1],
        [1, 3, 4, 3, 2, 5]
    ],
    [
        [3, 2, 1, 4, 3, 5],
        [1, 3, 1, 5, 2, 6],
        [1, 4, 2, 1, 1, 2],
        [2, 1, 1, 1, 7, 3],
        [1, 4, 2, 1, 3, 2],
        [1, 5, 6, 1, 7, 3]
    ],
    [
        [1, 4, 1, 1, 3, 1],
        [2, 1, 2, 1, 2, 2],
        [1, 1, 2, 1, 4, 1],
        [1, 2, 3, 1, 3, 1],
        [5, 6, 3, 1, 4, 1],
        [5, 1, 2, 1, 3, 1]
    ]
]

has_via = [[False]*6 for _ in range(6)]
for x, y in [(1,1), (2,2), (3,3), (4,4)]:
    has_via[x][y] = True

ROWS, COLS, LAYERS = 6, 6, 3

def is_valid(x, y):
    return 0 <= x < ROWS and 0 <= y < COLS

def dijkstra3D(start, target, blocked):
    from queue import PriorityQueue
    sx, sy, sl = start
    tx, ty, tl = target

    if blocked[sl][sx][sy] or blocked[tl][tx][ty]:
        return []

    dist = [[[INF]*COLS for _ in range(ROWS)] for _ in range(LAYERS)]
    parent = [[[(None)]*COLS for _ in range(ROWS)] for _ in range(LAYERS)]
    dist[sl][sx][sy] = grid[sl][sx][sy]

    pq = PriorityQueue()
    pq.put((grid[sl][sx][sy], sx, sy, sl))

    dirs = [(0,1),(1,0),(-1,0),(0,-1)]
    while not pq.empty():
        cost, x, y, l = pq.get()
        if (x, y, l) == (tx, ty, tl):
            break

        for dx, dy in dirs:
            nx, ny = x + dx, y + dy
            if is_valid(nx, ny) and not blocked[l][nx][ny]:
                new_cost = dist[l][x][y] + grid[l][nx][ny]
                if new_cost < dist[l][nx][ny]:
                    dist[l][nx][ny] = new_cost
                    parent[l][nx][ny] = (x, y, l)
                    pq.put((new_cost, nx, ny, l))

        if has_via[x][y]:
            for dl in [-1, 1]:
                nl = l + dl
                if 0 <= nl < LAYERS and not blocked[nl][x][y]:
                    new_cost = dist[l][x][y] + VIA_COST + grid[nl][x][y]
                    if new_cost < dist[nl][x][y]:
                        dist[nl][x][y] = new_cost
                        parent[nl][x][y] = (x, y, l)
                        pq.put((new_cost, x, y, nl))

    if dist[tl][tx][ty] == INF:
        return []

    path = []
    cur = (tx, ty, tl)
    while cur:
        path.append(cur)
        cur = parent[cur[2]][cur[0]][cur[1]]
    return path[::-1]

def compute_cost(path):
    cost = 0
    for i in range(len(path)):
        x, y, l = path[i]
        cost += grid[l][x][y]
        if i > 0 and path[i-1][2] != l:
            cost += VIA_COST
    return cost

def route_nets_permutation(nets):
    best_result = {"cost": INF, "routed": 0, "paths": []}
    for perm in itertools.permutations(nets):
        blocked = [[[False]*COLS for _ in range(ROWS)] for _ in range(LAYERS)]
        paths = []
        total_cost = 0
        for net in perm:
            path = dijkstra3D(net["start"], net["end"], blocked)
            if not path:
                break
            for x, y, l in path:
                blocked[l][x][y] = True
            total_cost += compute_cost(path)
            paths.append((net["name"], path))
        if len(paths) > best_result["routed"] or (len(paths) == best_result["routed"] and total_cost < best_result["cost"]):
            best_result = {"cost": total_cost, "routed": len(paths), "paths": paths}
    return best_result

def visualize(paths):
    fig, axs = plt.subplots(1, LAYERS, figsize=(15, 5))
    layers = [[['.']*COLS for _ in range(ROWS)] for _ in range(LAYERS)]
    for idx, (name, path) in enumerate(paths):
        symbol = SYMBOLS[idx] if idx < len(SYMBOLS) else '*'
        for x, y, l in path:
            layers[l][x][y] = symbol
    for l in range(LAYERS):
        axs[l].imshow(np.zeros((ROWS, COLS)), cmap='gray', vmin=0, vmax=1)
        for i in range(ROWS):
            for j in range(COLS):
                axs[l].text(j, i, layers[l][i][j], ha='center', va='center', color='red', fontsize=12)
        axs[l].set_title(f'Layer {l}')
        axs[l].set_xticks(range(COLS))
        axs[l].set_yticks(range(ROWS))
    plt.tight_layout()
    plt.show()

def main_gui():
    nets = []

    def add_net():
        try:
            x1, y1, l1 = int(sx.get()), int(sy.get()), int(sl.get())
            x2, y2, l2 = int(tx.get()), int(ty.get()), int(tl.get())
            if not (is_valid(x1, y1) and is_valid(x2, y2) and 0 <= l1 < LAYERS and 0 <= l2 < LAYERS):
                raise ValueError("Invalid coordinates")
            nets.append({"name": f"Net{len(nets)+1}", "start": (x1,y1,l1), "end": (x2,y2,l2)})
            status.set(f"Added Net{len(nets)}: ({x1},{y1},L{l1}) to ({x2},{y2},L{l2})")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def run_router():
        if not nets:
            messagebox.showinfo("Info", "No nets to route")
            return
        result = route_nets_permutation(nets)
        messagebox.showinfo("Routing Complete", f"Routed {result['routed']} nets with total cost {result['cost']}")
        visualize(result["paths"])

    root = tk.Tk()
    root.title("VLSI Grid Router")

    frm = ttk.Frame(root, padding=10)
    frm.grid()

    ttk.Label(frm, text="Start X Y Layer").grid(column=0, row=0)
    sx, sy, sl = tk.Entry(frm), tk.Entry(frm), tk.Entry(frm)
    sx.grid(column=1, row=0); sy.grid(column=2, row=0); sl.grid(column=3, row=0)

    ttk.Label(frm, text="Target X Y Layer").grid(column=0, row=1)
    tx, ty, tl = tk.Entry(frm), tk.Entry(frm), tk.Entry(frm)
    tx.grid(column=1, row=1); ty.grid(column=2, row=1); tl.grid(column=3, row=1)

    ttk.Button(frm, text="Add Net", command=add_net).grid(column=0, row=2)
    ttk.Button(frm, text="Run Router", command=run_router).grid(column=1, row=2)

    status = tk.StringVar()
    ttk.Label(frm, textvariable=status).grid(column=0, row=3, columnspan=4)

    root.mainloop()

if __name__ == "__main__":
    main_gui()
