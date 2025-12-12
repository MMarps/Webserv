/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigPrint.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:44:23 by mmarps            #+#    #+#             */
/*   Updated: 2025/12/12 16:45:57 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPRINT_HPP
# define CONFIGPRINT_HPP

# include <iostream>
# include "Config.hpp"

void	printConfig(Config const &cfg, std::ostream &os = std::cout);

#endif // CONFIG_PRINTER_HPP