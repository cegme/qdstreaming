
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX(a,b) ( ((a) > (b)) ? (a) : (b) )
#define MIN(a,b) ( ((a) < (b)) ? (a) : (b) )

/**
  * Create random large matrix in binary on disk.
  * Return the temporary file name.
  */
void randomFileMatrix(int rows, int cols, const char * fileName) {

  // Initialize the random generator
  srand((unsigned) time(NULL));

  FILE * fp;
  fp = fopen (fileName, "wb");

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      float cell = ((float)rand() / (float)RAND_MAX);
      fwrite (&cell, sizeof(float), sizeof(cell), fp); 
    }
  }

  fclose(fp);
}


/**
  * Get a row from the file backed matrix.
  * The user must delete the reference.
  * No bounds checks and fp must be open, YOLO
  */
const float * getRow(int rowNum, int cols, FILE *fp) {
  
  const float * rowBuffer1 = (const float *) malloc(sizeof(float) * cols);
  
  // Get row 1
  int offset = rowNum * cols * sizeof(float);
  fseek(fp, offset, SEEK_SET);
  fgets((char*)rowBuffer1, sizeof(float)*cols, fp);

  return rowBuffer1;
}

float cosine (const float * row1, const float * row2, int size) {

  float sim = 0.0F;
  float dot = 0.0F;
  float mag1 = 0.0F;
  float mag2 = 0.0F;
  for (int i = 0; i < size; ++i) {
    dot += row1[i] * row2[i];
    mag1 += pow(row1[i], 2);
    mag2 += pow(row2[i], 2);
  }

  return dot / (sqrt(mag1) * sqrt(mag2));
}

/**
  * To a bunch of sample on the file backed matrix
  */
void local_samples(FILE * fp, int samples, float distance, int rows, int cols) {
  
  for (int s = 0; s < samples; ++s) {
    int rowid1 = rand() % rows;
    int jump = rand() % (int) rows * distance;
    int direction = (rand() % 2 == 1) ? 1 : -1;
    int rowid2 = rowid1 + jump*direction;

    const float * row1 = getRow(rowid1, cols, fp);
    const float * row2 = getRow(rowid2, cols, fp);

    float sim = cosine(row1, row2, cols);
   
    // Free the row because room was allocated for it
    for (int j = 0; j < cols; ++j) {
      free((void*)(&row1[j]));
      free((void*)(&row2[j]));
    }
  }

}


/** TODO make inpus parameters */
int main(int argc, char** argv) {

  // Create matrix
  const char * file = "/tmp/mymatrix.mat";
  int rows = 10;
  int cols = 5;
  int samples = 100;
  float distance = .2;

  randomFileMatrix(rows, cols, file);
  FILE * fp = fopen(file, "rb");
  // TODO add timing
  local_samples(fp, samples, distance, rows, cols);
  
  return 0;
}




