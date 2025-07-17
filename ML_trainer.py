# Train ML Model to Predict Netlist Routability

import pandas as pd
import xgboost as xgb
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, mean_squared_error
import joblib

# Load features and results
features_df = pd.read_csv("features.csv")
results_df = pd.read_csv("router_results.csv")

# Merge on netlist_id
merged_df = features_df.merge(results_df, on="netlist_id")

# Aggregating features per netlist
agg_df = merged_df.groupby("netlist_id").agg({
    "manhattan_dist": ["mean", "max"],
    "layer_diff": "mean",
    "is_horizontal_dominant": "sum",
    "nets": "first",
    "routed": "first",
    "total_cost": "first"
})
agg_df.columns = ['_'.join(col).strip() for col in agg_df.columns.values]
agg_df.reset_index(inplace=True)

# Define input and output
X = agg_df[[
    "manhattan_dist_mean", "manhattan_dist_max",
    "layer_diff_mean", "is_horizontal_dominant_sum"
]]
y = agg_df["routed_first"]  # Can be changed to total_cost_first for regression

# Train-test split
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train model
model = xgb.XGBRegressor(objective="reg:squarederror", n_estimators=100)
model.fit(X_train, y_train)

# Evaluate
y_pred = model.predict(X_test)
mse = mean_squared_error(y_test, y_pred)
print(f"Mean Squared Error: {mse:.2f}")

# Save model
joblib.dump(model, "router_predictor_model.pkl")
print("Model saved to router_predictor_model.pkl")
