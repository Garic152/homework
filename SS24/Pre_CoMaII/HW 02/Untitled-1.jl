using DataStructures

struct NewStack
    queue::Queue
end


function stack_insert(stack::NewStack, in)
    enqueue!(stack.queue, in)
end
 

function stack_pop(in::NewStack)
    out = NewStack(Queue{Int}())

    global i = length(in.queue) - 1

    while i > 0
        enqueue!(out.queue, dequeue!(in.queue))
        i -= 1
    end

    return out
end


function stack_is_emty(in::NewStack)
    return true ? isempty(in.queue) : false
end


# Create a new queue of integers
new = NewStack(Queue{Int}())


for i in 1:15
    stack_insert(new, i)
end


new = stack_pop(new)

println(stack_is_emty(new))

println(new)