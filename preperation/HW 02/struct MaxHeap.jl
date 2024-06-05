struct MaxHeap
    keys::Vector{Int}
end


function heapify(keys::Vector{Int}, N::Int, i::Int)
    largest = i

    left = 2 * i
    right = 2 * i + 1

    # try
    # println("I:", keys[i])
    # println("Left:", keys[left])
    # println("Right:", keys[right])
    # catch
    # end

    if left <= N && keys[left] > keys[largest]
        largest = left
    end

    if right <= N && keys[right] > keys[largest]
        largest = right
    end

    if largest != i
        keys[i], keys[largest] = keys[largest], keys[i]

        heapify(keys, N, largest)
    end
end


function BuildHeap(keys::Vector{Int})
    # find last non-leaf node
    last = size(keys)[1] ÷ 2

    for i = reverse(1:last)
        heapify(keys, size(keys)[1], i)
    end
end


function maximum(h::MaxHeap)
    return h.keys[1]
end


function extractMax(h::MaxHeap)
    max = h.keys[1]
    deleteat!(h.keys, 1)
    BuildHeap(h.keys)
    return max
end
 
function increaseKey(h::MaxHeap, i::Int, k::Int)
    if h.keys[i] < k
        h.keys[i] = k
    end

    BuildHeap(h.keys)
end

function insert(h::MaxHeap, k::Int)
    append!(h.keys, k)
    BuildHeap(h.keys)
end


heap = MaxHeap([2, 3])

BuildHeap(heap.keys)

println(heap)