#include "../include/philosophers.h"

int	main(int ac, char **av)
{
	t_table	table;

	if (5 == ac || 6 == ac)
	{
		if (parse_input(&table, av))
			return (EXIT_FAILURE);
		if (data_init(&table))
			return (EXIT_FAILURE);
		if (dinner_start(&table))
			return (EXIT_FAILURE);
		if (clean(&table))
			return (EXIT_FAILURE);
	}
	else
		return (c_error(CHECK_ARG_NUMBER));
	return (0);
}

