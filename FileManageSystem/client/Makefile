SRCS:=$(wildcard *.c)
TARGETS:=client
CC:=gcc
$(TARGETS):$(SRCS)
		gcc $^ -o $@ -pthread -lssl -lcrypto  -Wall -lcrypt -I /usr/include/mysql/ -lmysqlclient
clean:
		rm -rf $(TARGETS)

