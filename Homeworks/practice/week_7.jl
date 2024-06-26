# using Debugger

heights_and_balances = Dict{Real,Array{Real,1}}()

mutable struct Node
    key::Real
    left::Union{Nothing,Node}
    right::Union{Nothing,Node}
    parent::Union{Nothing,Node}

    Node() = new(Inf, nothing, nothing, nothing)
    Node(key::Real) = new(key, nothing, nothing, nothing)
end


function calculate_balances(node::Union{Nothing,Node})
    if node.left
        calculate_balances(node.left)
    elseif node.right
        calculate_balances(node.right)
    end
end


function calculate_heights(node::Union{Nothing,Node})
    if node.left === nothing && node.right === nothing
        heights_and_balances[node.key] = [0, 0]
        return 0
    end

    if node.left !== nothing
        left_height = calculate_heights(node.left)
    else
        left_height = 0
    end

    if node.right !== nothing
        right_height = calculate_heights(node.right)
    else
        right_height = 0
    end

    # Update the node's height based on the heights of its children
    heights_and_balances[node.key] = [0, max(left_height, right_height) + 1]

    return max(left_height, right_height) + 1
end


function height(node::Union{Nothing,Node})::Int
    if node === nothing
        return 0
    else
        return max(height(node.left), height(node.right)) + 1
    end
end


function insert!(T::Node, p::Real)::Node
    x = T
    y = nothing

    p = Node(p)

    while x !== nothing
        y = x

        if p.key < x.key
            x = x.left
        else
            x = x.right
        end
    end

    p.parent = y

    if y === nothing
        T = p
    elseif p.key < y.key
        y.left = p
    else
        y.right = p
    end

    return y
end

test = Node(5)

insert!(test, 4)
insert!(test, 6)
insert!(test, 7)

println(test)


calculate_heights(test)

println(heights_and_balances)
