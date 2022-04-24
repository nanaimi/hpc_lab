#   M.L. for High Performance Computing Lab @USI & @ETHZ - malik.lechekhab@usi.ch 
#   Data extraction method
"""
    read_csv_graph(path_file_adj, path_file_coords)

Extract the data located in the csv `path_file_adj` and `path_file_coords`.
"""
function read_csv_graph(path_file_adj, path_file_coords)
    # Steps
    #   1.  Load the .csv files
    #       see readdlm(...)
    adj_list, _ = readdlm(path_file_adj, ',', header=true, Int)
    coords, _ = readdlm(path_file_coords, ',', header=true, Float64)

    length_adj, _ = size(adj_list)
    nr_nodes, _ = size(coords)
    #   2.  Construct the adjacency matrix A
    A = zeros(Float64, nr_nodes, nr_nodes)

    for i in 1:length_adj
        A[adj_list[i,1],adj_list[i,2]] += 1
    end

    A = (A + transpose(A)) / 2

    #   3.  Visualize and save the result 
    #       use drawGraph(A, coords)

    draw_graph(A, coords)

    #   4.  Return the matrix A and the coordinates 
    #       return(A, coords)
    return(A, coords)
end
