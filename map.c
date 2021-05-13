#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "map.h"

#define INITIAL_SIZE 10;
#define EXPAND_FACTOR 2;
#define UNDEFINED_ITERATOR -1;
#define SWAP(a,b) swap(&(a),&(b),sizeof(a));

/** The Map is implemented as an array of data and key Elements.
 *  With nextIndex as an index to the next available position and
 *  maximal size stored in maxsize.
 */

/** functions declarations **/
Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement
              compareMapKeyElements compareKeyElements);
void mapDestroy(Map map);

static MapResult allocate_memory_for_data_key(Map map);
int mapGetSize(Map map);

typedef struct element{
    MapDataElement* data;
    MapKeyElement* key;
} *Element;

struct map_t{
    Element array;
    int nextIndex;
    int maxSize;
    int iterator;
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
};

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement
              compareMapKeyElements compareKeyElements)
{
    if (!copyMapDataElements ||
    !copyMapKeyElements ||
    !freeMapDataElements ||
    !freeMapKeyElements ||
    !compareMapKeyElements)
    {
        return NULL;
    }
    Map map = malloc(sizeof(*map));
    if (map == NULL)
    {
        return NULL;
    }
    map->array = malloc(INITIAL_SIZE *sizeof(Element));
    if (map->array == NULL)
    {
        free(map);
        return NUll;
    }
    if(allocate_memory_for_data_key(map) == MAP_OUT_OF_MEMORY)
    {
        return NUll;
    }
    map->nextIndex = 0;
    map->maxSize = INITIAL_SIZE;
    map->iterator = UNDEFINED_ITERATOR;
    map->copyDataElement = copyDataElement;
    map->copyKeyElement = copyMapKeyElements;
    map->freeDataElement = freeMapDataElements;
    map->freeKeyElement = freeMapKeyElements;
    map->compareKeyElements = compareKeyElements;
    return map;
}

void mapDestroy(Map map)
{
    if (map == NULL)
    {
        return;
    }
    if (mapClear(map) == MAP_SUCCSESS)
    {
        free(map->array);
        free(map);
    }
    return;
}

Map mapCopy(Map map)
{
    Map map_copy = mapCreate(map->copyDataElement, map->copyKeyElement,
                             map->freeDataElement, map->freeKeyElement,
                             map->compareKeyElements);
    if (mapGetSize(map) > map_copy->maxSize)
    {
        int newSize = EXPAND_FACTOR * map->maxSize;
        if(expand(map_copy, newSize) == MAP_OUT_OF_MEMORY)
        {
            return NULL;
        }
    }
    map_copy->nextIndex = map_copy->nextIndex;
    map_copy->maxSize = map_copy->maxSize;
    map->iterator = UNDEFINED_ITERATOR;
    map_copy->iterator = UNDEFINED_ITERATOR;
    for (int i = 0; i < mapGetSize(map); i++)
    {
        MapKeyElement key_to_copy = (map->array[i])->key;
        MapDataElement data_to_copy = (map->array[i])->data;
        MapKeyElement key_replica = map->copyKeyElement(key_to_copy);
        MapDataElement data_replica = map->copyDataElement(data_to_copy);
        if (key_replica == NULL || data_replica == NULL)
        {
            return NULL;
        }
        (map_copy->array[i])->key = key_replica;
        (map_copy->array[i])->data = data_replica;
    }
    return map_copy;
}

int mapGetSize(Map map)
{
    if (map == NULL)
    {
        return -1;
    }
    return map->nextIndex;
}

bool mapContains(Map map, MapKeyElement element)
{
    if (map == NULL || element == NULL)
    {
        return false;
    }
    return (findKeyIndex(map, element) != map->nextIndex);
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if (map == NULL || keyElement == NULL || dataElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    bool key_is_new = true;
    int key_index = findKeyIndex(map, keyElement);
    Element element_to_update = map->array[key_index];
    if (key_index < map->nextIndex)
    {
        map->freeKeyElement(element_to_update->key);
        map->freeDataElement(element_to_update->data);
        key_is_new = false;
    }
    element_to_update->key = new_key;
    element_to_update->data = new_data;
    mapSort(map);
    if (key_is_new)
    {
        (map->nextIndex)++;
        if (map->nextIndex == (map->maxSize))
        {
            int newSize = EXPAND_FACTOR * map->maxSize;
            if (expand(map, newSize) == MAP_OUT_OF_MEMORY)
            {
                return MAP_OUT_OF_MEMORY;
            }
        }
    }
    map->iterator = UNDEFINED_ITERATOR;
    return MAP_SUCSSES;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    if (map == NULL || keyElement == NULL || !mapContains(map, keyElement))
    {
        return NULL;
    }
    int key_index = findKeyIndex(map, keyElement);
    return (map->array[key_index])->data;
}

MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if (map == NULL || keyElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    if (!mapContains(map))
    {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    int key_index = find(map, keyElement);
    int array_size = mapGetSize(map);
    MapKeyElement key_to_remove = (map->array[key_index])->key;
    MapDataElement data_to_remove = (map->array[key_index])->data;
    map->freeKeyElement(key_to_remove);
    map->freeDataElement(data_to_remove);

    for (int i = key_index; i < array_size-1; i++)
    {
        SWAP(map->array[i], map->array[i+1]);
    }
    map->nextIndex--;
    map->iterator = UNDEFINED_ITERATOR;
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map)
{
    if (map == NULL || map->nextIndex == 0)
    {
        return NULL;
    }
    map->iterator = 0;
    return mapGetNext(map);
}

MapKeyElement mapGetNext(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }
    if (map->iterator >= map->nextIndex || map->iterator == UNDEFINED_ITERATOR)
    {
        return NULL;
    }
    MapKeyElement next_key = (map->elements[map->iterator++]).key;
    MapKeyElement next_key_copy = map->copyKeyElement(next_key);
    return next_key_copy;
}

MapResult mapClear(Map map)
{
    if (map == NULL)
    {
        return MAP_NULL_ARGUMEMT;
    }
    Element array = map->array;
    for (int i = 0; i < mapGetSize(map); i++)
    {
        map->freeKeyElement(array[i].key);
        map->freeDataElement(array[i].data);
    }
    return MAP_SUCCSESS;
}

static MapResult expand(Map map, int size)
{
    Element new_array = realloc(map->array, size * sizeof(Element));
    if (new_array == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->array = new_array;
    map->maxSize = size;
    return MAP_SUCCESS;
}

static MapResult allocate_memory_for_data_key(Map map)
{
    int array_size = mapGetSize(map);
    Element array = map->array;
    for (int i = 0; i < array_size; i++)
    {
        MapDataElement* data = malloc(sizeof(data));
        if (data == NULL)
        {
            return MAP_OUT_OF_MEMORY;
        }
        MapKeyElement* key = malloc(sizeof(key));
        if (key == NULL)
        {
            map->freeMapDataElements(data);
            return MAP_OUT_OF_MEMORY;
        }
        array[i].data = data;
        array[i].key = key;
    }
    return MAP_SUCCESS;
}

static void mapSort(Map map)
{
    Element array = map->elments_array;
    int i = map->nextIndex;
    while (array[i] < array[i-1] && i > 0)
    {
        SWAP(array[i], array[i-1]);
        i--;
    }
}

int findKeyIndex(Map map, MapKeyElement keyElement)
{
    Element array = map->array;
    int array_size = mapGetSize(map);
    compareMapKeyElements compare_function = map->compareKeyElements;
    for (int i = 0; i < array_size; i++)
    {
       if(compare_function(keyElement, array[i].key) == 0)
       {
           return i;
       }
       if(compare_function(keyElement, array[i].key) < 0)
       {
           return i - 1;
       }
    }
    return array_size;
}