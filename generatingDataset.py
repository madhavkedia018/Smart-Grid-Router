# Project: ML-Based VLSI Router Optimizer
# This script: Generates random netlists and stores them in JSON format.

import json
import random
import os
import pandas as pd
import math

# Configurable grid size and layers
GRID_WIDTH = 10
GRID_HEIGHT = 10
LAYERS = 2

# Number of netlists to generate
NUM_NETLISTS = 1000
NETS_PER_NETLIST = (6, 10)  # range

OUTPUT_DIR = "netlists"
FEATURE_CSV = "features.csv"
os.makedirs(OUTPUT_DIR, exist_ok=True)

def generate_terminal():
    x = random.randint(0, GRID_WIDTH - 1)
    y = random.randint(0, GRID_HEIGHT - 1)
    z = random.randint(0, LAYERS - 1)
    return (x, y, z)

def generate_netlist(netlist_id):
    netlist = []
    num_nets = random.randint(*NETS_PER_NETLIST)
    used_pairs = set()

    for i in range(num_nets):
        while True:
            src = generate_terminal()
            dst = generate_terminal()
            if src != dst and (src, dst) not in used_pairs and (dst, src) not in used_pairs:
                used_pairs.add((src, dst))
                break

        net = {
            "id": chr(65 + i),
            "src": src,
            "dst": dst
        }
        netlist.append(net)

    return netlist

def extract_features(netlist, netlist_id):
    feature_rows = []
    for net in netlist:
        src = net['src']
        dst = net['dst']
        dx = abs(dst[0] - src[0])
        dy = abs(dst[1] - src[1])
        dz = abs(dst[2] - src[2])

        feature_rows.append({
            "netlist_id": netlist_id,
            "net_id": net['id'],
            "manhattan_dist": dx + dy,
            "layer_diff": dz,
            "is_horizontal_dominant": int(dx >= dy),
            "src_x": src[0], "src_y": src[1], "src_z": src[2],
            "dst_x": dst[0], "dst_y": dst[1], "dst_z": dst[2],
        })
    return feature_rows

# Generate and store all netlists + features
all_features = []
for i in range(NUM_NETLISTS):
    netlist = generate_netlist(i)
    with open(os.path.join(OUTPUT_DIR, f"netlist_{i}.json"), "w") as f:
        json.dump(netlist, f, indent=2)
    all_features.extend(extract_features(netlist, i))

# Save features as CSV
pd.DataFrame(all_features).to_csv(FEATURE_CSV, index=False)
print(f"Generated {NUM_NETLISTS} netlists and extracted features to {FEATURE_CSV}")
