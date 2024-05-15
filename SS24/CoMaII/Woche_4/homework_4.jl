mutable struct Node
  value::Int
  parent::Union{Node,Nothing}
end


function union_find(partition::Vector{Tuple{Int,Vector{Int}}})::Dict{Int,Node}
  dict = Dict{Int,Node}()
  for (root, elements) in partition
    root_node = Node(root, nothing)
    dict[root] = root_node
    for element in elements
      if element !== root
        element_node = Node(element, root_node)
        dict[element] = element_node
      end
    end
  end
  return dict
end


function find_set(node::Node)::Node
  while node.parent !== nothing
    node = node.parent
  end
  return node
end


function find_set!(node::Node)::Node
  root_node = node

  while root_node.parent !== nothing
    root_node = root_node.parent
  end
  if node !== root_node
    node.parent = root_node
  end
  return root_node
end


function union!(node1::Node, node2::Node)::Nothing
  root1 = find_set!(node1)
  root2 = find_set!(node2)

  if root1 !== root2
    root2.parent = root1
  end
  return nothing
end


function add!(nodes::Dict{Int,Node}, value::Int)::Nothing
  @assert haskey(nodes, value) == false
  nodes[value] = Node(value, nothing)
  return nothing
end


# partition = [(1, [1, 2, 3]), (4, [4, 5]), (6, [6, 7, 8, 9])]
# 
# nodes = union_find(partition)
# 
# union!(nodes[1], nodes[4])
# 
# print("\n")
# 
# add!(nodes, 11)
# 
# for element in nodes
#   println(element)
# end
