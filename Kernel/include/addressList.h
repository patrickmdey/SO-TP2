#ifndef ADDRESS_LIST_H
#define ADDRESS_LIST_H

typedef struct t_addressNode{
    void * address;
    struct t_addressNode * next;
} t_addressNode;

typedef struct t_addressList {
    t_addressNode * first;
    int size;
} t_addressList;


int addAddress(t_addressList * l, void * address);

int removeAddress(t_addressList* l, void * address);

int getAddressesSize(t_addressList* l);

void freeAddressList(t_addressList* l);


#endif