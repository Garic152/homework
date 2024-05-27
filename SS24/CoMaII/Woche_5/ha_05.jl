mutable struct Node
  value::Union{Char,Nothing}
  freq::Int
  left::Union{Node,Nothing}
  right::Union{Node,Nothing}
end


function getFequencies(input::String)
  frequencies = Dict{Char,Int64}()
  for character in input
    if haskey(frequencies, character) != true
      frequencies[character] = 1
    else
      frequencies[character] = frequencies[character] + 1
    end
  end
  return frequencies
end


function findLowestTwo(q1::Vector{Node}, q2::Vector{Node})::Tuple{Node,Node}
  min_nodes = []

  push!(min_nodes, q1)


  for node in q1
    if node.freq < min_node_1.freq
      min_node_1 = node
    elseif node.freq == min_node_1.freq
      if node.value < min_node_1.value
        min_node_1 = node
      end
    end
  end

  min_node_2 = q2[1]

  for node in q2
    if node.freq < min_node_2.freq
      min_node_2 = node
    elseif node.freq == min_node_2.freq
      if node.value < min_node_2.value
        min_node_2 = node
      end
    end
  end

  deleteat!(q1, findfirst(x -> x == min_node_1, q1))
  deleteat!(q2, findfirst(x -> x == min_node_2, q2))

  return (min_node_1, min_node_2)
end


function huffman_tree(freqs::Dict{Char,Int})::Node
  q1 = Vector{Node}()

  for (key, freq) in freqs
    push!(q1, Node(key, freq, nothing, nothing))
  end

  sort!(q1, by=x -> x.freq)

  q2 = Vector{Node}()
end

q1 = [Node('a', 5, nothing, nothing), Node('b', 9, nothing, nothing)]
q2 = [Node('c', 12, nothing, nothing), Node('d', 13, nothing, nothing)]

println(findLowestTwo(q1, q2))

huffman_tree(getFequencies("Wenn der Physiker nicht weiter weiß, gr¨undet er ein Arbeitskreis"))

huffman_tree(getFequencies("Wenn der Physiker nicht weiter weiß, gr¨undet er ein Arbeitskreis"))
