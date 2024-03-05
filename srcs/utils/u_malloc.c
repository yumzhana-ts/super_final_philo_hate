#include "../../include/philosophers.h"

//🛡️: return mem or NULL Lines: ✅
void    *malloc_handler(size_t bytes)
{
    void    *memory_block;
    
    memory_block = malloc(bytes);
    if(memory_block == NULL)
    {
        c_error(INVALID_MALLOC);
        return(NULL);
    }
    return(memory_block);    
}