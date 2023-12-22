from sklearn.linear_model import Perceptron

# Define training data
X = [[0, 0], [0, 1], [1, 0], [1, 1]] # inputs
y = [0, 0, 0, 1] # AND gate

# Create perceptron object
p = Perceptron(tol=1e-3, random_state=0) # tol is the stopping criterion, random_state is the seed

# Train the perceptron
p.fit(X, y) # fit the perceptron with the inputs and outputs

# Predict the output
print(p.predict(X))