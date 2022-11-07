NAME = user_system
CC = gcc
CFLAGS =
LDFLAGS = -lssl -lcrypto

SRCS_FILE = user keygen aes 

SRCS = $(addsuffix .c, $(SRCS_FILE))
OBJS = $(addsuffix .o, $(SRCS_FILE))

.PHONY : all clean flcean 

.c.o : $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $< 

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -c -o $@ $< 

all: $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

