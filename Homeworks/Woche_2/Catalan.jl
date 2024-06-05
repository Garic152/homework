function Catalan2(n)
    if n <= 1
        return 1
    end

    catalan_numbers = zeros(Int, n + 1)

    catalan_numbers[1] = 1
    catalan_numbers[2] = 1

    for i in 3:n+1
        for j in 1:i-1
            catalan_numbers[i] += catalan_numbers[j] * catalan_numbers[i-j]
        end
    end

    return catalan_numbers[n+1]
end


function CatalanTriangle(i)
    triangle = zeros(Int, (i, i))

    for n in 1:i
        for k in 1:n
            if n >= 1 && k == 1
                triangle[n, k] = 1
            elseif n >= 2 && k == 2
                triangle[n, 2] = n - 1
            elseif 2 < k < n
                triangle[n, k] = triangle[n, k-1] + triangle[n-1, k]
            elseif n >= 3
                triangle[n, n] = triangle[n, n-1]
            end
            print(triangle[n, k], " ")
        end
        println("")
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

CatalanTriangle(9)
