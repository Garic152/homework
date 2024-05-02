mutable struct Node
  key::Int
  left::Union{Node,Nothing}
  right::Union{Node,Nothing}
end


function node(key::Int)
  return Node(key, nothing, nothing)
end


const MaybeNode = Union{Node,Nothing}


function node(key::Int, left::MaybeNode, right::MaybeNode)
  return Node(key, left, right)
end


function getKeys(node::Node)::Vector{Int}
  keys = []

  if node.left !== nothing
    append!(keys, getKeys(node.left))
  end

  if node.right !== nothing
    append!(keys, getKeys(node.right))
  end

  append!(keys, node.key)

  return keys
end


function height(node::Node)::Int
  if node.left === nothing && node.right === nothing #ERROR HERE
    return 1
  end

  return max(height(node.left), height(node.right)) + 1
end


function tree2vec(node::Node)::Vector{Union{Int,Nothing}}
  if node === nothing
    return nothing
  end

  max_nodes = 2^(height(node)) - 1

  array = [node.key]

  queue = [node]

  while length(array) < max_nodes
    current = popfirst!(queue)

    if current === nothing
      push!(array, nothing)
      push!(array, nothing)

      push!(queue, nothing)
      push!(queue, nothing)
    end

    if node.left === nothing
      push!(array, nothing)
    else
      push!(array, node.left.key)
      push!(queue, node.left)
    end

    if node.right === nothing
      push!(array, nothing)
    else
      push!(array, node.right.key)
      push!(queue, node.left)
    end
  end

  return array
end


function vec2tree(vec::Vector{Union{Int,Nothing}})
  if length(vec) == 0
    return nothing
  end

  root = node(vec[1])

  queue = [root]

  i = 2

  while i < length(vec)
    current = popfirst!(queue)

    if vec[i] === nothing
    else
      current.left = node(vec[i])
      push!(queue, current.left)
    end
    i += 1

    if vec[i] === nothing
    else
      current.right = node(vec[i])
      push!(queue, current.right)
    end
    i += 1
  end

  return root
end

x = node(1)
y = node(2)
z = node(3, x, y)
println(z)

println(getKeys(z))
println(height(z))

# tree = vec2tree([4, 3, nothing, 1, 2, nothing, nothing])

u = node(4, z, nothing)
println(tree2vec(u))
