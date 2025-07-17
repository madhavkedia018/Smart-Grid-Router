# Predict Best Net Order Using Trained ML Model

import json
import itertools
import pandas as pd
import joblib
from train_ml_model import model, X
from feature_extraction_utils import extract_features
from router_runner import write_router_input, run_router

# Load model
model = joblib.load("router_predictor_model.pkl")

# Load one test netlist (e.g., netlist_1001.json)
with open("netlists/netlist_1001.json") as f:
    netlist = json.load(f)

# Generate permutations (limit to 50 for speed)
orders = list(itertools.permutations(netlist))[:50]

# Score each order
best_order = None
best_pred = -1

for order in orders:
    # Fake feature aggregation for this ordering
    features = extract_features(list(order), netlist_id=9999)
    df = pd.DataFrame(features)
    agg = {
        "manhattan_dist_mean": df["manhattan_dist"].mean(),
        "manhattan_dist_max": df["manhattan_dist"].max(),
        "layer_diff_mean": df["layer_diff"].mean(),
        "is_horizontal_dominant_sum": df["is_horizontal_dominant"].sum(),
    }
    x_df = pd.DataFrame([agg])
    pred = model.predict(x_df)[0]

    if pred > best_pred:
        best_pred = pred
        best_order = list(order)

# Route the best order
write_router_input(best_order)
routed, cost = run_router()

print("Best predicted order routed:", routed)
print("Total cost:", cost)
