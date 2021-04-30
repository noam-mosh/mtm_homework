#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
/**  creates reversed string.
 returns pointer to the reversed string
 if fails, returns NULL */

char* reversStr(char* str, int* x)
{
    char* str2;
    int i, str_len;
    str_len = strlen(str);
    if (x != NULL)
    {
        *x = str_len;
    }
    str2 = (char*)malloc(str_len + 1);
    if (str2 == NULL)
    {
        return NULL;
    }
    for (i = 0; i < str_len; i++)
    {
        str2[i] = str[str_len - i - 1];
    }
    str2[str_len] = '\0';
    if (str_len % 2 != 0) {
        printf("%s", str);
    }
    if (str_len % 2 == 0)
    {
        printf("%s", str2);
    }
    return str2;
}

typedef struct node_t {
    int x;
    struct node_t *next;
} *Node;
typedef enum {
    SUCCESS=0,
    MEMORY_ERROR,
    EMPTY_LIST,
    UNSORTED_LIST,
    NULL_ARGUMENT,
} ErrorCode;

int getListLength(Node list);
bool isListSorted(Node list);

// ========================================
Node createNode(int value, Node previous)
{
    Node current_node = malloc(sizeof(*current_node));
    if (current_node == NULL)
    {
        return NULL;
    }
    current_node->x = value;
    current_node->next = NULL;

    if (previous != NULL)
    {
        previous->next = current_node;
    }
    return current_node;
}

bool isListIsInvalid(Node list)
{
    if (list == NULL)
    {
        *error_code = EMPTY_LIST;
        return true;
    }
    if (!isListSorted(list))
    {
        *error_code = UNSORTED_LIST;
        return true;
    }
    return false;
}

int getMaxValueAndMovePointer(Node list1, Node list2)
{
    if (current1->x > current2->x)
    {
        int greater_num = current1->x;
        current1 = current1->next;
        return greater_num
    }
    int greater_num = current2->x;
    current2 = current2->next;
    return greater_num;
}

void nodeDestroy(Node list) {
    while (list) {
        Node toDelete = list;
        list = list->next;
        free(toDelete);
    }
}


Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code)
{
    Node current1 = list1, current2 = list2;
    if (isListIsInvalid(current1) || isListIsInvalid(current2))
    {
        return NULL;
    }
    int greater_num = getMaxValueAndMovePointer(current1, current2);
    Node head = createNode(greater_num, NULL);
    if (head == NULL)
    {
        *error_code = MEMORY_ERROR;
        return NULL;
    }
    Node previous = head;

    while (current1 != NULL || current2 != NULL)
    {
        greater_num = getMaxValueAndMovePointer(current1, current2);
        previous = createNode(greater_num, previous);
        if (previous == NULL)
        {
            *error_code = MEMORY_ERROR;
            destroy(head);
            }
    }
    (current1 != NULL ? createNode(greater_num, current1) : createNode(greater_num, current2))
    *error_code = SUCCESS;
    return head;
}