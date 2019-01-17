
import mnist_utils as mu
import dataset as ds
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt

import keras
from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import Dense, Dropout, BatchNormalization
from keras.optimizers import RMSprop



print(tf.VERSION)
print(tf.keras.__version__)
print(keras.__version__)

def load_dataset(batch_size):
    """
    Load Accordo5 handwritten digit dataset
    Included processing: normalization, randomization, split

    Parameter
    ---------
    None

    Return
    ------
    training_set, dev_test, test_set: DATASET
        Returns the dataset split in training set, cross validation set and
        test set

    """
    # set useful variables
    n_classes = 10
    # load data set
    print("[INFO] Loading A5 dataset ... ")
    dataset = mu.MNIST()
    dataset, datalabels = dataset.load("./data/A5_DB_v2-images.idx3-ubyte",
                                       "./data/A5_DB_v2-labels.idx1-ubyte")
    # display examples of images used as inputs to neural network
    #print("[INFO] Display some examples ... ")
    #print("[INFO] Close window containing figures to continue")
    #img_set = mu.MNIST()
    #img_set.setImagesLabels(dataset, datalabels)
    #img_set.showDigits()

    # Randomize
    set = ds.DATASET(dataset, datalabels, n_classes, batch_size)
    print("[INFO] Randomizing dataset ... ")
    set.permutation()

    # use a fixed seed to ease debug
    # np.random.seed(0)

    # Split
    print("[INFO] Splitting dataset ... ")
    percentage = 80
    (X_train, y_train,
     X_test, y_test) = set.splitkeras(reordered=True,
                                 training_percentage=percentage,
                                 test_percentage=(100 - percentage))
    training_set = ds.DATASET(X_train, y_train, n_classes, batch_size)
    test_set = ds.DATASET(X_test, y_test, n_classes, batch_size)

    normalization_method = 'z_score_std'
    #normalization_method='min_max_scaling'

    #print("[INFO] Normalization datasets( training/dev/test) using %s method ... " % normalization_method)
    #p1, p2 = training_set.normalize(normalization_method)
    #print ("[INFO] p1:%f p2:%f" %(p1, p2))
    #test_set.normalize(normalization_method, p1, p2)
    return training_set, test_set

def main():
    batch_size = 128
    num_classes = 10
    epochs = 100

    # Load dataset and process it
    training_set, test_set = load_dataset(batch_size)
    n_input = np.shape(training_set.data)[1]
    n_hidden = 512
    #hidden_activation = tf.nn.sigmoid
    hidden_activation = tf.nn.tanh
    num_classes = max(training_set.labels) - min(training_set.labels) + 1
    #output_activation = tf.nn.sigmoid
    output_activation = tf.nn.softmax
    l2_reg = 0.01
    learning_rate = 0.01
    training_epochs = 100

    tbCallBack = keras.callbacks.TensorBoard(log_dir='./Graph', histogram_freq=0,
                                write_graph=True, write_images=True)
    model = Sequential()
    model.add(Dense(units=n_hidden, input_shape=(784,), name="Layer1"))
    # Should be done btw pre-activation and activation
    model.add(BatchNormalization(name='BN'))
    model.add(Dense(units=n_hidden, activation=hidden_activation))
    model.add(Dropout(0.5))

    #model.add(Dense(n_hidden, activation=hidden_activation))
    #model.add(Dropout(0.3))

    model.add(Dense(num_classes, activation=output_activation))

    model.summary()

    model.compile(loss='sparse_categorical_crossentropy',
                  #optimizer=RMSprop(),
                  optimizer="sgd",
                  metrics=['accuracy'])

    history = model.fit(training_set.data, training_set.labels,
                        batch_size=batch_size,
                        epochs=epochs,
                        verbose=1,
                        validation_data=(test_set.data, test_set.labels),
                        callbacks=[tbCallBack])

    score = model.evaluate(test_set.data, test_set.labels, verbose=0)
    print('Test loss:', score[0])
    print('Test accuracy:', score[1])

    # serialize model to JSON
    model_json = model.to_json()
    with open("model.json", "w") as json_file:
        json_file.write(model_json)
    # serialize weights to HDF5
    #model.save_weights("model.h5")
    model.save("model.h5")
    print("Saved model to disk")

if __name__ == "__main__":
    main()