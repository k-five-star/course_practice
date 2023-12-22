x = 10 # random value for x
learning_rate = 0.01 # learning rate
precision = 0.0001 # precision
max_iterations = 10000 # maximum number of iterations

# loss function
loss_function = lambda x: (x-3)**2 + 10 # (x-3)^2 + 10

# derivative of loss function => gradient
gradient = lambda x: 2*(x-3)

# gradient descent
for i in range(max_iterations):
    x = x - learning_rate * gradient(x)
    
    if i % 100 == 0:
        print("손실함수 값({}) x값({})".format(loss_function(x), x))

print("최소값: {}".format(x))
                    