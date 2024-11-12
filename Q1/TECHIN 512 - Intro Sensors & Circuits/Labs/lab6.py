# this section will be for fitting lines to the 
# data found for 0C, Room Temp, and 100C

import matplotlib.pyplot as plt
from scipy.optimize import curve_fit


temps = [0, 35, 100]
res = [1, 2, 3]

plt.scatter(temps, res, color = "blue")

def log_func(temps, a, b): 
    return a * np.log(x) + b

params_log, _ = curve_fit(log_func, x, y) 
a_log, b_log = params_log 
y_log = log_func(temps, a_log, b_log) 
plt.plot(temps, y_log, color='red', label='Logarithmic Fit')

poly_params = np.polyfit(x, y, 2) 
poly_func = np.poly1d(poly_params) 
y_poly = poly_func(x) 
plt.plot(x, y_poly, color='green', label='Polynomial Fit (degree 2)')

plt.title('Scatter Plot with Logarithmic and Polynomial Regression') 
plt.xlabel('X Axis') 
plt.ylabel('Y Axis') 
plt.legend() 
plt.grid(True)
plt.show()