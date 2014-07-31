#! /usr/bin/python

import csv
import matplotlib.pyplot as plt
import math
from math import log
import numpy as np
import pandas

with open('resultslin114-01.log', 'r') as csvfile:
    rows = csv.reader(csvfile, delimiter='|'); next(rows, None) # Skip Header
    r = [[int(z[0]),int(z[1]), int(z[2]),float(z[3]),int(z[4]),float(z[5]),float(z[6]),float(z[7])] for z in sorted(rows)]
    #print map(lambda x: float(x[3]), r)
    X1 = 1
    Y1 = 7
    p1 = plt.plot([log(x[X1]) for x in r if x[3] == 0.001 and x[2] == 1000000],
              [log(y[Y1]) for y in r if y[3] == 0.001 and y[2] == 1000000], 'bo')
    p2 = plt.plot([log(x[X1]) for x in r if x[3] == 0.01 and x[2] == 1000000],
              [log(y[Y1]) for y in r if y[3] == 0.01 and y[2] == 1000000], 'yo')
    p3 = plt.plot([log(x[X1]) for x in r if x[3] == 0.1 and x[2] == 1000000],
              [log(y[Y1]) for y in r if y[3] == 0.1 and y[2] == 1000000], 'ro')
    p4 = plt.plot([log(x[X1]) for x in r if x[3] == 0.3 and x[2] == 1000000],
              [log(y[Y1]) for y in r if y[3] == 0.3 and y[2] == 1000000], 'ko')
    p5 = plt.plot([log(x[X1]) for x in r if x[3] == 0.5 and x[2] == 1000000],
              [log(y[Y1]) for y in r if y[3] == 0.5 and y[2] == 1000000], 'go')
    p6 = plt.plot([log(x[X1]) for x in r if x[3] == 0.7 and x[2] == 1000000],
              [log(y[Y1]) for y in r if y[3] == 0.7 and y[2] == 1000000], 'mo')
    p7 = plt.plot([log(x[X1]) for x in r if x[3] == 0.9 and x[2] == 1000000],
              [log(y[Y1]) for y in r if y[3] == 0.9 and y[2] == 1000000], 'co')
    plt.legend(["0.001", "0.01", "0.1", "0.3", "0.5", "0.7", "0.9"])
    #plt.legend([p1, p2, p3, p4, p5, p6, p7], ["0.001", "0.01", "0.1", "0.3", "0.5", "0.7", "0.9"])
    plt.show()
