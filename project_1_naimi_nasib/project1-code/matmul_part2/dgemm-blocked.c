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

const char* dgemm_desc = "Naive, three-loop dgemm.";  

// what we want to do:
// 1) divide matrices into blocks
// 2) iterate over blocks and perform:
//    1) load submatrices
//    2) compute new block CIJ w/ naiveMM 
//    3) store block in original C
// end

/* This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are lda-by-lda matrices stored in column-major format.
 * On exit, A and B maintain their input values. */    
void square_dgemm (int n, double* A, double* B, double* C) {
  // TODO: Implement the blocking optimization

  int s = (n < 12) ? n : 12; // blocksize 
  int numBlocks = (n % s == 0) ? (n / s) : (n / s + 1); // number of blocks

  for (int I = 0; I < numBlocks; ++I) {                 // iterate through row block index of C and A
    for (int J = 0; J < numBlocks; ++J) {               // iterate through column block index of C and B
      // load block C_{I,J}
      const int subblock_I = (I < (numBlocks-1)) ? s : (n % s);
      const int subblock_J = (J < (numBlocks-1)) ? s : (n % s);
      double CIJ[subblock_I*subblock_J];

      for (int i = 0; i < subblock_I; i++) {
        #pragma omp parallel for
        for (int j = 0; j < subblock_J; j++) {
          CIJ[i + j * subblock_J] = C[(I + i) + (J + j) * n];
        }
      }

      for (int K = 0; K < numBlocks; ++K) {
        // Load A_{I,K}
        // Load B_{K,J}
        const int subblock_K = (K < (numBlocks-1)) ? s : (n % s);

        double AIK[subblock_I*subblock_K];
        double BKJ[subblock_K*subblock_J];
        for (int k = 0; k < subblock_K; k++) {
          #pragma omp parallel for
          for (int i = 0; i < subblock_I; i++) {
            AIK[i + k * subblock_K] = A[(I + i) + (K + k) * n];
          }
          #pragma omp parallel for
          for (int j = 0; j < subblock_I; j++) {
            BKJ[k + j * subblock_J] = C[(K + k) + (J + j) * n];
          }
        }

        // Naive MM on subblock
        /* For each row i of A */
        for (int i = 0; i < subblock_I; ++i) {
          /* For each column j of B */
          for (int j = 0; j < subblock_J; ++j) {
            /* Compute C(i,j) */
            double cij = CIJ[i+j*n];
            #pragma omp parallel for
            for( int k = 0; k < subblock_K; k++ ) {
              cij += AIK[i+k*n] * BKJ[k+j*n];
            }
            CIJ[i+j*n] = cij;
          }
        }
      }

      // store block in C
      for (int i = 0; i < subblock_I; i++) {
        #pragma omp parallel for
        for (int j = 0; j < subblock_J; j++) {
          C[(I + i) + (J + j) * n] = CIJ[i + j * subblock_J];
        }
      }
    }
  }
}


/* OLD IMPLEMENTATION*/
// void square_dgemm (int n, double* A, double* B, double* C)
// {
//   // TODO: Copy the code you implemented in part1
//   //       And parallelize it with OpenMP
//   int s = (n < 12) ? n : 12;
//   int numBlocks = (n % s == 0) ? (n / s) : (n / s + 1);
//   for (int K = 0; K < numBlocks; ++K) {
//       for (int J = 0; J < numBlocks; ++J) {
//           #pragma omp parallel for
//           for (int i = 0; i < n; ++i) {
//               int kmax = (K*s + s) > n ? n : (K*s + s);
//               int jmax = (J*s + s) > n ? n : (J*s + s);
//               for (int j = J*s; j < jmax; ++j) {
//                   double cij = C[i + j * n];
//                   for (int k = K*s; k < kmax; k++) {
//                       cij += A[i + k * n] * B[k + j * n];
//                   }
//                   C[i + j * n] = cij;
//               }
//           }
//       }
//   }
// }
