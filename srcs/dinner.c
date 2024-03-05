#include "../include/philosophers.h"

//🛡️: return 0 or 1 Lines: ✅
int	thinking(t_philo *philo, bool pre_simulation)
{
	long	t_eat;
	long	t_sleep;
	long	t_think;

	if (!pre_simulation)
	{
		if (write_status(THINKING, philo, DEBUG_MODE))
			return (1);
	}
	if (philo->table->philo_nbr % 2 == 0)
		return (0);
	t_eat = philo->table->time_to_eat;
	t_sleep = philo->table->time_to_sleep;
	t_think = (t_eat * 2) - t_sleep;
	if (t_think < 0)
		t_think = 0;
	if (precise_usleep(t_think * 0.42, philo->table))
		return (1);
	return (0);
}

//🛡️: return NULL Lines: ✅
void	*lone_philo(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	wait_all_threads(philo->table);
	set_long(&philo->philo_mutex,
		&philo->last_meal_time, get_time(MILLISECOND));
	increase_long(&philo->table->table_mutex,
		&philo->table->threads_running_nbr);
	write_status(TAKE_FIRST_FORK, philo, DEBUG_MODE);
	if (write_status(TAKE_FIRST_FORK, philo, DEBUG_MODE))
		return (NULL);
	if (is_error(simulation_finished(philo->table)))
		return (NULL);
	while (!is_true(simulation_finished(philo->table)))
	{
		if (precise_usleep(200, philo->table))
			return (NULL);
	}
	return (NULL);
}


//🛡️: return 1 or 0 Lines: ✅
static int	eat(t_philo *philo)
{
	if (mutex_handler(&philo->first_fork->fork, LOCK))
		return (1);
	if (write_status(TAKE_FIRST_FORK, philo, DEBUG_MODE))
		return (1);
	if (mutex_handler(&philo->second_fork->fork, LOCK))
		return (1);
	if (write_status(TAKE_SECOND_FORK, philo, DEBUG_MODE))
		return (1);
	if (set_long(&philo->philo_mutex,
			&philo->last_meal_time, get_time(MILLISECOND)))
		return (1);
	philo->meals_counter++;
	if (write_status(EATING, philo, DEBUG_MODE))
		return (1);
	if (precise_usleep(philo->table->time_to_eat, philo->table))
		return (1);
	if (philo->table->nbr_limit_meals > 0
		&& philo->meals_counter == philo->table->nbr_limit_meals)
	{
		if (set_bool(&philo->philo_mutex, &philo->full, true))
			return (1);
	}
	if (mutex_handler(&philo->first_fork->fork, UNLOCK))
		return (1);
	if (mutex_handler(&philo->second_fork->fork, UNLOCK))
		return (1);
	return (0);
}

//🛡️: return NULL Lines: 🦉
static void	*dinner_simulation(void *data)
{
	t_philo		*philo;

	philo = (t_philo *)data;

	if (wait_all_threads(philo->table))
		return (NULL);
	if (set_long(&philo->philo_mutex, &philo->last_meal_time,
		get_time(MILLISECOND)))
		return (NULL);
	if (increase_long(&philo->table->table_mutex,
		&philo->table->threads_running_nbr))
		return (NULL);
	if (de_synchronize_philos(philo))
		return (NULL);
	if (is_error(simulation_finished(philo->table)))
		return (NULL);
	while (!is_true(simulation_finished(philo->table)))
	{
		if (is_error(get_bool(&philo->philo_mutex, &philo->full)))
			return (NULL);
		if (is_true(get_bool(&philo->philo_mutex, &philo->full)))
			break ;
		if (eat(philo))
			return (NULL);
		if (write_status(SLEEPING, philo, DEBUG_MODE))
			return (NULL);
		if (precise_usleep(philo->table->time_to_sleep, philo->table))
			return (NULL);
		if (thinking(philo, false))
			return (NULL);
	}
	return (NULL);
}	

//🛡️: return NULL Lines: 🦉
int	dinner_start(t_table *table)
{
	int			i;

	i = -1;
	if (table->nbr_limit_meals == 0)
		return (0);
	else if (table->philo_nbr == 1)
	{
		if (thread_handler(&table->philos[0].thread_id, lone_philo,
				&table->philos[0], CREATE))
			return (1);
	}
	else
	{
		while (++i < table->philo_nbr)
		{
			if (thread_handler(&table->philos[i].thread_id, dinner_simulation,
					&table->philos[i], CREATE))
				return (1);
		}
	}
	if (thread_handler(&table->monitor, monitor_dinner, table, CREATE))
		return (1);
	if (!get_time(MILLISECOND))
		return (1);
	table->start_simulation = get_time(MILLISECOND);
	if (set_bool(&table->table_mutex, &table->all_threads_ready, true))
		return (1);
	i = -1;
	while (++i < table->philo_nbr)
	{
		if (thread_handler(&table->philos[i].thread_id, NULL, NULL, JOIN))
			return (1);
	}
	if (set_bool(&table->table_mutex, &table->end_simulation, true))
		return (1);
	if (thread_handler(&table->monitor, NULL, NULL, JOIN))
		return (1);
	return (0);
}