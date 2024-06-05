const TupleSet = Vector{Tuple{Int, Int}}

function TupleSet(V::Vector{Tuple{Int, Int}})
    V_sorted = sort(V)
    return V_sorted
end


struct Partition
    Sets::Vector{TupleSet}
end


function Partition(V::TupleSet)::Partition
    sets = Vector{TupleSet}()

    for tuple in V
    new_set = TupleSet([tuple])

    append!(sets, [new_set])
    end

    # Gib die Liste der Partitionen zurück
    return Partition(sets)
end


function MakeSet(P::Partition, (x, y)::Tuple{Int, Int})
    if ([(x, y)] in P.Sets) != true
        push!(P.Sets, [(x, y)])
    end
    
end


function FindSet(P::Partition, (x,y)::Tuple{Int,Int})
    index = -1

    for i = 1:length(P.Sets)
        if (x, y) in P.Sets[i]
            index = i
        end
    end

    if index > 0
        sorted = sort(P.Sets[index])
        return sorted[1]
    end
    
    return -1
end


function union!(P::Partition, (x1, y1)::Tuple{Int,Int}, (x2, y2)::Tuple{Int,Int})
    set1 = FindSet(P, (x1, y1))
    set2 = FindSet(P, (x2, y2))

    set1_index = 0
    set2_index = 0

    for i = 1:length(P.Sets)
        if set1 == P.Sets[i][1]
            set1_index = i
        end

        if set2 == P.Sets[i][1]
            set2_index = i
        end
    end

    # println(set1_index, set2_index)

    new_set = sort(append!(P.Sets[set1_index], P.Sets[set2_index]))
    deleteat!(P.Sets, min(set1_index, set2_index))
    deleteat!(P.Sets, max(set1_index, set2_index) - 1)

    append!(P.Sets, [new_set])
end


S = TupleSet([(0, 3), (0, 1), (1, 3), (0, 0)])
P = Partition(S)
println("Initial Partition: ", P)

MakeSet(P, (1, 4))
println("Add new element (1, 4): ", P)

MakeSet(P, (100, 3))
println("Add new element (100, 3): ", P)

union!(P, (100, 3), (1, 4))
println("Merging (100, 3) and (1, 4): ", P)

union!(P, (100, 3), (0, 0))
println("Merging (100, 3) and (0, 0): ", P)