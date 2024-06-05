function Catalan(n)
    if n <= 1
        return 1
    end

    result = 0
    for i in 0:n-1
        result += catalan(i) * catalan(n - i - 1)
    end

    return result
end


function Catalan2(n)
    if n <= 1
        return 1
    end

    catalan_numbers = zeros(Int, n+1)

    catalan_numbers[1] = 1
    catalan_numbers[2] = 1

    for i in 3:n+1 # Laufzeit vom ersten for-Loop: n
        for j in 1:i-1 # Laufzeit vom zweiten for-Loop: n
            catalan_numbers[i] += catalan_numbers[j] * catalan_numbers[i-j]
        end
    end

    return catalan_numbers[n+1]

    # Gesamtlaufzeit O(Catalan2(n)) = O(for-Loop 1 * for-Loop 2) = n^2
end


function CatalanTriangle(n)
    triangle = zeros(Int, (n+1, n+1))

    for i in 1:n+1
        for j in 1:i
            if i >= 1 && j == 1
                triangle[i, j] = 1
            elseif j == 2
                triangle[i, 2] = i
            elseif 2 < j < i
                triangle[i, j] = triangle[i - 1, j - 1] + triangle[i - 1, j]
            else
                triangle[i, i] = triangle[i, i - 1]
            end
        end
    end

    return triangle
end

#function catalan_remove(Y)
#    n, m = size(Y)
#    i, j = 1, 1
#
#    Y[i, j] = 9999
#
#    while true
#        i, j = i%3, j%3
#
#        if i < n
#            Y[i, j] = Y[i + 1, j]
#            i + 1
#
#        elseif j < m
#            Y[i, j] = Y[i, j + 1]
#            j + 1
#        else
#            break
#        end 
#    end
#    return Y
#end

for i in 0:10
    println(Catalan(i), " ", Catalan2(i))
end

println(CatalanTriangle(5))