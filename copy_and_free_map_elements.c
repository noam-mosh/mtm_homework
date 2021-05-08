#include <stdlib.h>
#include <assert.h>
#include "map.h"

//static MapKeyElement copyInt(MapKeyElement key_element)
//{
//    if (key_element == NULL) {
//        return NULL;
//    }
//    int* newInt = malloc(sizeof(int));
//    if (newInt == NULL)
//    {
//        return NULL;
//    }
//    *newInt = *(int*) key_element;
//    return newInt;
//}
//
//static void freeInt(MapKeyElement key_element)
//{
//    free(key_element);
//}
//
//static void freeTournament(MapDataElement data_element)
//{
//    free(data_element);
//}
//
//static int compareInt(MapKeyElement key_element1, MapKeyElement key_element2)
//{
//    assert(key_element1 != NULL && key_element2 != NULL);
//    int key1 = *(* int) key_element1;
//    int key2 = *(* int) key_element2;
//    return (key1 - key2);
//}