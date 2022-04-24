#   M.L. for High Performance Computing Lab @USI & @ETHZ - malik.lechekhab@usi.ch 

include("./bench_bisection.jl");
include("./bench_recursive.jl");
include("./bench_metis.jl");

include("./count_cut.jl");

include("./read_csv_graph.jl");
include("./read_mat_graph.jl");

include("./part_coordinate.jl");
include("./part_inertial.jl");
include("./part_metis.jl");
include("./part_spectral.jl");
include("./partition.jl");

include("./plot_fiedler.jl");
include("./plot_partitions.jl");

include("./recursive_bisection.jl");