import numpy as np

y = np.array([0.0, 0.0, 0.8, 0.1, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0])
target = np.array([0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])

def MSE(target, y):
    return np.square(target - y).sum() * 0.5

print(MSE(target, y))