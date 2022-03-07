/* 
    Please include compiler name below (you may also include any other modules you would like to be loaded)

COMPILER= gnu

    Please include All compiler flags and libraries as you want them run. You can simply copy this over from the Makefile's first few lines
 
CC = cc
OPT = -O3
CFLAGS = -Wall -std=gnu99 $(OPT)
MKLROOT = /opt/intel/composer_xe_2013.1.117/mkl
LDLIBS = -lrt -Wl,--start-group $(MKLROOT)/lib/intel64/libmkl_intel_lp64.a $(MKLROOT)/lib/intel64/libmkl_sequential.a $(MKLROOT)/lib/intel64/libmkl_core.a -Wl,--end-group -lpthread -lm

*/

#include <omp.h>

const char* dgemm_desc = "Naive, three-loop dgemm.";

/* This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are lda-by-lda matrices stored in column-major format.
 * On exit, A and B maintain their input values. */    
void square_dgemm (int n, double* A, double* B, double* C)
{
  // TODO: Copy the code you implemented in part1
  //       And parallelize it with OpenMP
  int s = (n < 12) ? n : 12;
  int numBlocks = (n % s == 0) ? (n / s) : (n / s + 1);
  for (int K = 0; K < numBlocks; ++K) {
      for (int J = 0; J < numBlocks; ++J) {
          #pragma omp parallel for
          for (int i = 0; i < n; ++i) {
              int kmax = (K*s + s) > n ? n : (K*s + s);
              int jmax = (J*s + s) > n ? n : (J*s + s);
              for (int j = J*s; j < jmax; ++j) {
                  double cij = C[i + j * n];
                  for (int k = K*s; k < kmax; k++) {
                      cij += A[i + k * n] * B[k + j * n];
                  }
                  C[i + j * n] = cij;
              }
          }
      }
  }
}
