CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -pedantic
TARGET = s21_grep
SRC = s21_grep.c 
OBJ = $(SRC: .c = .o)
RM = rm -f
INTEGRATION = s21_grep.sh

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o:%.c
	$(CC) $(CFLAGS)  -c $< -o $@

test: $(INTEGRATION)    
	bash $(INTEGRATION) 

clean:
	$(RM) $(TARGET)
