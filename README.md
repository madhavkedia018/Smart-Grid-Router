# Grid-Based VLSI Router

A C++ project that simulates a multi-layer grid router for VLSI interconnects using graph algorithms. The tool supports multi-net routing, stacked via transitions, cost-aware Dijkstra pathfinding, and layered visualizations, making it ideal for exploring real-world physical design constraints in ASIC design.

---

## Project Overview

This router models a chip as a 3D grid where:
- Each cell has a routing cost (e.g., congestion or metal resistance)
- Vias allow vertical movement between layers, incurring extra cost
- Multiple nets must be routed while avoiding overlaps

The project uses an extended Dijkstra’s algorithm to find optimal paths across layers and nets.

---

## Features

### Core Capabilities

* Multi-Layer Routing Grid
  - Supports arbitrary number of layers, rows, and columns
  - Each cell on each layer has an independent cost

* Multi-Net Routing Support
  - Takes user-defined source and target for each net
  - Uses grid cell blocking to prevent overlaps between nets

* Via Modeling
  - Vias enable vertical connections between adjacent layers
  - Cost of via transitions is customizable
  - Vias span all layers at fixed (x, y) positions

* 3D Dijkstra’s Algorithm
  - Finds cost-optimal paths through cells and layers
  - Supports combined via and intra-layer movements

* ASCII-Based Visualization
  - Layer-by-layer terminal visualization of routed nets
  - Each net assigned a unique character (A, B, C, ...)

* Blockage Handling
  - Blocks cells already used by routed nets to avoid conflicts

---

## Example Input Flow

1. Input grid size and number of layers
2. Input routing cost per cell for each layer
3. Input via positions on the 2D plane
4. Input number of nets with their start and end coordinates (x, y, layer)
5. Visualize the routed nets across all layers

---

## Output Sample

```
Layer 0:
. . A A A
. . A . .
. . A . .
. B B B B
. . . . .

Layer 1:
. . . . .
. A A . .
. . A . .
. . . . B
. . . B B
```

- Net A originates on Layer 0 and continues on Layer 1 through a stacked via
- Net B also spans from Layer 0 to Layer 1 using a different path and via


---



## Future Scope

- [ ] Optimized Routing Scheduler
  - Prioritize nets to maximize the number of successfully routed connections
  - Apply greedy heuristics or backtracking to reduce cost while maximizing routability

- [ ] File I/O Support
  - Load cell costs, vias, and netlists from external files
  - Export routed grid to a layout or JSON format

- [ ] Obstacle Modeling
  - Add user-defined obstacles and reserved areas within layers

- [ ] Graphical Visualization
  - Interactive GUI or Web-based visualizer for better debugging

- [ ] Routing Statistics Report
  - Number of vias used, total path length, average cost per net


