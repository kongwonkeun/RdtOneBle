/*
 * XLinkedList.h
 *
 * Created: 2019-03-27 13:02:06
 *  Author: kong
 */

#ifndef XLINKEDLIST_H_
#define XLINKEDLIST_H_

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

#endif /* XLINKEDLIST_H_ */