mutable struct Node2
    successors::Vector{Node2}
    name::String
    color::Symbol

    Node2(name::String) = new([], name, :white)
end


function dfs(node::Node2, result::Vector{Any})
    node.color = :green

    for successor in node.successors
        if successor.color == :white
            dfs(successor, result)
        end
    end

    append!(result, [node])
    return result
end


function top_sort(G::Vector{Node2})# why is this part there? ::Vector{Node2}
    result = []
    for node in G
        if node.color == :white
            result = dfs(node, result)
        end
    end
    return reverse(result)
end

A = Node2("Write Code")
B = Node2("Compile Code")
C = Node2("Run Tests")
D = Node2("Deploy Application")

A.successors = [B]
B.successors = [C]
C.successors = [D]

result = top_sort([A]) 

for line in result
    println(line.name)
end
