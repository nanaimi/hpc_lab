import numpy as np
from mpi4py import MPI

# get comm, size & rank
comm = MPI.COMM_WORLD
mpi_size = comm.Get_size()
mpi_rank = comm.Get_rank()

# Cyclic Boundary
if (mpi_rank == mpi_size - 1):
    right = 0
else:
    right = mpi_rank + 1

if mpi_rank == 0:
    left = mpi_size - 1
else:
    left = mpi_rank - 1

sum = np.zeros(1,dtype='i')
snd_buf = np.array(mpi_rank,dtype='i')
rcv_buf = np.zeros(1,dtype='i')
for i in range(mpi_size):
    comm.Send([snd_buf, MPI.INT], dest=right)
    comm.Recv([rcv_buf, MPI.INT], source=left)
    sum += rcv_buf
    snd_buf = rcv_buf
    

print(f'''{mpi_rank}: {np.asscalar(sum)}''')