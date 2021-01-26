SRCS:=$(wildcard *.c)
TARGETS:=server
CC:=gcc
$(TARGETS):$(SRCS)
		gcc $^ -o $@ -pthread -I../include  -lssl -lcrypto  -I /usr/include/mysql/ -lmysqlclient -Wall
clean:
		rm -rf $(TARGETS)

