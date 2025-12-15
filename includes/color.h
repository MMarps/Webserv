/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 13:04:21 by jle-doua          #+#    #+#             */
/*   Updated: 2024/12/09 16:50:48 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H

# define COLOR_H

// classique
# define YELLOW "\e[0;33m"
# define PURPLE "\e[0;35m"
# define WHITE "\e[0;37m"
# define BLACK "\e[0;30m"
# define GREEN "\e[0;32m"
# define BLUE "\e[0;34m"
# define CYAN "\e[0;36m"
# define RED "\e[0;31m"

// gras
# define BPURPLE "\e[1;35m"
# define BYELLOW "\e[1;33m"
# define BWHITE "\e[1;37m"
# define BGREEN "\e[1;32m"
# define BBLACK "\e[1;30m"
# define BBLUE "\e[1;34m"
# define BCYAN "\e[1;36m"
# define BRED "\e[1;31m"

// sombre
# define DPURPLE "\e[2;35m"
# define DYELLOW "\e[2;33m"
# define DWHITE "\e[2;37m"
# define DGREEN "\e[2;32m"
# define DBLACK "\e[2;30m"
# define DBLUE "\e[2;34m"
# define DCYAN "\e[2;36m"
# define DRED "\e[2;31m"

// italique
# define IPURPLE "\e[3;35m"
# define IYELLOW "\e[3;33m"
# define IWHITE "\e[3;37m"
# define IGREEN "\e[3;32m"
# define IBLACK "\e[3;30m"
# define IBLUE "\e[3;34m"
# define ICYAN "\e[3;36m"
# define IRED "\e[3;31m"

// souligné
# define UPURPLE "\e[4;35m"
# define UYELLOW "\e[4;33m"
# define UWHITE "\e[4;37m"
# define UGREEN "\e[4;32m"
# define UBLACK "\e[4;30m"
# define UBLUE "\e[4;34m"
# define UCYAN "\e[4;36m"
# define URED "\e[1;4;31m"

// couleur et bg inversé
# define RPURPLE "\e[7;35m"
# define RYELLOW "\e[7;33m"
# define RWHITE "\e[7;37m"
# define RGREEN "\e[7;32m"
# define RBLACK "\e[7;30m"
# define RBLUE "\e[7;34m"
# define RCYAN "\e[7;36m"
# define RRED "\e[7;31m"

// barré
# define LPURPLE "\e[9;35m"
# define LYELLOW "\e[9;33m"
# define LWHITE "\e[9;37m"
# define LGREEN "\e[9;32m"
# define LBLACK "\e[9;30m"
# define LBLUE "\e[9;34m"
# define LCYAN "\e[9;36m"
# define LRED "\e[9;31m"

# define NC "\e[0m"
#endif