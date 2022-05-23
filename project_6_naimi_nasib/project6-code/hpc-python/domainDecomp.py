import numpy as np
from mpi4py import MPI

# get comm, size & rank
comm = MPI.COMM_WORLD
mpi_size = comm.Get_size()
mpi_rank = comm.Get_rank()

# Return a balanced distribution of processes per coordinate direction
dims = 2
dimensions = MPI.Compute_dims(mpi_size, dims)

# Create cartesian communicator
cart_comm = comm.Create_cart(dimensions, periods=[True,True])

# Get Cartesian coordinates of rank > Translate ranks to logical coordinates
coord2d = cart_comm.Get_coords(mpi_rank)

# Get neighbouring ranks
west, east   = cart_comm.Shift(direction=0, disp=1)
south, north = cart_comm.Shift(direction=1, disp=1)

print(f'''rank: {mpi_rank}, coord: {coord2d}, north: {north}, east: {east}, south: {south}, west: {west}''')

if mpi_rank == 0:
    print(dimensions)