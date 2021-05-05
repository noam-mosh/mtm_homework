#include <stdlib.h>
#include <assert.h>
#include "map.h"

#define INITIAL_SIZE 10;
#define EXPAND_FACTOR 2;
#define SWAP(a,b) swap(&(a),&(b),sizeof(a))
/** The Map is implemented as an array of data and key Elements.
 *  With nextIndex as an index to the next available position and
 *  maximal size stored in maxsize.
 */

typedef struct element{
    MapDataElement* data;
    MapKeyElement* key;
} *Element;

struct map_t{
    Element elements_array;
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
    map->elements_array = malloc(INITIAL_SIZE *sizeof(Element));
    if (map->elements_array == NULL)
    {
        free(map);
        return NUll;
    }
    map->nextIndex = 0;
    map->maxSize = INITIAL_SIZE;
    map->copyDataElement = copyDataElement;
    map->copyKeyElement = copyMapKeyElements;
    map->freeDataElement = freeMapDataElements;
    map->freeKeyElement = freeMapKeyElements;
    map->compareKeyElements = compareKeyElements;
    return map;
}

int mapGetSize(Map map)
{
    if (map == NULL)
    {
        return -1;
    }
    int map_size = 0;
    Map map_ptr = map;
    while(map_ptr->key != NULL)
    {
        map_size++;
        map_ptr += sizeof(Map);
    }
    return map_size;
}

bool mapContains(Map map, MapKeyElement element)
{
    if (map == NULL || element == NULL)
    {
        return false;
    }
    int map_size = mapGetSize(map);
    int map_size = mapGetSize(map);
    for (int i = 0; i < map_size; ++i)
    {
        if ((map+(i*sizeof(Map)))->key == element)
        {
            return true;
        }
    }
    return false;
}

static void mapReplaceData(Map key_ptr, MapKeyElement keyElement, MapDataElement dataElement)
{
    key_ptr->freeDataElement(key_ptr->data);
    key_ptr->data = key_ptr->copyDataElement(dataElement);
}

static void mapKeyRearrange(Map map, int key_index, int map_size, MapKeyElement keyElement, MapDataElement dataElement)
{
    Map new_map = realloc(map, sizeof(*map)*map_size+1);
    ///add flag for adding and removing from realloc
    if (new_map == NULL)
    {
        mapDestroy(map);
        return MAP_OUT_OF_MEMORY;
    }
    map = new_map;
    new_map = NULL;
    last_element = map + (sizeof(Map)*map_size)
    last_element->key = keyElement;
    last_element->data = dataElement;
    for (int i = 0; i < map_size-key_index; i++)
    {
        SWAP(last_element-i,last_element-i);
    }
}

int findIndexOfNewElement(Map map, MapKeyElement keyElement)
{
    Element array = map->elements_array;
    int array_size = map->nextIndex;
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

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if (map == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    if (mapContains(map, keyElement, dataElement))
    {
        mapKeyRearrange(map, )
    }
    if (map->nextIndex == map->maxSize)
    {
        expand(map);
    }
    MapKeyElement new_key_element = map->copy











    int map_size = mapGetsize(map);
    Map ptr = map;
    for (;ptr < map + (map_size * sizeof(Map)); ptr += sizeof(Map))
    {
        if (ptr->key == keyElement)
        {
            mapKeyReplace(ptr, keyElement, dataElement);
            return MAP_SUCSSES;
        }
        else if (ptr->key > keyElement)
        {
            break;
        }
    }
    int key_index = (ptr - map) / sizeof(Map);
    mapKeyRearrange(map, key_index, map_size+1, keyElement, dataElement);
    return MAP_SUCSSES;
}

MapKeyElement mapGetFirst(Map map)
{
    return map
}


MapResult mapClear(Map map)
{
    if (map == NULL)
    {
        return MAP_NULL_ARGUMEMT;
    }
    int map_size = mapGetSize(map);
    for (int i = 0; i < map_size; ++i)
    {
        Map current_ptr = (map + (i*sizeof(Map)));
        current_ptr->freeDataElement(current_ptr->data);
        current_ptr->freeKeyElement(current_ptr->key);
    }
    return MAP_SUCCSESS;
}


void mapDestroy(Map map)
{
    if (map == NULL)
    {
        return;
    }
    if (mapClear(map) == MAP_SUCCSESS)
    {
        free(map);
    }
    return;
}

