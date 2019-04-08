/*
 * LinkedList.h
 *
 * Created: 2019-04-08 15:47:42
 *  Author: kong
 */ 

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdlib.h>

template<class T> struct LinkedListNode
{
    T data;
    LinkedListNode<T>* next;
};

template <typename T> class LinkedList
{
    public:
    LinkedList();
    ~LinkedList();
    void add(T);
    T get(unsigned int index);
    void clear();
    unsigned int size();
    private:
    unsigned int m_size;
    LinkedListNode<T>* m_root;
    LinkedListNode<T>* m_last;
};

template <typename T> LinkedList<T>::LinkedList() : m_size(0), m_root(NULL), m_last(NULL)
{
}

template <typename T> LinkedList<T>::~LinkedList()
{
    clear();
}

template <typename T> void LinkedList<T>::add(T item)
{
    LinkedListNode<T>* itemNode = new LinkedListNode<T>();
    itemNode->data = item;
    itemNode->next = NULL;
    if (m_root == NULL) {
        m_root = itemNode;
        } else {
        m_last->next = itemNode;
    }
    m_last = itemNode;
    m_size++;
}

template <typename T> T LinkedList<T>::get(unsigned int index)
{
    if (index >= m_size) {
        return T();
    }
    LinkedListNode<T>* itemNode = m_root;
    for (unsigned int i = 0; i < index; i++) {
        itemNode = itemNode->next;
    }
    return itemNode->data;
}

template <typename T> void LinkedList<T>::clear()
{
    LinkedListNode<T>* itemNode = m_root;
    for (unsigned int i = 0; i < m_size; i++) {
        LinkedListNode<T>* n = itemNode;
        itemNode = itemNode->next;
        delete n;
    }
    m_size = 0;
    m_root = NULL;
    m_last = NULL;
}

template <typename T> unsigned int LinkedList<T>::size()
{
    return m_size;
}



#endif /* LINKEDLIST_H_ */