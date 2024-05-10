mutable struct Heap{T<:Real}
  data::Vector{T}
  comparator::Function

  function Heap(data::Vector{T}; comparator::Function)::Heap{T} where {T<:Real}
    @assert hasmethod(comparator, Tuple{typeof(data[1]),typeof(data[1])})
    return new{T}(data, comparator)
  end

  function Heap(data::Vector{T})::Heap{T} where {T<:Real}
    @assert hasmethod((x, y) -> x >= y, Tuple{typeof(data[1]),typeof(data[1])})
    return new{T}(data, (x, y) -> x >= y)
  end
end


function is__Heap(heap::Heap{T})::Bool where {T<:Real}
  for i in length(heap.data):-1:2
    if i % 2 == 1
      if heap.comparator(heap.data[Int(round(i / 2 - 1, RoundNearestTiesUp))], heap.data[i]) == false
        return false
      end
    else
      if heap.comparator(heap.data[Int(i / 2)], heap.data[i]) == false
        return false
      end
    end
  end

  return true
end


function heapify!(heap::Heap{T})::Heap{T} where {T<:Real}
  for i = 1:length(heap.data)
    j = i

    while j > 1 && heap.comparator(heap.data[j], heap.data[Int(round(j / 2, RoundDown))])
      temp = heap.data[Int(round(j / 2, RoundDown))]
      heap.data[Int(round(j / 2, RoundDown))] = heap.data[j]
      heap.data[j] = temp

      j = Int(round(j / 2, RoundDown))
    end
  end

  return heap
end


function heap(data::Vector{T}; comparator::Function)::Heap{T} where {T<:Real}
  heap = Heap(data; comparator)
  heap = heapify!(heap)
  @assert is__Heap(heap) "Heap is no heap!"
  return heap
end


function heapSort!(heap::Heap{T})::Heap{T} where {T<:Real}
  if length(heap.data) == 0
    return []
  end

  for i = 2:length(heap.data)
    j = i

    while j > 1 && heap.comparator(heap.data[j-1], heap.data[j])
      temp = heap.data[j]
      heap.data[j] = heap.data[j-1]
      heap.data[j-1] = temp
      j -= 1
    end
  end

  return heap
end


function heapSort!(data::Vector{T}; comparator::Function)::Vector{T} where {T<:Real}
  if length(data) == 0
    return []
  end

  for i = 2:length(data)
    j = i

    while j > 1 && comparator(data[j-1], data[j])
      temp = data[j]
      data[j] = data[j-1]
      data[j-1] = temp
      j -= 1
    end
  end

  return data
end


function maximum(heap::Heap{T})::T where {T<:Real}
  if length(heap.data) == 0
    return nothing
  end

  return pop!(heapSort!(heap).data)
end
