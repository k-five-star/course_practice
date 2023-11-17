import matplotlib.pyplot as plt
from sklearn import linear_model

reg_model = linear_model.LinearRegression()
height = [[174], [152], [138], [128], [186]]
weight = [71, 55, 46, 38, 88]

reg_model.fit(height, weight)

print(reg_model.predict([[165]]))

plt.scatter(height, weight, color='black')

plt.plot(height, reg_model.predict(height), color='blue', linewidth=3)

plt.show()