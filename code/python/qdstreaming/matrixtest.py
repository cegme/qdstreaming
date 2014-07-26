

import numpy as np
import scipy as sp 

import logging
import logging.handlers
import sys
from sys import stderr
import timeit
from timeit import timeit
import tempfile
from tempfile import mkstemp

from numpy.random import choice 
from numpy import memmap
from scipy.spatial.distance import cosine

class MatrixTest:
    def __init__(self, num_rows=10000, num_cols=5000, chunk=10, tmpdir='/tmp', cell_size=4):
        self.NUM_ROWS = num_rows
        self.NUM_COLS = num_cols
        self.CELL_SIZE = cell_size
        self.chunk = chunk
        print >> stderr, "Setting up matrix of size {NUM_ROWS}x{NUM_COLS}".format(NUM_COLS=self.NUM_COLS, NUM_ROWS=self.NUM_ROWS)
        self.tmpdir = tmpdir
        self.setup_matrix(tmpfile=mkstemp(dir=tmpdir, suffix='.mat') )


    def setup_matrix(self, tmpfile=mkstemp(suffix='.mat')):

        print >> stderr, 'The temp file: {tmpfile}'.format(tmpfile=tmpfile)

        for i in range(self.chunk):
            fp = memmap(tmpfile[1], 
                        dtype='float32', 
                        mode='r+', 
                        shape=(self.NUM_ROWS/self.chunk, self.NUM_COLS), 
                        offset=i*self.CELL_SIZE*self.NUM_ROWS/self.chunk*self.NUM_COLS)
            fp[:] = np.random.randn(self.NUM_ROWS/self.chunk, self.NUM_COLS)
            print >> stderr, "finished chunk {i}".format(i=i)

        self.matrix = memmap(tmpfile[1],
                        dtype='float32',
                        mode='r',
                        shape=(self.NUM_ROWS, self.NUM_COLS), 
                        offset=0)
        print >> stderr, 'Created the matrix!'


    def local_samples(self, samples=100, distance=0.2):
        randint = np.random.randint
        rand = np.random.rand
        matrix = self.matrix
        NUM_ROWS = self.NUM_ROWS
        tot_samples = samples

        avg_dist = 0L
        while samples > 0:
            row1 = randint(NUM_ROWS)

            jump = choice([1, -1])

            # Row two should be close lets say withing 20%
            jump *= NUM_ROWS * distance
            row2 = max(min(row1 + jump, NUM_ROWS - 1), 0)
            
            cosine(matrix[row1, :], matrix[row2, :])
            avg_dist += abs(row1 - row2)

            samples -= 1
        print >> stderr, 'Avg Dist: {avg_dist}'.format(avg_dist=avg_dist/tot_samples)


if __name__ == '__main__':
    """Test to see if locality of operations in a matrix affects time.
    
    Build a large matrix, time how long it takes to do millions of operations between _close_
    elements and far elements.

    Usage:
        time python -mtimeit -v -n 5 -r 2 -s 'import matrixtest; m = matrixtest.MatrixTest(num_rows=1000000, num_cols=5000, chunk=100)' 'm.local_samples(100000, .001)'
        time python -mtimeit -v -n 5 -r 2 -s 'import matrixtest; m = matrixtest.MatrixTest(num_rows=1000000, num_cols=5000, chunk=100)' 'm.local_samples(100000, .95)'
    """
    
    # matrix = np.random.randn(NUM_ROWS, NUM_COLS)
    setup =  'import matrixtest\n'
    setup += 'm = matrixtest.MatrixTest(num_rows=1000000, num_cols=5000, chunk=100)'

    timeit(stmt='local_samples(100000)', setup=setup)


