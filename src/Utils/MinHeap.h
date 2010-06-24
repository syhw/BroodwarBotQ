#pragma once
#include <vector>
#include <map>
#include <utility>

// Hacked from BWTA's Heap implementation
// Mininmum value on top
template<class _Tp, class _Val>
class MinHeap 
{
    private:
        /**
         * If data[i] is a node, 
         * data[2*i+1] is its left child
         * data[2*i+2] is its right child
         * data[(i-1)/2] is its parent
         */
        std::vector< std::pair<_Tp, _Val> > data;
        std::map< _Tp, int> obj_to_pos; // maps to position in data

        /**
         * Percolate the given element in the heap up
         * @param the index of the element to percolate up
         * @return the final index of the element
         */
        int percolate_up(int index);

        /**
         * Percolate the given element in the heap down
         * @param the index of the element to percolate down
         * @return the final index of the element
         */
        int percolate_down(int index);

    public:
        MinHeap() {}
        MinHeap( std::pair< _Tp, _Val > x);
        ~MinHeap() {}

        /**
         * Pushes an (object, value) onto the heap
         * @param the object-value pair to push onto the heap
         */
        void push(std::pair< _Tp, _Val > x);

        void pop();

        const std::pair< _Tp, _Val >& top() const;
        
        /**
         * Update the given object with the given value and moves the object
         * Pushes the object if it doesn't exist
         * @param an object that exists in the heap
         * @param the new value for that object
         * @return boolean success
         */
        bool set( _Tp& x, _Val& v);

        /** 
         * Return the given object's value
         * @param object
         * @return value
         */
        // const _Val& get(_Tp& x) const;

        // bool contains(_Tp& x) const;

        bool empty() const;

        int size() const;

        // void clear();

        // bool erase(_Tp& x);

};

template <class _Tp, class _Val> 
MinHeap<_Tp, _Val>::MinHeap( std::pair< _Tp, _Val > x)
{
    data.push_back(x);
    obj_to_pos.insert(std::make_pair(x.first, 0));
}

template <class _Tp, class _Val>
bool MinHeap<_Tp, _Val>::set(_Tp& x, _Val& v)
{
    std::map<_Tp, int>::iterator it = obj_to_pos.find(x);
    if (it == obj_to_pos.end())
    {
        push(std::make_pair(x, v));
        return true;
    }
    int index = it->second;
    data[index].second = v;
    index = percolate_up(index);
    if (index >= 0 && index < (int)data.size())
    {
        percolate_down(index); // if only we knew where to go (up/down)
        // normally for A*, we only go up when we set, and "up" when we push
        return true;
    }
    return false;
}

template <class _Tp, class _Val>
void MinHeap<_Tp, _Val>::push(std::pair< _Tp, _Val > x)
{
    int index = data.size();
    if (obj_to_pos.insert(std::make_pair(x.first, index)).second)
    {
        data.push_back(x);
        percolate_up(index);
    }
}

template <class _Tp, class _Val>
int MinHeap<_Tp, _Val>::percolate_up(int index)
{
    if (index < 0 || index >= (int)data.size())
        return -1;
    unsigned int parent = (index - 1) / 2;
    while (index > 0 && data[parent].second > data[index].second)
    {
        std::pair<_Tp, _Val> temp = data[parent];
        data[parent] = data[index];
        data[index] = temp;
        obj_to_pos.find(data[index].first)->second = index;
        index = parent;
        parent = (index - 1) / 2;
    }
    return index;
}

template <class _Tp, class _Val>
int MinHeap<_Tp, _Val>::percolate_down(int index)
{
    if (index < 0 || index >= (int)data.size())
        return -1;
    unsigned int lchild = index*2 + 1;
    unsigned int rchild = index*2 + 2;
    while ((data.size()>lchild && data[index].second>data[lchild].second) || 
            (data.size()>rchild && data[index].second>data[rchild].second))
    {
        unsigned int dest = lchild;
        if (data.size()>rchild && data[rchild].second<data[lchild].second)
            dest = rchild;
        std::pair< _Tp, _Val > temp = data[dest];
        data[dest] = data[index];
        data[index] = temp;
        obj_to_pos.find(data[index].first)->second = index;
        index = dest;
        lchild = index*2 + 1;
        rchild = index*2 + 2;
    }
    obj_to_pos.find(data[index].first)->second = index;
    return index;
}

template <class _Tp, class _Val>
bool MinHeap<_Tp, _Val>::empty() const
{
    return data.empty();
}

template <class _Tp, class _Val>
const std::pair<_Tp, _Val>& MinHeap<_Tp, _Val>::top() const
{
    return data.front();
}

template <class _Tp, class _Val>
int MinHeap<_Tp, _Val>::size() const
{
    return data.size();
}

template <class _Tp, class _Val>
void MinHeap<_Tp, _Val>::pop()
{
    if (data.empty())
        return;
    obj_to_pos.erase(data.front().first);
    data.front() = data.back();
    data.pop_back();
    if (data.empty())
        return;
    std::map<_Tp, int>::iterator it = obj_to_pos.find(data.front().first);
    if (it != obj_to_pos.end())
    {
        it->second = 0;
        percolate_down(0);
    }
}
