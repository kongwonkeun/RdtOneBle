/*
 * XLinkedList.h
 *
 * Created: 2019-03-27 13:02:06
 *  Author: kong
 */

#ifndef XLINKEDLIST_H_
#define XLINKEDLIST_H_

#include <stdlib.h>

template<class T> struct XLinkedListNode
{
    T data;
    XLinkedListNode<T>* next;
};

template <typename T> class XLinkedList
{
public:
    XLinkedList();
    ~XLinkedList();
    void add(T);
    T get(unsigned int index);
    void clear();
    unsigned int size();
private:
    unsigned int m_size;
    XLinkedListNode<T>* m_root;
    XLinkedListNode<T>* m_last;
};

template <typename T> XLinkedList<T>::XLinkedList() : m_size(0), m_root(NULL), m_last(NULL)
{
    //
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

#endif /* XLINKEDLIST_H_ */