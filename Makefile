NAME			:= webserv

CC				:= c++
CFLAGS			:= -Wall -Werror -Wextra -std=c++98

# Includes
INC				:= -Iincludes

# Colors
GREEN			:= \033[1;32m
BLUE			:= \033[1;34m
RED				:= \033[1;31m
NC				:= \033[0m

# Directories
SRC_DIR			:= srcs
OBJ_DIR			:= obj

# Source files (without .c)
FILES :=	main.cpp \
			Lexer.cpp \
			Parser.cpp \
			Server.cpp \
			Config.cpp \
			ConfigPrint.cpp \
			Request.cpp \
			Response.cpp \

# Source and object files
SRCS := $(addprefix $(SRC_DIR)/, $(FILES))
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INC) $(OBJS) -o $@
	@echo "$@ : $(BLUE)[READY]$(NC)"

# Pattern rules for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@
	@echo "$@ : $(GREEN)[OK]$(NC)"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)============== [OBJECT DELETED] ==============$(NC)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)========== [ OBJECT / EX DELETED ] ==========$(NC)"

re: fclean all

.PHONY: all clean fclean re