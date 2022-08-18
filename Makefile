# Put sources and objects shit + link stuff and don't link for tests
SRC = lexer.c parser.c error_reporter.c printer.c
OBJ = $(SRC:%.c=build/%.o)
EXEC = s-lang
CC ?= gcc
CFLAGS += -std=c99 -I . -I cake_libs/                                \
		  		-Wall -Wextra -Wformat=2 -Wshadow                          \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs  \
		  		-Wno-unused-parameter -Wno-unused-command-line-argument    \
		  		-Wno-unused-function -Werror -O0 -ggdb

ifeq ($(TEST), +)
  CFLAGS += -DTEST -O0 -ggdb
else
  # CFLAGS += -O3
endif

.PHONY: $(EXEC) lexer_test error_reporter_test parser_test clean

$(EXEC): bin/$(EXEC)

bin/$(EXEC): $(OBJ)
	@mkdir -p bin
	$(CC) $^ -o $@
	@termux-elf-cleaner $@

build/%.o: %.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $^ -o $@

lexer_test: build/lexer.o
	@mkdir -p bin
	$(CC) $^ -o bin/$@
	@termux-elf-cleaner bin/$@

error_reporter_test: build/error_reporter.o
	@mkdir -p bin
	$(CC) $^ -o bin/$@
	@termux-elf-cleaner bin/$@

parser_test: build/parser.o build/lexer.o build/error_reporter.o	\
						 build/printer.o
	@mkdir -p bin
	$(CC) $^ -o bin/$@
	@termux-elf-cleaner bin/$@


clean:
	@if [ -d build ]; then rm -rf build; fi
	@if [ -d bin ]; 	then rm -rf bin; 	 fi
