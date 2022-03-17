#include <omp.h>
#include "walltime.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>

#define NUM_ITERATIONS 100

// Example benchmarks
// 0.008s ~0.8MB
// #define N 100000
// 0.1s ~8MB
// #define N 1000000
// 1.1s ~80MB
// #define N 10000000
// 13s ~800MB
// #define N 100000000
// 127s 16GB
//#define N 1000000000
#define EPSILON 0.1

using namespace std;

int main(int argc, char** argv) {

  ofstream datafile;

  int N = atoi(argv[1]);

  double time_serial, time_start = 0.0;
  double *a, *b;

  // Allocate memory for the vectors as 1-D arrays
  a = new double[N];
  b = new double[N];

  // Initialize the vectors with some values
  for (int i = 0; i < N; i++) {
    a[i] = i;
    b[i] = i / 10.0;
  }

  long double alpha = 0;
  // serial execution
  // Note that we do extra iterations to reduce relative timing overhead
  time_start = wall_time();
  for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
    alpha = 0.0;
    for (int i = 0; i < N; i++) {
      alpha += a[i] * b[i];
    }
  }
  time_serial = wall_time() - time_start;
  cout << "Serial execution time = " << time_serial << " sec" << endl;

  long double alpha_parallel = 0;
  double time_red = 0;
  double time_critical = 0;

  time_red = wall_time();
  //   TODO: Write parallel version (2 ways!)
  //   i.  Using reduction pragma
  for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
    alpha_parallel = 0.0;
    #pragma omp parallel for default(shared) reduction(+:alpha_parallel)
    for (int i = 0; i < N; i++) {
      alpha_parallel += a[i] * b[i];
    }
  }
  time_red = wall_time() - time_red;

  alpha_parallel = 0;
  time_critical = wall_time();

// #pragma omp parallel private(local_sum) 
//             {
//                 local_sum = 0;
//                 #pragma omp for
//                 {
//                 for( i=0; i<N; i ++) {
//                     local_sum = a[i] * b[i] + local_sum;
//                 }
//                 }
//                 #pragma omp critical 
//                 {
//                 alpha_parallel += local_sum;
//                 }
//             }
//             }

  //   ii. Using  critical pragma
  for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
    alpha_parallel = 0.0;
    double local_sum;
    int i, tid;
    #pragma omp parallel private(local_sum) 
    {
      local_sum = 0;
      #pragma omp for 
      {
        for(i=0; i<N; i ++) {
          local_sum = a[i] * b[i] + local_sum;
        }
      }
      #pragma omp critical
      {
        alpha_parallel += local_sum;
      }
    }
  }  
  
  time_critical = wall_time() - time_critical;


  if ((fabs(alpha_parallel - alpha) / fabs(alpha_parallel)) > EPSILON) {
    cout << "parallel reduction: " << alpha_parallel << ", serial: " << alpha
         << "\n";
    cerr << "Alpha not yet implemented correctly!\n";
    exit(1);
  }
  cout << "Parallel dot product = " << alpha_parallel
       << " time using reduction method = " << time_red
       << " sec, time using critical method " << time_critical << " sec"
       << endl;

  datafile.open ("timedata.csv");
  datafile << time_serial << "," << time_red << "," << time_critical << ",\n";
  datafile.close();

  // De-allocate memory
  delete[] a;
  delete[] b;

  return 0;
}
