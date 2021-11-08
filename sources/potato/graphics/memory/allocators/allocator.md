Each allocator manages a list of pools
It's nice to segrate the pools based on memory types, that way
it helps to quickly iterate over all the pools of that type and 
find a free spot. 
