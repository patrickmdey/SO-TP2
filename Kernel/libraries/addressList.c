#include <addressList.h>
#include <stddef.h>
#include <memoryManager.h>

static void freeRec(t_addressNode* addressNode);
static t_addressNode * deleteAddress(t_addressNode* node, void* address, int* flag);

int addAddress(t_addressList * l, void * address) {
    if (l == NULL)
        return 0;

    t_addressNode * addNode = malloc(sizeof(t_addressNode));
    if (addNode == NULL)
        return 0;

    addNode->address = address;
    addNode->next = NULL;

    if (l->first == NULL) {
        l->first = addNode;
        return 1;
    }

    t_addressNode* current = l->first;
    while (current->next != NULL) {
        current = current->next;
    }

    l->size++;
    current->next = addNode;
    return 1;
}

int removeAddress(t_addressList * l, void * address) {
    if (l == NULL)
        return 0;

    int flag = 0;
    l->first = deleteAddress(l->first, address, &flag);
    if (flag)
        l->size--;

    return flag;
}

int getAddressesSize(t_addressList* l) {
    if (l == NULL)
        return -1;
    return l->size;
}

void freeAddressList(t_addressList* l) {
    if (l == NULL)
        return;

    freeRec(l->first);
    free((void *) l);
}

static t_addressNode* deleteAddress(t_addressNode * node, void * address, int* flag) {
    if (node == NULL) {
        return NULL;
    }

    if (node->address == address) {
        t_addressNode* next = node->next;
        free(node);
        *flag = 1;
        return next;
    }

    node->next = deleteAddress(node->next, address, flag);
    return node;
}

static void freeRec(t_addressNode * addressNode) {
    if (addressNode == NULL)
        return;

    t_addressNode * next = addressNode->next;

    free(addressNode->address);
    free(addressNode);
    freeRec(next);
}