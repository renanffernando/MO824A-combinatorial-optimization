import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from scipy.interpolate import interp1d

# # sem j = 500
# x = np.array([100, 200, 300, 400, 600])
# y = np.array([3.263497, 18.304874, 60.564301, 97.239349, 222.136828])

# com j = 500
x = np.array([100, 200, 300, 400, 500, 600])
y = np.array([3.263497, 18.304874, 60.564301, 97.239349, 50.536251, 222.136828])

model = LinearRegression().fit(x.reshape(-1,1),y)

predict = model.predict(x.reshape(-1,1))


# plot linearRegression prediction
plt.scatter(x, predict)
for i in range(0, len(x)):
    plt.plot(x[i:i+2], predict[i:i+2], 'ro-')


x_new = np.linspace(x.min(), x.max(),500)

f = interp1d(x, y, kind='quadratic')
y_smooth = f(x_new)

plt.plot(x_new,y_smooth)
plt.scatter(x, y)

plt.show()
