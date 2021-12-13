#ifndef MINHEAP_H
#define MINHEAP_H

#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

template<typename T>
class MinHeap {
public:
    MinHeap(int d);
    /* Constructor that builds a d-ary Min Heap
       This should work for any d >= 2,
       but doesn't have to do anything for smaller d.*/

    ~MinHeap();

    void add(T item, float priority);
    /* adds the item to the heap, with the given priority. */

    const T& peek() const;
    /* returns the element with smallest priority.
       Break ties however you wish.
       Throws an exception if the heap is empty. */

    const float& peekVal() const;
    /* returns the value with smallest priority.
       Break ties however you wish.
       Throws an exception if the heap is empty. */

    void remove();
    /* removes the element with smallest priority.
       Break ties however you wish.
       Throws an exception if the heap is empty. */

    bool isEmpty();
    /* returns true iff there are no elements on the heap. */

private:
    // whatever you need to naturally store things.
    // You may also add helper functions here.

    int dairy, size;  // Variable that determines how many children each node has
    // as well as a size counter to keep track of the numbers in the heap

    std::vector<std::pair<T, float>> heap;  // vector of pairs type T item and a priority
    void bubbleUp(int index);               // Helper function to swap nodes when adding into the heap
    void trickleDown(int index);            // Helper function to swap nodes when removing from the heap
};

// Constructor
template<typename T>
MinHeap<T>::MinHeap(int d) : dairy(d) {
    // Initialize size as 0 since it is an empty heap
    size = 0;
}

// Destructor
template<typename T>
MinHeap<T>::~MinHeap() {}

// Add an item with a user given priority
template<typename T>
void MinHeap<T>::add(T item, float priority) {
    heap.push_back(std::make_pair(item, priority));  // First push the pair of item and priority into the vector
    bubbleUp(size);                                  // Call the bubbleUp function
    ++size;                                          // Increment counter
    return;
}

// Function to bubble Up when adding a new pair
template<typename T>
void MinHeap<T>::bubbleUp(int index) {
    if (index <= 0) {
        return;
    }
    int parentIndex = (index - 1) / dairy;  // Parent index

    // If parent node is larger, swap
    if (heap[parentIndex].second > heap[index].second) {
        std::swap(heap[parentIndex], heap[index]);
    }
        // If parent node priority is the same as the child's priority
    else if (heap[parentIndex].second == heap[index].second) {
        // Swap iff the item of the parent is greater than that of the child
        if (heap[parentIndex].first > heap[index].first) {
            std::swap(heap[parentIndex], heap[index]);
        }
    }

    // Recursively call bubbleUp until we hit the 0th index
    if (parentIndex > 0) {
        bubbleUp(parentIndex);
    }
    return;
}

// Function to return the item with the smallest priority
template<typename T>
const T& MinHeap<T>::peek() const {
    // If error is thrown, function does not return anything
    if (size == 0) {
        throw std::out_of_range("Empty heap!");
    }
    else{
        return heap[0].first;
    }
}

template<typename T>
const float& MinHeap<T>::peekVal() const {
    // If error is thrown, function does not return anything
    if (size == 0) {
        throw std::out_of_range("Empty heap!");
    }
    else{
        return heap[0].second;
    }
}

// Function to remove the node with the smallest priority
template<typename T>
void MinHeap<T>::remove() {
    // Throw exception if the heap is empty
    try {
        if (size == 0) {
            throw std::out_of_range("Vector<X>::at() : "
                                    "index is out of range(Heap underflow)");
        }

        heap[0] = heap.back();
        heap.pop_back();  // Universal call to pop last item in the heap
        size -= 1;        // Decrement the size by 1
        trickleDown(0);   // Call to swap the value down if there is a smaller priority
        return;
    } catch (const std::out_of_range& oor) {
        std::cout << oor.what() << std::endl;
    }
}  // Fin remove()

// Function to trickle down an item when placed at the top
template<typename T>
void MinHeap<T>::trickleDown(int index) {
    int child = index * dairy + 1;  // The index to the child of the given index pair
    int minIndex = index;

    // Loop through every child
    for (int i = 0; i < dairy; ++i) {
        // If the child index is in the heap
        if (child + i < size) {
            // Compare the two priorities
            if (heap[child + i].second < heap[minIndex].second) {
                // Update the index storing the lowest priority
                minIndex = child + i;
            }
                // If the priorities are equal
            else if (heap[child + i].second == heap[minIndex].second) {
                // Compare the two Items
                if (heap[child + i].first < heap[minIndex].first) {
                    // Update the min as the item value that is less than the two
                    minIndex = child + i;
                }
            }
        } else {
            break;
        }
    }

    // If we had to update the index of the lowest priority
    if (minIndex != index) {
        std::swap(heap[index], heap[minIndex]);  // Swap
        trickleDown(minIndex);                   // Call trickleDown recursively
    }

    return;
}  // Fin TrickleDown

// Returns if the heap is empty
template<typename T>
bool MinHeap<T>::isEmpty() {
    if (size <= 0) {
        return true;
    } else {
        return false;
    }
}
#endif
