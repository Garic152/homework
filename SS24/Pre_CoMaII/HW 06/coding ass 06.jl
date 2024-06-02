mutable struct Node
    key::Int
    left::Union{Node, Nothing}
    right::Union{Node, Nothing}
    parent::Union{Node, Nothing}
end


function getKeyList(tree::Node)::Vector{Int}
    list = []

    if tree.left !== nothing
        push!(list, getKeyList(tree.left)...)
    end

    push!(list, tree.key)

    if tree.right !== nothing
        push!(list, getKeyList(tree.right)...)
    end

    return list
end


function find(tree::Node, k::Int)::Union{Node, Nothing}
    while tree !== nothing && k != tree.key
        if k < tree.key
            tree = tree.left
        else
            tree = tree.right
        end
    end
    return tree
end


function min(tree::Node)::Int
    while tree.left !== nothing
        tree = tree.left
    end
    return tree.key
end


function isBST(node::Union{Node, Nothing})::Bool
    if node === nothing
        return true
    end

    left_valid = (node.left === nothing || node.key >= node.left.key) && isBST(node.left)
    right_valid = (node.right === nothing || node.key <= node.right.key) && isBST(node.right)

    return left_valid && right_valid
end


function fromString(str::String)::Node
    root = nothing
    current_node = root

    i = 1
    while i <= length(str)
        c = str[i]

        if isdigit(c)
            val = parse(Int, str[i:i])
            i += 1

            new_node = Node(val, nothing, nothing, nothing)
            if root == nothing
                root = new_node
                current_node = root
            else
                if current_node.left === nothing
                    current_node.left = new_node
                else
                    current_node.right = new_node
            end
        end

        elseif c == '('
            current_node = new_node

        elseif c == ')'
            if current_node.parent !== nothing
                current_node = current_node.parent
            end 

        elseif c == ','
        end

        i += 1
    end

    if isBST(root) == false
        println("Der Baum ist kein Suchbaum!")
    end
    return root
end


test = Node(3, 
            Node(2, 
                 Node(1, nothing, nothing, nothing), 
                 Node(2, nothing, nothing, nothing), 
                 nothing), 
            Node(5, 
                 Node(4, nothing, nothing, nothing), 
                 nothing, nothing), 
                 nothing)

println(fromString("3(2(1,2),5(7,))"))
# println(fromString("4(3,5)"))