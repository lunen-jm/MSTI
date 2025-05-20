
import streamlit as st
import pandas as pd
import numpy as np
import plotly.express as px
import altair as alt
import pydeck as pdk
from datetime import datetime

# Generate some random data for the example
dates = pd.date_range(start="2025-01-01", periods=30, freq="D")
temperature = np.random.normal(25, 5, 30)
humidity = np.random.normal(65, 10, 30)

my_df = pd.DataFrame({
    "Date": dates,
    "Temperature": temperature,
    "Humidity": humidity,
})

st.set_page_config(page_title="Weather Data Dashboard", page_icon= "", layout="wide")

st.title("Weather Data Dashboard")

