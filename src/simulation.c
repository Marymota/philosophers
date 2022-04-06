/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmota <mmota@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/08 18:32:17 by mmota             #+#    #+#             */
/*   Updated: 2022/04/06 17:38:01 by mmota            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	thinking(t_sim *sim, t_philos *philo)
{
	long int	current_time;

	if (!sim->dead)
	{
		pthread_mutex_lock(&sim->end);
		pthread_mutex_lock(&sim->write);
		current_time = get_time() - sim->start;
		printf("%li %i is thinking\n", current_time, philo->id);
		pthread_mutex_unlock(&sim->write);
		pthread_mutex_unlock(&sim->end);
		return (1);
	}
	return (0);
}

int	sleeping(t_sim *sim, t_philos *philo)
{
	long int	current_time;

	pthread_mutex_unlock(&philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
	if (!sim->dead)
	{
		pthread_mutex_lock(&sim->write);
		current_time = get_time() - sim->start;
		printf("%li %i is sleeping\n", current_time, philo->id);
		pthread_mutex_unlock(&sim->write);
		ft_usleep(sim->specs.time_to_sleep);
		return (1);
	}
	pthread_mutex_unlock(&philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
	return (0);
}

int	eating(t_sim *sim, t_philos *philo)
{
	long int	current_time;

	current_time = get_time() - sim->start;
	if (!sim->dead)
	{
		pthread_mutex_lock(&sim->end);
		printf("%li %i is eating\n", current_time, philo->id);
		pthread_mutex_unlock(&sim->end);
		pthread_mutex_lock(&sim->time_meal);
		philo->time_meal = current_time;
		pthread_mutex_unlock(&sim->time_meal);
		if (--philo->meals_count == 0)
			sim->finish_eat++;
		ft_usleep(sim->specs.time_to_eat);
		return (1);
	}
	return (0);
}

int	get_forks(t_sim *sim, t_philos *philo)
{
	long int	current_time;

	pthread_mutex_lock(&philo->left_fork);
	if (sim->dead)
	{
		pthread_mutex_unlock(&philo->left_fork);
		return (0);
	}
	pthread_mutex_lock(&sim->write);
	current_time = get_time() - sim->start;
	printf("%li %i has taken a fork\n", current_time, philo->id);
	pthread_mutex_unlock(&sim->write);
	pthread_mutex_lock(philo->right_fork);
	if (!sim->dead)
	{
		pthread_mutex_lock(&sim->write);
		current_time = get_time() - sim->start;
		printf("%li %i has taken a fork\n", current_time, philo->id);
		pthread_mutex_unlock(&sim->write);
		return (1);
	}
	return (0);
}

void	*action(void *arg)
{
	static int	i = -1;
	t_sim		*sim;
	t_philos	*philo;

	sim = (t_sim *)arg;
	pthread_mutex_lock(&sim->increment);
	philo = &sim->philos[++i];
	pthread_mutex_unlock(&sim->increment);
	if (sim->specs.n_philos == 1)
		return (0);
	if (philo->id % 2 != 0)
		ft_usleep(sim->specs.time_to_die / 10);
	while (1)
	{
		if (philo->meals_count == 0 || !get_forks(sim, philo))
			break ;
		if (!eating(sim, philo))
			break ;
		if (!sleeping(sim, philo))
			break ;
		if (!thinking(sim, philo))
			break ;
	}
	return (0);
}
