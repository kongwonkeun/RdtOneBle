/*
 * XLinkedList.cpp
 *
 * Created: 2019-03-30 12:25:48
 *  Author: kong
 */ 


#include "../system/XLinkedList.h"

template <typename T> XLinkedList<T>::XLinkedList() : m_size(0), m_root(NULL), m_last(NULL)
{
    // nothing to do
}

template <typename T> XLinkedList<T>::~XLinkedList()
{
    clear();
}

template <typename T> void XLinkedList<T>::add(T item)
{
    XLinkedListNode<T>* itemNode = new XLinkedListNode<T>();
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

template <typename T> T XLinkedList<T>::get(unsigned int index)
{
    if (index >= m_size) {
        return T();
    }
    XLinkedListNode<T>* itemNode = m_root;
    for (unsigned int i = 0; i < index; i++) {
        itemNode = itemNode->next;
    }
    return itemNode->data;
}

template <typename T> void XLinkedList<T>::clear()
{
    XLinkedListNode<T>* itemNode = m_root;
    for (unsigned int i = 0; i < m_size; i++) {
        XLinkedListNode<T>* n = itemNode;
        itemNode = itemNode->next;
        delete n;
    }
    m_size = 0;
    m_root = NULL;
    m_last = NULL;
}

template <typename T> unsigned int XLinkedList<T>::size()
{
    return m_size;
}


