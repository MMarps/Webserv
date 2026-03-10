/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigPrint.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:44:23 by mmarps            #+#    #+#             */
/*   Updated: 2026/03/10 15:23:02 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPRINT_HPP
# define CONFIGPRINT_HPP

# include <iostream>
# include "Config.hpp"

void	printConfig(Config const &cfg, std::ostream &os = std::cout);

#endif