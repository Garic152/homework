using DataStructures: Stack

mutable struct Queue
    stack1::Stack
    stack2::Stack
end


function invert_stacks(queue::Queue)::Queue
    while length(queue.stack1) != 0
        push!(queue.stack2, pop!(queue.stack1))
    end
    
    queue.stack1 = deepcopy(queue.stack2)
    empty!(queue.stack2)

    return queue
end

function queue_insert(queue::Queue, n::Int)::Queue
    invert_stacks(queue)
    push!(queue.stack1, n)
    invert_stacks(queue)
    return queue
end


function queue_pop(queue::Queue)::Queue
    invert_stacks(queue)
    pop!(queue.stack1)
    invert_stacks(queue)
    return queue
end


function queue_isEmpty(queue::Queue)::Bool
    return isempty(queue.stack1)
end


# test = Queue(Stack{Int}(), Stack{Int}())
# 
# for i in 1:15
#     queue_insert(test, i)
# end
# 
# println(test)
# 
# queue_insert(test, 1)
# println(test)
# 
# queue_pop(test)
# println(test)
# 
# for i in 1:15
#     queue_pop(test)
# end
# 
# print(queue_isEmpty(test))