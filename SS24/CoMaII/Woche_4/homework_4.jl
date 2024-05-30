"""
This file implements the basic functionality of a union find datastructure using dicts and a mutable node struct.
"""
mutable struct Node
  value::Int
  parent::Union{Node,Nothing}
end


function union_find(partition::Vector{Tuple{Int,Vector{Int}}})::Dict{Int,Node}
  """
  Convert vector of nodes to union find dictionary.
  """
  # 1. First create a dict to save the union structure
  # 2. Loop over the roots in the vector, create empty node and save it in dict
  # 3. If child exists, link it to root and save it
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
  """
  Find root of given node (no argument modification).
  """
  # Visit parent until parent is empty and root found
  while node.parent !== nothing
    node = node.parent
  end
  return node
end


function find_set!(node::Node)::Node
  """
  Find root of given node x and change parent of x to root (WITH argument modification).
  """
  # Visit parent until parent is empty and root found
  # and then update the parent of the node to root
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
  """
  Combine two union find trees.
  """
  # first find the roots and then update parent if roots are not the same
  root1 = find_set!(node1)
  root2 = find_set!(node2)

  if root1 !== root2
    root2.parent = root1
  end
  return nothing
end


function add!(nodes::Dict{Int,Node}, value::Int)::Nothing
  """
  Add a new node to a union find tree.
  """
  # Check if node is already in tree
  # Then add the entry to the dict
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
