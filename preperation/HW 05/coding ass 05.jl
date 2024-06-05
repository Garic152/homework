struct FactorTree
    value::Int
    left::Union{Int, FactorTree}
    right::Union{Int, FactorTree}
end


function isprime(v::Int)
    if v == 1
        return true
    end

    for i in 2:sqrt(v)
        if v % i == 0
            return false
        end
    end
    return v > 1
end


function factors(v::Int)
    factors = []

    if v == 1
        return 1
    end

    for i in 2:sqrt(v)
        if v % i == 0
            append!(factors, i)
        end
    end

    distances = []

    for factor in factors
        append!(distances, (v/factor) - factor)
    end

    return Int(factors[argmin(distances)])
end


function FactorTree(v::Int)::FactorTree
    if isprime(v)
        return FactorTree(v, 0, 0)
    end

    factor = factors(v)

    return FactorTree(v, FactorTree(factor), FactorTree(Int(v / factor)))
end


function getFactors(t::FactorTree)::Dict{Int, Int}
    dict = Dict{Int, Int}()

    if t.left == 0 && t.right == 0
        return Dict{Int, Int}(t.value => 1)
    end

    dict = mergewith(+, dict, getFactors(t.left))
    dict = mergewith(+, dict, getFactors(t.right))
    
    return dict
end


function getShape(t::FactorTree)::String
    if isa(t, Int)
        return "p"
    else
        left_shape = (t.left isa FactorTree && (t.left.left isa FactorTree || t.left.right isa FactorTree)) ? "p2" : "p"
        right_shape = (t.right isa FactorTree && (t.right.left isa FactorTree || t.right.right isa FactorTree)) ? "p2" : "p"
        return "f($left_shape|$right_shape)"
    end
end


function compareShape(t::FactorTree, h::FactorTree)::Bool
    return getShape(t) == getShape(h)
end



function computeShapes(n::Int)::Dict{String, Vector{Int}}
    res = Dict("p2" => Vector{Int}(), "f(p|p2)" => Vector{Int}(), "p" => Vector{Int}())

    while n > 0
        current_tree = FactorTree(n)

        if isa(current_tree.left, Int) && isa(current_tree.right, Int)
            push!(res["p"], n)
        elseif isa(current_tree.left.left, Int) ⊻ isa(current_tree.right.left, Int)
            push!(res["f(p|p2)"], n)
        else
            push!(res["p2"], n)
        end
        n -= 1
    end
    return res
end