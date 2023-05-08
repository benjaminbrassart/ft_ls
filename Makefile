# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/08 09:55:50 by bbrassar          #+#    #+#              #
#    Updated: 2023/05/08 09:59:15 by bbrassar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := ft_ls

CC := cc
CFLAGS := -Wall
CFLAGS += -Werror
CFLAGS += -Wextra
CFLAGS += -c
CFLAGS += -MMD -MP

RM := rm -vf
MKDIR := mkdir -vp

DIR_SRC := src
DIR_OBJ := obj

SRC := main.c
OBJ := $(SRC:%.c=$(DIR_OBJ)/%.o)
DEP := $(OBJ:.o=.d)

$(NAME): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ): $(DIR_OBJ)/%.o: $(DIR_SRC)/%.c
	@$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@

-include $(DEP)

.PHONY: all clean fclean re

all: $(NAME)

clean:
	@$(RM) -r $(DIR_OBJ)

fclean: clean
	@$(RM) $(NAME)

re: fclean all
