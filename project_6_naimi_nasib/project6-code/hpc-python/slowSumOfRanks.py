from mpi4py import MPI

# get comm, size & rank
comm = MPI.COMM_WORLD
mpi_size = comm.Get_size()
mpi_rank = comm.Get_rank()

# Cyclic boundary
if (mpi_rank == mpi_size - 1):
    right = 0
else:
    right = mpi_rank + 1

if mpi_rank == 0:
    left = mpi_size - 1
else:
    left = mpi_rank - 1

sum = 0
snd_buf = mpi_rank
rcv_buf = 0
for i in range(mpi_size):
    comm.send(snd_buf, dest=right)
    rcv_buf = comm.recv(source=left)
    sum += rcv_buf
    snd_buf = rcv_buf

print(f'''{mpi_rank}: {sum}''')