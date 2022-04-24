#   M.L. for High Performance Computing Lab @USI & @ETHZ - malik.lechekhab@usi.ch 
#   Main file of Project 4

#   I/O packages
using DelimitedFiles, MAT
#   Math packages
using Arpack, LinearAlgebra, Metis, Random, SparseArrays, Statistics
#   Plot packages
using Graphs, SGtSNEpi, GLMakie, Colors, CairoMakie, PrettyTables

#   Tools
include("./Tools/add_paths.jl");

#   Generate adjacency matrices and vertices coordinates
path_file = "/path/to/project/Sources/Meshes/2D/airfoil1.mat"
A, coords = read_mat_graph(path_file);


#   Run benchmark
benchmark_bisection()
benchmark_recursive()
benchmark_metis()


