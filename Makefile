COMPILER=gcc

library:
	@$(COMPILER) -c src/*.c -Iinclude/ -o bin/library.o
	@echo "built library.o"

project:
	$(COMPILER) -c main.c -Iinclude -o bin/main.o
	@$(COMPILER) bin/library.o -o MessagePassing
	@echo "Finished building MessagePassingLibrary"