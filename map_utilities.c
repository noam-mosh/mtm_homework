#include <stdlib.h>
#include <assert.h>
#include "map.h"

#define INITIAL_SIZE 10;
#define EXPAND_FACTOR 2;
#define UNDEFINED_ITERATOR -1;
#define SWAP(a,b) swap(&(a),&(b),sizeof(a))

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