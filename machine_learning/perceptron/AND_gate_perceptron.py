import numpy as np

epsilon =  1e-5 # small number to compare with

X = np.array([[0,0,1],[0,1,1],[1,0,1],[1,1,1]]) # the last 1 is for bias, and the first 2 are inputs (00, 01, 10, 11)
y = np.array([[0],[0],[0],[1]]) # output (AND gate)
W = np.zeros(len(X[0])) # weights (3 weights for 3 inputs), initialized to 0

def step_functnion(x): # step function to return 1 or 0 based on the input
    if x > epsilon:
        return 1   
    else:  
        return 0
    
def perceptron_fit(X, Y, epochs = 10): # 10 epochs by default
    global W # use the global W variable defined above
    eta = 0.7 # learning rate

    for x in range(epochs): # for each epoch
        print("epoch=", x, "==============")

        for i in range(len(X)): # for each row in X (00, 01, 10, 11)
            predict = step_functnion(np.dot(X[i], W)) # predict the output based on the weights and inputs  
                                                    #dot function means multiply and add the values in the array
            error = Y[i] - predict # calculate the error Y[i] is the actual output, predict is the predicted output
            W += eta * error * X[i] # update the weights based on the error and the learning rate
            print("X=", X[i], "y=", Y[i], "predict=", predict, "answer=", Y[i], "W=", W)
        
        print("=====================================")


def perceptron_predict(X, Y):
    global W # use the global W variable defined above

    for x in X:
        print(x[0], x[1], "->", step_functnion(np.dot(x, W))) # print the predicted output based on the weights and inputs


perceptron_fit(X, y, 10) # train the perceptron for 10 epochs
perceptron_predict(X, y) # predict the output based on the trained weights