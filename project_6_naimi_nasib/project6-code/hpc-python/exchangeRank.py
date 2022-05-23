import numpy as np
from mpi4py import MPI

# get comm, size & rank
comm = MPI.COMM_WORLD
mpi_size = comm.Get_size()
mpi_rank = comm.Get_rank()

subdomain = 4
domain = subdomain + 2

data = np.full((domain, domain), mpi_rank, dtype=int)

dims = 2
dimensions = MPI.Compute_dims(mpi_size, dims)

cart_comm = comm.Create_cart(dimensions, periods=[True,True])
coord2d = cart_comm.Get_coords(mpi_rank)

west, east = cart_comm.Shift(direction = 0, disp = 1)
south, north = cart_comm.Shift(direction = 1, disp = 1)

print("rank:", mpi_rank, " coord:" , coord2d, "north:", north, "east:", east, "south:", south, "west:", west)

# receive buffers and send buffers 
# deliver boundaries to neighbours adn receive theirs
recBufN = np.zeros(subdomain,dtype=int)
recBufS = np.zeros(subdomain,dtype=int)
recBufW = np.zeros(subdomain,dtype=int)
recBufE = np.zeros(subdomain,dtype=int)

sendBufN = data[0,1:domain-1]
sendBufS = data[domain-1,1:domain-1]
sendBufE = np.ascontiguousarray(data[1:domain-1,domain-1])
sendBufW = np.ascontiguousarray(data[1:domain-1,0])

req = []
# north
req.append(cart_comm.Isend([sendBufN, MPI.INT], dest = north, tag = 1))
req.append(cart_comm.Irecv([recBufS, MPI.INT], source = south, tag = 1))

# south
req.append(cart_comm.Isend([sendBufS, MPI.INT], dest = south, tag = 2))
req.append(cart_comm.Irecv([recBufN, MPI.INT], source = north, tag = 2))

# east
req.append(cart_comm.Isend([sendBufE, MPI.INT], dest = east, tag = 3))
req.append(cart_comm.Irecv([recBufW, MPI.INT], source = west, tag = 3))

# west
req.append(cart_comm.Isend([sendBufW, MPI.INT], dest = west, tag = 4))
req.append(cart_comm.Irecv([recBufE, MPI.INT], source = east, tag = 4))

MPI.Request.Waitall(req)

data[domain-1,1:domain-1] = recBufS
data[0,1:domain-1] = recBufN
data[1:domain-1,0] = recBufW
data[1:domain-1,domain-1] = recBufE

if mpi_rank == 0:
    print(data)
