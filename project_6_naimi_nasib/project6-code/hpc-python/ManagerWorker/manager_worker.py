from mandelbrot_task import *
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
from mpi4py import MPI # MPI_Init and MPI_Finalize automatically called
import numpy as np
import sys
import time

# some parameters
MANAGER = 0       # rank of manager
TAG_TASK      = 1 # task       message tag
TAG_TASK_DONE = 2 # tasks done message tag
TAG_DONE      = 3 # done       message tag

def manager(comm, tasks):
    """
    The manager.

    Parameters
    ----------
    comm : mpi4py.MPI communicator
        MPI communicator
    tasks : list of objects with a do_task() method perfroming the task
        List of tasks to accomplish

    Returns
    -------
    ... ToDo ...
    """
    mpi_status = MPI.Status()
    mpi_size = comm.Get_size()
    numTasks = len(tasks)

    finishedTasks = []

    for i in range(1,mpi_size):
        taskId  = i-1
        comm.send(obj=tasks[taskId], dest=i, tag=TAG_TASK)

    for taskInd in range(mpi_size,numTasks):
        finishedTasks.append(comm.recv(source=MPI.ANY_SOURCE, tag=TAG_TASK_DONE, status=mpi_status))
        comm.send(obj=tasks[taskInd], dest=mpi_status.Get_source(), tag=TAG_TASK)

    for i in range(1, mpi_size):
        finishedTasks.append(comm.recv(source=MPI.ANY_SOURCE, tag=TAG_TASK_DONE, status=mpi_status))
        comm.send(obj=None, dest=i, tag=TAG_DONE)

    return finishedTasks

def worker(comm):

    """
    The worker.

    Parameters
    ----------
    comm : mpi4py.MPI communicator
        MPI communicator
    """
    counter = 0
    mpi_rank = comm.Get_rank()
    mpi_status = MPI.Status()
    while True:
        task = comm.recv(source=MANAGER, tag=MPI.ANY_TAG, status=mpi_status)
        if mpi_status.Get_tag() == TAG_DONE:
            break
        task.do_work()
        counter += 1
        comm.send(obj=task, dest= MANAGER, tag=TAG_TASK_DONE)
    return counter

def readcmdline(rank):
    """
    Read command line arguments

    Parameters
    ----------
    rank : int
        Rank of calling MPI process

    Returns
    -------
    nx : int
        number of gridpoints in x-direction
    ny : int
        number of gridpoints in y-direction
    ntasks : int
        number of tasks
    """
    # report usage
    if len(sys.argv) != 4:
        if rank == MANAGER:
            print("Usage: manager_worker nx ny ntasks")
            print("  nx     number of gridpoints in x-direction")
            print("  ny     number of gridpoints in y-direction")
            print("  ntasks number of tasks")
        sys.exit()

    # read nx, ny, ntasks
    nx = int(sys.argv[1])
    if nx < 1:
        sys.exit("nx must be a positive integer")
    ny = int(sys.argv[2])
    if ny < 1:
        sys.exit("ny must be a positive integer")
    ntasks = int(sys.argv[3])
    if ntasks < 1:
        sys.exit("ntasks must be a positive integer")

    return nx, ny, ntasks


if __name__ == "__main__":

    # get COMMON WORLD communicator, size & rank
    comm    = MPI.COMM_WORLD
    size    = comm.Get_size()
    my_rank = comm.Get_rank()
    TasksDoneByWorker = 0

    # report on MPI environment
    if my_rank == MANAGER:
        print(f"MPI initialized with {size:5d} processes")

    # read command line arguments
    nx, ny, ntasks = readcmdline(my_rank)

    # start timer
    timespent = - time.perf_counter()

    # trying out ... YOUR MANAGER-WORKER IMPLEMENTATION HERE ...
    if my_rank == MANAGER:
        x_min = -2.
        x_max  = +1.
        y_min  = -1.5
        y_max  = +1.5
        M = mandelbrot(x_min, x_max, nx, y_min, y_max, ny, ntasks)
        tasks = M.get_tasks()
        for task in tasks:
            task.do_work()
        m = M.combine_tasks(tasks)
        plt.imshow(m.T, cmap="gray", extent=[x_min, x_max, y_min, y_max])
        plt.savefig("mandelbrot.png")
    else:
        TasksDoneByWorker = worker(comm)

    # stop timer
    timespent += time.perf_counter()
    if my_rank != MANAGER:
        print(f'''Process {my_rank} completed {TasksDoneByWorker} tasks''')

    # inform that done
    if my_rank == MANAGER:
        print(f"Run took {timespent:f} seconds")
        print("Done.")
        print("### END TIME ###")
        print(f'''{size}, {timespent},''')



