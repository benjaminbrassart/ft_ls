# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/08 09:55:50 by bbrassar          #+#    #+#              #
#    Updated: 2023/05/08 13:56:50 by bbrassar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := ft_ls
NAME_LIBFT := libft/libft.a

CC := cc
CFLAGS := -Wall
CFLAGS += -Werror
CFLAGS += -Wextra
CFLAGS += -c
CFLAGS += -MMD -MP
CFLAGS += -I.
CFLAGS += -g3

LDFLAGS := -L $(dir $(NAME_LIBFT))
LDLIBS := -lft

RM := rm -vf
MKDIR := mkdir -vp

DIR_SRC := src
DIR_OBJ := obj

SRC := main.c
SRC += options.c
OBJ := $(SRC:%.c=$(DIR_OBJ)/%.o)
DEP := $(OBJ:.o=.d)

$(NAME): $(OBJ) $(NAME_LIBFT)
	$(CC) $(filter %.o,$^) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ): $(DIR_OBJ)/%.o: $(DIR_SRC)/%.c
	@$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@

-include $(DEP)

$(NAME_LIBFT):
	$(MAKE) $(MAKEFLAGS) -C $(@D) $(@F)

.PHONY: all clean fclean re

all: $(NAME)

clean:
	@$(RM) -r $(DIR_OBJ)

fclean: clean
	@$(RM) $(NAME)

re: fclean all
