#include <memoryManager.h>
#include <stringLib.h>
#include <limits.h>
#include <utils.h>
#include <taskManager.h>

#define TOTAL_MEM 1024 * 1000
#define BLOCK_SIZE 32
#define TOTAL_BLOCKS (TOTAL_MEM/BLOCK_SIZE)
uint8_t* start = (uint8_t*)0x600000;


#ifndef BUDDY
static int findBlocks(int amount);
static void asignMemory(int firstIdx, int count, int size);

int nextindex = 0;

typedef struct block_t {
    uint16_t size;  // = a lo que pidio size = 64 
    uint8_t free;
    int pos;    //pos q tengo en el array
    uint8_t usesNext;
} block_t;

block_t blockArray[TOTAL_BLOCKS];

void initMemory() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        blockArray[i].free = 1;
        blockArray[i].pos = i;
    }
}

void* malloc(uint32_t size) {
    if (size == 0)
        return NULL;

    int amount = (size / BLOCK_SIZE);
    if (size % BLOCK_SIZE != 0)
        amount++;

    int firstIdx = findBlocks(amount);
    if (firstIdx == -1) {
        printStringWC("NO HAY MEMORIA", BLACK, RED);
        printStringLn("");
        return NULL;
    }

    asignMemory(firstIdx, amount, size);

    t_PCB* currentProcess = getCurrentProcess();
    currentProcess->dirArray[currentProcess->dirArrayIndex++] = (uint8_t*) &start[firstIdx * BLOCK_SIZE];
    // printString("MALLOC -> ");
    // printInt(currentProcess->dirArrayIndex);
    // printStringLn("");

    return &start[firstIdx * BLOCK_SIZE];
}

void free(void* dir) {
    if ((uint64_t)dir % BLOCK_SIZE) {
        //printStringLn("DIR NO VALIDA");
        return;
    }
    int idx = ((uint8_t*)dir - start) / BLOCK_SIZE;
    if (idx < 0 || idx >= TOTAL_BLOCKS) {
        return;
    }

    if(blockArray[idx].free == 1)
        return;
    
    while (blockArray[idx].usesNext == 1) {
        blockArray[idx].free = 1;
        blockArray[idx].size = 0;
        blockArray[idx].usesNext = 0;
        idx++;
    }
    blockArray[idx].free = 1;
    blockArray[idx].size = 0;
}

char** getMemoryInfo(uint64_t* size) {
    int freeMemory = 0;
    int usedMemory = 0;
    int blocksUsed = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            freeMemory++;
        }
        else {
            blocksUsed++;
            usedMemory += blockArray[i].size;
        }
    }
    *size = 3;
    char** info = malloc((*size) * sizeof(char*));
    int i;
    for (i = 0; i < *size; i++) {
        info[i] = malloc(100);
    }

    int offset = 0;
    offset += strcpy(info[0], "Memoria total: ");
    offset += uintToBase(TOTAL_MEM, info[0] + offset, 10);
    offset += strcpy(info[0] + offset, " bytes");
    info[0][offset] = 0;
    offset = 0;
    offset += strcpy(info[1], "Memoria en uso: ");
    offset += uintToBase(usedMemory, info[1] + offset, 10);
    offset += strcpy(info[1] + offset, " bytes");
    info[1][offset] = 0;
    offset = 0;
    offset += strcpy(info[2], "Bloques en uso: ");
    offset += uintToBase(blocksUsed, info[2] + offset, 10);
    info[2][offset] = 0;
    return info;
}

static void asignMemory(int firstIdx, int count, int size) {
    for (int i = firstIdx; i < count + firstIdx; i++) {
        blockArray[i].size = BLOCK_SIZE;
        blockArray[i].usesNext = 1;
        blockArray[i].free = 0;
        if (i == count + firstIdx - 1) {
            if (size % BLOCK_SIZE != 0)
                blockArray[i].size = size % BLOCK_SIZE;

            blockArray[i].usesNext = 0;
        }
    }
}
static int findBlocks(int amount) {
    int i, count = 0, firstIdx;
    for (i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            if (count == 0) {
                firstIdx = i;
            }
            count++;
            if (count == amount) {
                return firstIdx;
            }
        }
        else {
            count = 0;
        }
    }
    return -1;
}
/*
// Otro sistema --> menor cantidad de bloques por memoria pedida
static int findBlocks(int amount) {
    //printStringLn("BUDDY!");
    int i, count = 0, firstIdx, best_fit = INT_MAX, best_fit_idx = -1;
    for (i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            if (count == 0) {
                firstIdx = i;
            }
            count++;
        }
        else {
            if (count >= amount && count < best_fit) {
                best_fit_idx = firstIdx;
                best_fit = count;
            }
            count = 0;
        }
    }

    if (count >= amount && count < best_fit) {
        return firstIdx;
    }

    return best_fit_idx;
}
*/

#else
/*
#define BASE 2
enum powersOfTwo{K = 10, M = 20, G = 30};
#define MIN_ALLOC_LOG2 6 //Min size supported = 64 bytes
#define MAX_ALLOC_LOG2 (M - MIN_ALLOC_LOG2) //Max size supported = 1 GB
#define BINARY_POWER(x) (1 << (x))

typedef struct blockNode_t
{
    unsigned short int order;
    unsigned short int inUse;
    struct blockNode_t *prev;
    struct blockNode_t *next;
} blockNode_t;

static void pushNode(blockNode_t *oldNode, blockNode_t *newNode);
static void deleteNode(blockNode_t *node);
static blockNode_t *popNode(blockNode_t *node);
static int isNodeEmpty(blockNode_t *node);


static unsigned int getBlock(unsigned int request);
static int getBlockToUse(unsigned int firstBlock);
static blockNode_t *findBuddy(blockNode_t *node);
static blockNode_t *getPrincipalAdress(blockNode_t *node);
static void addNodeAndMerge(blockNode_t *node);
static void addNewNode(blockNode_t *node, blockNode_t *lastNode, unsigned int order);

static blockNode_t *heapPtr;
static unsigned int heapSize;
static unsigned int cantBlocks;
static blockNode_t blocksArr[MAX_ALLOC_LOG2];
static unsigned int availableMemory;
static int usedBlocks = 0;

char ** getMemoryInfo(uint64_t * size) {
    *size = 3;
    char ** info = malloc(*size * sizeof(char *));
    int i;
    for(i = 0; i < *size; i++) {
        info[i] = malloc(100);
    }
    int offset = 0;
    offset += strcpy(info[0], "Memoria total: ");
    offset += uintToBase(heapSize, info[0] + offset, 10);
    offset += strcpy(info[0] + offset, " bytes");
    info[0][offset] = 0;
    offset = 0;
    offset += strcpy(info[1], "Memoria en uso: ");
    offset += uintToBase(heapSize - availableMemory, info[1] + offset, 10);
    offset += strcpy(info[1] + offset, " bytes");
    info[1][offset] = 0;
    offset = 0;
    offset += strcpy(info[2], "Bloques en uso: ");
    offset += uintToBase(usedBlocks, info[2] + offset, 10);
    info[2][offset] = 0;

    return info;
}

void initMemory() {

    heapPtr = (blockNode_t *) start;
    heapSize = TOTAL_MEM;
    availableMemory = TOTAL_MEM;
    cantBlocks = log2(heapSize) - MIN_ALLOC_LOG2 + 1;

    if (cantBlocks > MAX_ALLOC_LOG2)
        cantBlocks = MAX_ALLOC_LOG2;

    blockNode_t * aux= blocksArr;

    //inicializo los ordenes con sus bloques
    for (int i = 0; i < cantBlocks; i++,aux++) {
        blocksArr[i].order = i;
        blocksArr[i].inUse = 1;
        blocksArr[i].prev = blocksArr[i].next = aux;
    }
    //agrego el de mayor tamaño al heap
    addNewNode(heapPtr, &blocksArr[cantBlocks - 1], cantBlocks - 1);
}

void *malloc(uint32_t size) {
    if (size == 0) {
        return NULL;
    }
    unsigned int totalBytes = size + sizeof(blockNode_t);

    if (totalBytes > heapSize) {
        return NULL;
    }

    unsigned int block = getBlock(totalBytes);
    //obtengo el boque para el tamaño adecuado
    int parentBlock = getBlockToUse(block);
    if (parentBlock == -1) {
        return NULL;
    }

    blockNode_t *dir= popNode(&blocksArr[parentBlock]);
    //divido bloques
    while(block < parentBlock){
        dir->order--;
        addNewNode(findBuddy(dir), &blocksArr[parentBlock - 1], parentBlock - 1);
        parentBlock--;
    }
    dir->inUse = 1;
    dir++;

    availableMemory -= BINARY_POWER(block + MIN_ALLOC_LOG2);
    usedBlocks++;
    return (void *)dir;
}

void free(void *dir) {

    if (dir == NULL)
        return;
    blockNode_t *blockNode = (blockNode_t *)dir - 1;
    blockNode->inUse = 0;
    availableMemory += BINARY_POWER(blockNode->order + MIN_ALLOC_LOG2);
    //como devuelvo el bloque, lo mergeo con su buddy
    addNodeAndMerge(blockNode);
    usedBlocks--;
    return;
}


static void pushNode(blockNode_t *oldNode, blockNode_t *newNode) {
    blockNode_t *prev = oldNode->prev;
    newNode->prev = prev;
    newNode->next = oldNode;
    prev->next = newNode;
    oldNode->prev = newNode;
}

static void deleteNode(blockNode_t *node) {
    blockNode_t *prev = node->prev;
    blockNode_t *next = node->next;

    prev->next = next;
    next->prev = prev;
}

static blockNode_t *popNode(blockNode_t *node) {
    blockNode_t *aux = node->prev;

    if (aux == node)
        return NULL;

    deleteNode(aux);

    return aux;
}

static int isNodeEmpty(blockNode_t *node) {
    return node->prev == node;
}

static void addNewNode(blockNode_t *node, blockNode_t *lastNode, unsigned int order) {
    node->inUse = 0;
    node->order = order;
    pushNode(lastNode, node);
}

//para utilizar en el free y mergear bloque con su buddy
static void addNodeAndMerge(blockNode_t *node) {

    blockNode_t *buddy = findBuddy(node);
    while (node->order != cantBlocks - 1 && buddy->inUse == 0 && buddy->order == node->order) {

        deleteNode(buddy);

        node = getPrincipalAdress(node);
        node->order++;

        buddy = findBuddy(node);
    }
    pushNode(&blocksArr[node->order], node);
}

static blockNode_t *findBuddy(blockNode_t *node) {

    uint64_t currOffset = (uint64_t)node - (uint64_t)heapPtr;
    uint64_t newOffset = currOffset ^ BINARY_POWER(MIN_ALLOC_LOG2 + node->order);
    return (blockNode_t *)((uint64_t)heapPtr + newOffset);
}

static blockNode_t *getPrincipalAdress(blockNode_t *node) {

    uint64_t msk = BINARY_POWER(node->order + MIN_ALLOC_LOG2);
    msk = ~msk;

    uint64_t currentOffset = (uint64_t)node - (uint64_t)heapPtr;
    uint64_t newOffset = currentOffset & msk;
    return (blockNode_t *)((uint64_t)heapPtr + newOffset);
}

static unsigned int getBlock(unsigned int req) {

    unsigned int aux = log2(req);
    if (aux < MIN_ALLOC_LOG2)
        return 0;
    aux -= MIN_ALLOC_LOG2;
    return isPower(BASE, req) == 0 ? aux + 1 : aux;
}

static int getBlockToUse(unsigned int firstBlock) {

    int currentBlock = firstBlock;
    while (currentBlock < cantBlocks && isNodeEmpty(&blocksArr[currentBlock]))
        currentBlock++;
    if (currentBlock == cantBlocks)
        return -1;
    return currentBlock;
}
*/
#define HEADER_SIZE 16

/*
 * The minimum allocation size is 16 bytes because we have an 8-byte header and
 * we need to stay 8-byte aligned.
 */
#define MIN_ALLOC_LOG2 4
#define MIN_ALLOC ((size_t) 1 << MIN_ALLOC_LOG2)

 /*
  * The maximum allocation size is currently set to 2gb. This is the total size
  * of the heap. It's technically also the maximum allocation size because the
  * heap could consist of a single allocation of this size. But of course real
  * heaps will have multiple allocations, so the real maximum allocation limit
  * is at most 1gb.
  */
#define MAX_ALLOC_LOG2 19
#define MAX_ALLOC ((size_t) 1 << MAX_ALLOC_LOG2)

  /*
   * Allocations are done in powers of two starting from MIN_ALLOC and ending at
   * MAX_ALLOC inclusive. Each allocation size has a bucket that stores the free
   * list for that allocation size.
   *
   * Given a bucket index, the size of the allocations in that bucket can be
   * found with "(size_t)1 << (MAX_ALLOC_LOG2 - bucket)".
   */
#define BUCKET_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1)

   /*
    * Free lists are stored as circular doubly-linked lists. Every possible
    * allocation size has an associated free list that is threaded through all
    * currently free blocks of that size. That means MIN_ALLOC must be at least
    * "sizeof(list_t)". MIN_ALLOC is currently 16 bytes, so this will be true for
    * both 32-bit and 64-bit.
    */
typedef struct list_t {
    struct list_t* prev, * next;
} list_t;

/*
 * Each bucket corresponds to a certain allocation size and stores a free list
 * for that size. The bucket at index 0 corresponds to an allocation size of
 * MAX_ALLOC (i.e. the whole address space).
 */
static list_t buckets[BUCKET_COUNT];

/*
 * We could initialize the allocator by giving it one free block the size of
 * the entire address space. However, this would cause us to instantly reserve
 * half of the entire address space on the first allocation, since the first
 * split would store a free list entry at the start of the right child of the
 * root. Instead, we have the tree start out small and grow the size of the
 * tree as we use more memory. The size of the tree is tracked by this value.
 */
static size_t bucket_limit;

/*
 * This array represents a linearized binary tree of bits. Every possible
 * allocation larger than MIN_ALLOC has a node in this tree (and therefore a
 * bit in this array).
 *
 * Given the index for a node, lineraized binary trees allow you to traverse to
 * the parent node or the child nodes just by doing simple arithmetic on the
 * index:
 *
 * - Move to parent:         index = (index - 1) / 2;
 * - Move to left child:     index = index * 2 + 1;
 * - Move to right child:    index = index * 2 + 2;
 * - Move to sibling:        index = ((index - 1) ^ 1) + 1;
 *
 * Each node in this tree can be in one of several states:
 *
 * - UNUSED (both children are UNUSED)
 * - SPLIT (one child is UNUSED and the other child isn't)
 * - USED (neither children are UNUSED)
 *
 * These states take two bits to store. However, it turns out we have enough
 * information to distinguish between UNUSED and USED from context, so we only
 * need to store SPLIT or not, which only takes a single bit.
 *
 * Note that we don't need to store any nodes for allocations of size MIN_ALLOC
 * since we only ever care about parent nodes.
 */
static uint8_t node_is_split[(1 << (BUCKET_COUNT - 1)) / 8];

/*
 * This is the starting address of the address range for this allocator. Every
 * returned allocation will be an offset of this pointer from 0 to MAX_ALLOC.
 */
static uint8_t* base_ptr;

/*
 * This is the maximum address that has ever been used by the allocator. It's
 * used to know when to call "brk" to request more memory from the kernel.
 */
static uint8_t* max_ptr;

/*
 * Make sure all addresses before "new_value" are valid and can be used. Memory
 * is allocated in a 2gb address range but that memory is not reserved up
 * front. It's only reserved when it's needed by calling this function. This
 * will return false if the memory could not be reserved.
 */
static int update_max_ptr(uint8_t* new_value) {
    if (new_value > max_ptr) {
        if (new_value > base_ptr + TOTAL_MEM) {
            printStringWC("NO HAY MEMORIA", BLACK, RED);
            return 0;
        }
        max_ptr = new_value;
    }
    return 1;
}

/*
 * Initialize a list to empty. Because these are circular lists, an "empty"
 * list is an entry where both links point to itself. This makes insertion
 * and removal simpler because they don't need any branches.
 */
static void list_init(list_t* list) {
    list->prev = list;
    list->next = list;
}

/*
 * Append the provided entry to the end of the list. This assumes the entry
 * isn't in a list already because it overwrites the linked list pointers.
 */
static void list_push(list_t* list, list_t* entry) {
    list_t* prev = list->prev;
    entry->prev = prev;
    entry->next = list;
    prev->next = entry;
    list->prev = entry;
}

/*
 * Remove the provided entry from whichever list it's currently in. This
 * assumes that the entry is in a list. You don't need to provide the list
 * because the lists are circular, so the list's pointers will automatically
 * be updated if the first or last entries are removed.
 */
static void list_remove(list_t* entry) {
    list_t* prev = entry->prev;
    list_t* next = entry->next;
    prev->next = next;
    next->prev = prev;
}

/*
 * Remove and return the first entry in the list or NULL if the list is empty.
 */
static list_t* list_pop(list_t* list) {
    list_t* back = list->prev;
    if (back == list) return NULL;
    list_remove(back);
    return back;
}

/*
 * This maps from the index of a node to the address of memory that node
 * represents. The bucket can be derived from the index using a loop but is
 * required to be provided here since having them means we can avoid the loop
 * and have this function return in constant time.
 */
static uint8_t* ptr_for_node(size_t index, size_t bucket) {
    return base_ptr + ((index - (1 << bucket) + 1) << (MAX_ALLOC_LOG2 - bucket));
}

/*
 * This maps from an address of memory to the node that represents that
 * address. There are often many nodes that all map to the same address, so
 * the bucket is needed to uniquely identify a node.
 */
static size_t node_for_ptr(uint8_t* ptr, size_t bucket) {
    return ((ptr - base_ptr) >> (MAX_ALLOC_LOG2 - bucket)) + (1 << bucket) - 1;
}
/*
 * Given the index of a node, this returns the "is split" flag of the parent.
 */
static int parent_is_split(size_t index) {
    index = (index - 1) / 2;
    return (node_is_split[index / 8] >> (index % 8)) & 1;
}

/*
 * Given the index of a node, this flips the "is split" flag of the parent.
 */
static void flip_parent_is_split(size_t index) {
    index = (index - 1) / 2;
    node_is_split[index / 8] ^= 1 << (index % 8);
}

/*
 * Given the requested size passed to "malloc", this function returns the index
 * of the smallest bucket that can fit that size.
 */
static size_t bucket_for_request(size_t request) {
    size_t bucket = BUCKET_COUNT - 1;
    size_t size = MIN_ALLOC;

    while (size < request) {
        bucket--;
        size *= 2;
    }

    return bucket;
}

/*
 * The tree is always rooted at the current bucket limit. This call grows the
 * tree by repeatedly doubling it in size until the root lies at the provided
 * bucket index. Each doubling lowers the bucket limit by 1.
 */
static int lower_bucket_limit(size_t bucket) {
    while (bucket < bucket_limit) {
        size_t root = node_for_ptr(base_ptr, bucket_limit);
        uint8_t* right_child;

        /*
         * If the parent isn't SPLIT, that means the node at the current bucket
         * limit is UNUSED and our address space is entirely free. In that case,
         * clear the root free list, increase the bucket limit, and add a single
         * block with the newly-expanded address space to the new root free list.
         */
        if (!parent_is_split(root)) {
            list_remove((list_t*)base_ptr);
            list_init(&buckets[--bucket_limit]);
            list_push(&buckets[bucket_limit], (list_t*)base_ptr);
            continue;
        }

        /*
         * Otherwise, the tree is currently in use. Create a parent node for the
         * current root node in the SPLIT state with a right child on the free
         * list. Make sure to reserve the memory for the free list entry before
         * writing to it. Note that we do not need to flip the "is split" flag for
         * our current parent because it's already on (we know because we just
         * checked it above).
         */
        right_child = ptr_for_node(root + 1, bucket_limit);
        if (!update_max_ptr(right_child + sizeof(list_t))) {
            return 0;
        }
        list_push(&buckets[bucket_limit], (list_t*)right_child);
        list_init(&buckets[--bucket_limit]);

        /*
         * Set the grandparent's SPLIT flag so if we need to lower the bucket limit
         * again, we'll know that the new root node we just added is in use.
         */
        root = (root - 1) / 2;
        if (root != 0) {
            flip_parent_is_split(root);
        }
    }

    return 1;
}

void initMemory() {

    base_ptr = start;
    max_ptr = start;
    bucket_limit = BUCKET_COUNT - 1;
    update_max_ptr(base_ptr + sizeof(list_t));
    list_init(&buckets[BUCKET_COUNT - 1]);
    list_push(&buckets[BUCKET_COUNT - 1], (list_t*)base_ptr);

}


void* malloc(uint32_t request) {
    size_t original_bucket, bucket;

    /*
     * Make sure it's possible for an allocation of this size to succeed. There's
     * a hard-coded limit on the maximum allocation size because of the way this
     * allocator works.
     */
    if (request + HEADER_SIZE > MAX_ALLOC) {
        return NULL;
    }

    /*
     * Find the smallest bucket that will fit this request. This doesn't check
     * that there's space for the request yet.
     */
    bucket = bucket_for_request(request + HEADER_SIZE);
    original_bucket = bucket;

    /*
     * Search for a bucket with a non-empty free list that's as large or larger
     * than what we need. If there isn't an exact match, we'll need to split a
     * larger one to get a match.
     */
    while (bucket + 1 != 0) {
        size_t size, bytes_needed, i;
        uint8_t* ptr;

        /*
         * We may need to grow the tree to be able to fit an allocation of this
         * size. Try to grow the tree and stop here if we can't.
         */
        if (!lower_bucket_limit(bucket)) {
            return NULL;
        }

        /*
         * Try to pop a block off the free list for this bucket. If the free list
         * is empty, we're going to have to split a larger block instead.
         */
        ptr = (uint8_t*)list_pop(&buckets[bucket]);
        if (!ptr) {
            /*
             * If we're not at the root of the tree or it's impossible to grow the
             * tree any more, continue on to the next bucket.
             */
            if (bucket != bucket_limit || bucket == 0) {
                bucket--;
                continue;
            }

            /*
             * Otherwise, grow the tree one more level and then pop a block off the
             * free list again. Since we know the root of the tree is used (because
             * the free list was empty), this will add a parent above this node in
             * the SPLIT state and then add the new right child node to the free list
             * for this bucket. Popping the free list will give us this right child.
             */
            if (!lower_bucket_limit(bucket - 1)) {
                return NULL;
            }
            ptr = (uint8_t*)list_pop(&buckets[bucket]);
        }

        /*
         * Try to expand the address space first before going any further. If we
         * have run out of space, put this block back on the free list and fail.
         */
        size = (size_t)1 << (MAX_ALLOC_LOG2 - bucket);
        bytes_needed = bucket < original_bucket ? size / 2 + sizeof(list_t) : size;
        if (!update_max_ptr(ptr + bytes_needed)) {
            list_push(&buckets[bucket], (list_t*)ptr);
            return NULL;
        }

        /*
         * If we got a node off the free list, change the node from UNUSED to USED.
         * This involves flipping our parent's "is split" bit because that bit is
         * the exclusive-or of the UNUSED flags of both children, and our UNUSED
         * flag (which isn't ever stored explicitly) has just changed.
         *
         * Note that we shouldn't ever need to flip the "is split" bit of our
         * grandparent because we know our buddy is USED so it's impossible for our
         * grandparent to be UNUSED (if our buddy chunk was UNUSED, our parent
         * wouldn't ever have been split in the first place).
         */
        i = node_for_ptr(ptr, bucket);
        if (i != 0) {
            flip_parent_is_split(i);
        }

        /*
         * If the node we got is larger than we need, split it down to the correct
         * size and put the new unused child nodes on the free list in the
         * corresponding bucket. This is done by repeatedly moving to the left
         * child, splitting the parent, and then adding the right child to the free
         * list.
         */
        while (bucket < original_bucket) {
            i = i * 2 + 1;
            bucket++;
            flip_parent_is_split(i);
            list_push(&buckets[bucket], (list_t*)ptr_for_node(i + 1, bucket));
        }

        /*
         * Now that we have a memory address, write the block header (just the size
         * of the allocation) and return the address immediately after the header.
         */
        *(size_t*)ptr = request;
        return ptr + HEADER_SIZE;
    }

    return NULL;
}

void free(void* ptr) {
    size_t bucket, i;

    /*
     * Ignore any attempts to free a NULL pointer.
     */
    if (!ptr) {
        return;
    }

    /*
     * We were given the address returned by "malloc" so get back to the actual
     * address of the node by subtracting off the size of the block header. Then
     * look up the index of the node corresponding to this address.
     */
    ptr = (uint8_t*)ptr - HEADER_SIZE;
    bucket = bucket_for_request(*(size_t*)ptr + HEADER_SIZE);
    i = node_for_ptr((uint8_t*)ptr, bucket);

    /*
     * Traverse up to the root node, flipping USED blocks to UNUSED and merging
     * UNUSED buddies together into a single UNUSED parent.
     */
    while (i != 0) {
        /*
         * Change this node from UNUSED to USED. This involves flipping our
         * parent's "is split" bit because that bit is the exclusive-or of the
         * UNUSED flags of both children, and our UNUSED flag (which isn't ever
         * stored explicitly) has just changed.
         */
        flip_parent_is_split(i);

        /*
         * If the parent is now SPLIT, that means our buddy is USED, so don't merge
         * with it. Instead, stop the iteration here and add ourselves to the free
         * list for our bucket.
         *
         * Also stop here if we're at the current root node, even if that root node
         * is now UNUSED. Root nodes don't have a buddy so we can't merge with one.
         */
        if (parent_is_split(i) || bucket == bucket_limit) {
            break;
        }

        /*
         * If we get here, we know our buddy is UNUSED. In this case we should
         * merge with that buddy and continue traversing up to the root node. We
         * need to remove the buddy from its free list here but we don't need to
         * add the merged parent to its free list yet. That will be done once after
         * this loop is finished.
         */
        list_remove((list_t*)ptr_for_node(((i - 1) ^ 1) + 1, bucket));
        i = (i - 1) / 2;
        bucket--;
    }

    /*
     * Add ourselves to the free list for our bucket. We add to the back of the
     * list because "malloc" takes from the back of the list and we want a "free"
     * followed by a "malloc" of the same size to ideally use the same address
     * for better memory locality.
     */
    list_push(&buckets[bucket], (list_t*)ptr_for_node(i, bucket));
}

char** getMemoryInfo(uint64_t* size) {
    return NULL;
    /*int freeMemory = 0;
    int usedMemory = 0;
    int blocksUsed = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (blockArray[i].free) {
            freeMemory++;
        } else {
            blocksUsed++;
            usedMemory += blockArray[i].size;
        }
    }
    *size = 3;
    char ** info = malloc((*size) * sizeof(char *));
    int i;
    for(i = 0; i < *size; i++) {
        info[i] = malloc(100);
    }

    int offset = 0;
    offset += strcpy(info[0], "Memoria total: ");
    offset += uintToBase(TOTAL_MEM, info[0] + offset, 10);
    offset += strcpy(info[0] + offset, " bytes");
    info[0][offset] = 0;
    offset = 0;
    offset += strcpy(info[1], "Memoria en uso: ");
    offset += uintToBase(usedMemory, info[1] + offset, 10);
    offset += strcpy(info[1] + offset, " bytes");
    info[1][offset] = 0;
    offset = 0;
    offset += strcpy(info[2], "Bloques en uso: ");
    offset += uintToBase(blocksUsed, info[2] + offset, 10);
    info[2][offset] = 0;
    return info;*/
}
#endif
