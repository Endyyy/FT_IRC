GREEN = /bin/echo -e "\x1b[1;3;32m$1\x1b[0m"

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS =  Server.cpp \
		User.cpp \
		tools.cpp \
		main.cpp \

OBJ = $(SRCS:.cpp=.o)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

NAME = ircserv

all:		$(NAME)

$(NAME):	$(OBJ)
		$(CC) $(CFLAGS) $(OBJ) -o $@
		$(call GREEN,"Compilation success 😁")

clean:
		rm -rf $(OBJ)
		$(call GREEN,"The .o cleaned up !")

fclean:		clean
		rm -rf $(NAME)
		$(call GREEN,"The rest too !")

re:		fclean all

.SILENT:

.PHONY:		all clean fclean re 