# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/08 09:55:50 by bbrassar          #+#    #+#              #
#    Updated: 2023/05/12 05:52:31 by bbrassar         ###   ########.fr        #
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
DIR_ASM := asm

SRC := main.c
SRC += options.c
SRC += exec.c
SRC += sort.c
SRC += libft_ext/ft_strcasecmp.c
OBJ := $(SRC:%.c=$(DIR_OBJ)/%.o)
DEP := $(OBJ:.o=.d)
ASM := $(SRC:%.c=$(DIR_ASM)/%.s)
DEP_ASM := $(SRC:.s=.d)

$(NAME): $(OBJ) $(NAME_LIBFT)
	$(CC) $(filter %.o,$^) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ): $(DIR_OBJ)/%.o: $(DIR_SRC)/%.c
	@$(MKDIR) $(@D)
	$(CC) $(CFLAGS) $< -o $@

-include $(DEP)

$(ASM): $(DIR_ASM)/%.s: $(DIR_SRC)/%.c
	@$(MKDIR) $(@D)
	$(CC) $(CFLAGS) -S -masm=intel $< -o $@

-include $(DEP_ASM)

$(NAME_LIBFT):
	$(MAKE) $(MAKEFLAGS) -C $(@D) $(@F)

.PHONY: all clean fclean re asm

all: $(NAME) $(ASM)

clean:
	@$(MAKE) -C $(dir $(NAME_LIBFT)) clean
	@$(RM) -r $(DIR_OBJ) $(DIR_ASM)

fclean: clean
	@$(MAKE) -C $(dir $(NAME_LIBFT)) fclean
	@$(RM) $(NAME)

re: fclean all

asm: $(ASM)

.NOTPARALLEL: re
