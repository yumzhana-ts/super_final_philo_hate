#include "../../include/philosophers.h"


//🛡️: return true/false Lines:✅ 
bool is_error(t_bool result) 
{
	if (result.error == 1)
		return (true);
	return (false);
}

//🛡️: return true/false Lines:✅
bool is_true(t_bool result) 
{
	if (result.error == 0 && result.value == true)
		return (true);
    return(false);
}