
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Util.h"


/**
  * Create random large matrix in binary on disk.
  * Return the temporary file name.
  */
void randomFileMatrix(int rows, int cols, const char * fileName) {

  // Initialize the random generator
  srand((unsigned) time(NULL));

  FILE * fp;
  fp = fopen (fileName, "wb");

  int r, c;
  for (r = 0; r < rows; ++r) {
    for (c = 0; c < cols; ++c) {
      float cell = ((float)rand() / (float)RAND_MAX);
      fwrite (&cell, sizeof(float), 1, fp); 
    }
  }

  fclose(fp);
}


/**
  * Get a row from the file backed matrix.
  * The user must delete the reference.
  * No bounds checks and fp must be open, YOLO
  */
float * getRow(int rowNum, int cols, FILE *fp) {
  
  float * rowBuffer1 = (float *) malloc(cols * sizeof(float));
  
  // Get row 1
  int offset = rowNum * cols * sizeof(float);
  fseek(fp, offset, SEEK_SET);
  size_t success = fread((void*)rowBuffer1, sizeof(float), cols, fp);
	//if (success != cols) log_err("Error reading rowNum %d (%d of %d)", rowNum, success, cols);

  return rowBuffer1;
}

float cosine (const float * row1, const float * row2, int size) {

  float dot = 0.0F;
  float mag1 = 0.0F;
  float mag2 = 0.0F;
  int i;
  for (i = 0; i < size; ++i) {
    dot += row1[i] * row2[i];
    mag1 += pow(row1[i], 2);
    mag2 += pow(row2[i], 2);
  }

  return dot / (sqrt(mag1) * sqrt(mag2));
}

/**
  * To a bunch of sample on the file backed matrix.
  * Return the average distance.
  */
float local_samples(FILE * fp, int samples, float distance, int rows, int cols) {
 
  unsigned long int avg_dist = 0L;
  int s;
  for (s = 0; s < samples; ++s) {
    // Sample the first row
    int rowid1 = rand() % rows ;
    
    // Sample the second row
    int jump = rand() % (int) rows * distance;
    int direction = (rand() % 2 == 1) ? 1 : -1;
    int rowid2 = MAX(MIN(rowid1 + jump*direction, rows - 1), 0); 

    // Fetch the two rows
    const float * row1 = getRow(rowid1, cols, fp);
    const float * row2 = getRow(rowid2, cols, fp);

    float sim = cosine(row1, row2, cols);
    
    avg_dist += abs(rowid1 - rowid2);
    // Free the row because room was allocated for it
    free((void*)(row1));
    free((void*)(row2));
  }

  return (float) avg_dist / samples;
}




int main(int argc, char** argv) {

  clock_t tic, toc, diff;

  // Default arguments
  char  file[50] = "/tmp/mymatrix.mat";
  int rows = 10000;
  int cols = 5;
  int samples = 10000000;
  float distance = .2F;

  // Process commandline args
  int i;
  for (i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-r") == 0) {
      rows = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-c") == 0) {
      cols  = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-s") == 0) {
      samples  = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-d") == 0) {
      distance  = atof(argv[++i]);
    }
    else if (strcmp(argv[i],"-f") == 0) {
      strcpy(file, argv[++i]);
    }
    else {
      log_info("Usage: ./a.out -f <tempmatrix> -c <cols> -r <rows> -d <distance> -s <samples>");
      exit(1);
    }
  }

  log_info("Running with parameters: -f %s -c %d -r %d -d %f -s %d", file, cols, rows, distance, samples);


  log_info("Creating a matrix of size %dx%d", rows, cols);
  tic = clock();
  randomFileMatrix(rows, cols, file);
  toc = clock();
  log_timer(tic, toc, "Time to create matrix");
	long matrixbuildtime = toc - tic;

  // Now openng the matrix file
  FILE * fp = fopen(file, "rb");

  log_info("Starting the sampling %d.", samples);
  tic = clock();
  float dist = local_samples(fp, samples, distance, rows, cols);
  toc = clock();

  fclose(fp);

  log_info("Finished sampling. Avg distance: %f", dist);
  log_timer(tic, toc, "Time to make %d samples", samples);

	long sampletime = toc - tic;
	

	// Write output to stdout according to script expectation
	// avgdist|matrixbuildtime(msecs)|sampletime(msecs)
	printf("%f|%lf|%lf", dist, 1000.0*matrixbuildtime/CLOCKS_PER_SEC, 1000.0*sampletime/CLOCKS_PER_SEC);
  
  return 0;
}




