//******************************************
// operators.cpp
// based on min-app code written by Oliver Fuhrer, MeteoSwiss
// modified by Ben Cumming, CSCS
// *****************************************

// Description: Contains simple operators which can be used on 3d-meshes

#include <iostream>

#include <mpi.h>

#include "data.h"
#include "operators.h"
#include "stats.h"

namespace operators {

// input: s, gives updated solution for f
// only handles interior grid points, as boundary points are fixed
// those inner grid points neighbouring a boundary point, will in the following
// be referred to as boundary points and only those grid points
// only neighbouring non-boundary points are called inner grid points
void diffusion(const data::Field &s, data::Field &f)
{
    using data::options;
    using data::domain;

    using data::bndE;
    using data::bndW;
    using data::bndN;
    using data::bndS;

    using data::buffE;
    using data::buffW;
    using data::buffN;
    using data::buffS;

    using data::y_old;

    double alpha = options.alpha;
    double beta = options.beta;

    int nx = domain.nx;
    int ny = domain.ny;
    int iend  = nx - 1;
    int jend  = ny - 1;

    // TODO exchange the ghost cells
    // try overlapping computation and communication
    // by using  MPI_Irecv and MPI_Isend.
    MPI_Request request[8];
    std::fill_n(request, 8, MPI_REQUEST_NULL);
    MPI_Status status[8];
    if(domain.neighbour_north>=0) {
        // ...
        for(int i = 0; i <= iend; i++){
            buffN[i] = s(i,jend);
        }
        int size = buffN.length();
        MPI_Irecv(&bndN[0], size, MPI_DOUBLE, domain.neighbour_north, domain.neighbour_north, domain.comm_cart, &request[0]);
        MPI_Isend(&buffN[0], size, MPI_DOUBLE, domain.neighbour_north, domain.rank, domain.comm_cart, &request[1]);
    }

    if(domain.neighbour_south>=0) {
       // ...
       for(int i = 0; i <= iend; i++){
            buffS[i] = s(i,0);
        }
       int size = bndS.length();
       MPI_Irecv(&bndS[0], size, MPI_DOUBLE, domain.neighbour_south, domain.neighbour_south, domain.comm_cart, &request[2]);
       MPI_Isend(&buffS[0], size, MPI_DOUBLE, domain.neighbour_south, domain.rank, domain.comm_cart, &request[3]);
    }
    if(domain.neighbour_east>=0) {
      // ...
      for(int j = 0; j <= jend; j++){
            buffE[j] = s(iend,j);
        }
      int size = bndE.length();
      MPI_Irecv(&bndE[0], size, MPI_DOUBLE, domain.neighbour_east, domain.neighbour_east, domain.comm_cart, &request[4]);
      MPI_Isend(&buffE[0], size, MPI_DOUBLE, domain.neighbour_east, domain.rank, domain.comm_cart, &request[5]);
    }
    if(domain.neighbour_west>=0) {
      // ...
      for(int j = 0; j <= jend; j++){
            buffW[j] = s(0,j);
        }
      int size = bndW.length();
      MPI_Irecv(&bndW[0], size, MPI_DOUBLE, domain.neighbour_west, domain.neighbour_west, domain.comm_cart, &request[6]);
      MPI_Isend(&buffW[0], size, MPI_DOUBLE, domain.neighbour_west, domain.rank, domain.comm_cart, &request[7]);
    }

    // the interior grid points
    for (int j=1; j < jend; j++) {
        for (int i=1; i < iend; i++) {
            f(i,j) = -(4. + alpha) * s(i,j)               // central point
                                    + s(i-1,j) + s(i+1,j) // east and west
                                    + s(i,j-1) + s(i,j+1) // north and south
                                    + alpha * y_old(i,j)
                                    + beta * s(i,j) * (1.0 - s(i,j));
        }
    }

    // TODO: wait on the receives from the outstanding MPI_Irecv using MPI_Waitall.
    // ...
    MPI_Waitall(8, request, status);

    // the east boundary
    {
        int i = nx - 1;
        for (int j = 1; j < jend; j++)
        {
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i-1,j) + s(i,j-1) + s(i,j+1)
                        + alpha*y_old(i,j) + bndE[j]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }
    }

    // the west boundary
    {
        int i = 0;
        for (int j = 1; j < jend; j++)
        {
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i+1,j) + s(i,j-1) + s(i,j+1)
                        + alpha * y_old(i,j) + bndW[j]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }
    }

    // the north boundary (plus NE and NW corners)
    {
        int j = ny - 1;

        {
            int i = 0; // NW corner
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i+1,j) + s(i,j-1)
                        + alpha * y_old(i,j) + bndW[j] + bndN[i]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }

        // north boundary
        for (int i = 1; i < iend; i++)
        {
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i-1,j) + s(i+1,j) + s(i,j-1)
                        + alpha*y_old(i,j) + bndN[i]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }

        {
            int i = nx-1; // NE corner
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i-1,j) + s(i,j-1)
                        + alpha * y_old(i,j) + bndE[j] + bndN[i]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }
    }

    // the south boundary
    {
        int j = 0;

        {
            int i = 0; // SW corner
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i+1,j) + s(i,j+1)
                        + alpha * y_old(i,j) + bndW[j] + bndS[i]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }

        // south boundary
        for (int i = 1; i < iend; i++)
        {
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i-1,j) + s(i+1,j) + s(i,j+1)
                        + alpha * y_old(i,j) + bndS[i]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }

        {
            int i = nx - 1; // SE corner
            f(i,j) = -(4. + alpha) * s(i,j)
                        + s(i-1,j) + s(i,j+1)
                        + alpha * y_old(i,j) + bndE[j] + bndS[i]
                        + beta * s(i,j) * (1.0 - s(i,j));
        }
    }

    // Accumulate the flop counts
    // 8 ops total per point
    stats::flops_diff +=
        + 12 * (nx - 2) * (ny - 2) // interior points
        + 11 * (nx - 2  +  ny - 2) // NESW boundary points
        + 11 * 4;                                  // corner points
}

} // namespace operators
