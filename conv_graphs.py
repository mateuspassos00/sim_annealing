import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# Load data
df = pd.read_csv("results/100/cs2/conv_r3")

# Plot the results
fig = go.Figure()
fig = make_subplots(specs=[[{"secondary_y": True}]])
fig.add_trace(go.Scatter(x=list(range(len(df))), y=df['current_cost'], name='Current Cost'))
fig.add_trace(go.Scatter(x=list(range(len(df))), y=df['best_cost'], name='Best Cost'))
fig.add_trace(go.Scatter(x=list(range(len(df))), y=df['temperature'], name='Temperature'), secondary_y=True)
fig.update_layout(template="simple_white", font=dict(size=15), 
                  title_text=f"100 Cities, CS2 - Best Cost: {df['best_cost'].iloc[-1]:.2f}",
                  width=800, title_x=0.5, height=450, xaxis_title='Iteration',
                  yaxis_title='Distance')
fig.update_yaxes(title_text="Temperature", secondary_y=True, automargin=True)
fig.update_xaxes(automargin=True)
fig.show()