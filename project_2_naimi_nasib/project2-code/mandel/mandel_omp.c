#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include "consts.h"
#include "pngwriter.h"

using namespace std;

unsigned long get_time() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec * 1000000 + tp.tv_usec;
}

int main(int argc, char **argv) {
  ofstream datafile;

  png_data *pPng = png_create(IMAGE_WIDTH, IMAGE_HEIGHT);

  double x, y, x2, y2, cx, cy;
  cy = MIN_Y;

  double fDeltaX = (MAX_X - MIN_X) / (double)IMAGE_WIDTH;
  double fDeltaY = (MAX_Y - MIN_Y) / (double)IMAGE_HEIGHT;

  long nTotalIterationsCount = 0;
  unsigned long nTimeStart = get_time();

  long i, j, n;

  n = 0;
  // do the calculation
  double cabs = 0;

  #pragma omp parallel private(n,x,y,x2,y2,cx,cy,cabs) reduction(+:nTotalIterationsCount)
  {
       #pragma omp for collapse(2)
       for (j = 0; j < IMAGE_HEIGHT; j++) {
              for (i = 0; i < IMAGE_WIDTH; i++) {   
                     cx = MIN_X + i * fDeltaX;
              	     cy = MIN_Y + j * fDeltaY;
                     y = cy;
                     y2 = y * y;
                     x = cx;
                     x2 = x * x;
                     // compute the orbit z, f(z), f^2(z), f^3(z), ...
                     // count the iterations until the orbit leaves the circle |z|=2.
                     // stop if the number of iterations exceeds the bound MAX_ITERS.
                     // >>>>>>>> CODE IS MISSING
                     n = 0;
                     cabs = x2 + y2;
                     while((cabs < 4) && (n < MAX_ITERS)){
                            n = n + 1;
                            y = 2 * x * y + cy;
                            x = x2 - y2 + cx;
                            x2 = x * x;
                            y2 = y * y;
                            cabs = x2 + y2;
                     }
                     nTotalIterationsCount += n;
                     // <<<<<<<< CODE IS MISSING
                     // n indicates if the point belongs to the mandelbrot set
                     // plot the number of iterations at point (i, j)
                     int c = ((long) n * 255) / MAX_ITERS;
                     png_plot (pPng, i, j, c, c, c);
              }
       }
  }

  unsigned long nTimeEnd = get_time();

  // print benchmark data
  printf("Total time:                 %g millisconds\n",
         (nTimeEnd - nTimeStart) / 1000.0);
  printf("Image size:                 %ld x %ld = %ld Pixels\n",
         (long)IMAGE_WIDTH, (long)IMAGE_HEIGHT,
         (long)(IMAGE_WIDTH * IMAGE_HEIGHT));
  printf("Total number of iterations: %ld\n", nTotalIterationsCount);
  printf("Avg. time per pixel:        %g microseconds\n",
         (nTimeEnd - nTimeStart) / (double)(IMAGE_WIDTH * IMAGE_HEIGHT));
  printf("Avg. time per iteration:    %g microseconds\n",
         (nTimeEnd - nTimeStart) / (double)nTotalIterationsCount);
  printf("Iterations/second:          %g\n",
         nTotalIterationsCount / (double)(nTimeEnd - nTimeStart) * 1e6);
  // assume there are 8 floating point operations per iteration
  printf("MFlop/s:                    %g\n",
         nTotalIterationsCount * 8.0 / (double)(nTimeEnd - nTimeStart));

  stringstream ss;
  ss << "t_" << omp_get_max_threads() << "_W_" << IMAGE_WIDTH << "_mandel.csv";
  string file_name = ss.str();
  datafile.open(file_name.c_str());
  datafile << IMAGE_WIDTH 
           << "," << nTotalIterationsCount 
           << "," << (nTimeEnd - nTimeStart) / (double)(IMAGE_WIDTH * IMAGE_HEIGHT)) 
           << "," << (nTimeEnd - nTimeStart) / (double)nTotalIterationsCount)
           << "," << nTotalIterationsCount / (double)(nTimeEnd - nTimeStart) * 1e6)
           << "," << nTotalIterationsCount * 8.0 / (double)(nTimeEnd - nTimeStart)) << ",\n";
  datafile.close();

  png_write(pPng, "mandel_omp.png");
  return 0;
}
