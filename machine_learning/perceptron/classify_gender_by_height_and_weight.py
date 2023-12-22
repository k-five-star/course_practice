from sklearn.linear_model import Perceptron

# Define training data
X = [
    [160, 55], 
    [163, 43], 
    [165, 48], 
    [170, 80], 
    [175, 76], 
    [180, 70],
]

y = [0, 0, 0, 1, 1, 1] # sex => 0 is woman, 1 is man

# Create perceptron object
p = Perceptron(tol = 1e-3, random_state = 753) # tol is the stopping criterion, random_state is the seed
p.fit(X, y) # fit the perceptron with the inputs and outputs
print(p.predict([[170, 90], [150, 45]])) # Predict the output