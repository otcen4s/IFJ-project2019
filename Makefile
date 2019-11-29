# IFJ2019

PATHS = src/
PATHT = test/
PATHB = bin/

CC := gcc
CFLAGS = -Wall -g -W -std=c99 -I unity/src unity/src/unity.c

SRC = $(wildcard $(PATHT)*.c)
BIN = $(addprefix $(PATHB), $(basename $(notdir $(SRC))))

test : $(PATHB) $(BIN)
	#./$(PATHB)string_lib_test
	./$(PATHB)scanner_test
	#./$(PATHB)stack_test
	./$(PATHB)parser_test
	#./$(PATHB)symtable_test
	#./$(PATHB)generator_test
	#./$(PATHB)expr_test
	### MEZIKOD ###
	#cat -n program.code
	### OUTPUT ###
	#./ic19int program.code

$(PATHB) :
	mkdir $(PATHB)

$(PATHB)scanner_test : $(PATHT)scanner_test.c $(PATHS)scanner.c $(PATHS)string_lib.c $(PATHS)stack.c
	$(CC) $(CFLAGS) $^ -o $@

$(PATHB)string_lib_test : $(PATHT)string_lib_test.c $(PATHS)string_lib.c
	$(CC) $(CFLAGS) $^ -o $@

$(PATHB)stack_test : $(PATHT)stack_test.c $(PATHS)stack.c
	$(CC) $(CFLAGS) $^ -o $@

$(PATHB)generator_test : $(PATHT)generator_test.c $(PATHS)generator.c $(PATHS)string_lib.c $(PATHS)stack.c
	$(CC) $(CFLAGS) $^ -o $@

$(PATHB)parser_test : $(PATHT)parser_test.c $(PATHS)parser.c $(PATHS)string_lib.c $(PATHS)scanner.c $(PATHS)symtable.c $(PATHS)stack.c $(PATHS)function_str.c $(PATHS)expr_parser.c $(PATHS)generator.c
	$(CC) $(CFLAGS) $^ -o $@

$(PATHB)symtable_test : $(PATHT)symtable_test.c $(PATHS)symtable.c $(PATHS)string_lib.c
	$(CC) $(CFLAGS) $^ -o $@
	
$(PATHB)expr_test : $(PATHT)expr_test.c $(PATHS)parser.c $(PATHS)string_lib.c $(PATHS)scanner.c $(PATHS)symtable.c $(PATHS)stack.c $(PATHS)expr_parser.c  $(PATHS)scanner.c $(PATHS)generator.c
	$(CC) $(CFLAGS) $^ -o $@

main : $(PATHS)main.c $(PATHS)parser.c $(PATHS)string_lib.c $(PATHS)scanner.c $(PATHS)symtable.c $(PATHS)stack.c $(PATHS)function_str.c $(PATHS)expr_parser.c $(PATHS)generator.c
	$(CC) -Wall -g -W -std=c99 $^ -o $(PATHB)$@

clean :
	rm -r $(PATHB)

odovzdanie:
	mkdir xzitna02
	cp ./src/* ./xzitna02
	cp ./rozdeleni ./xzitna02
	cp ./dokumentace.pdf ./xzitna02
	cp ./makefile_odovzdanie ./xzitna02/Makefile
	cd ./xzitna02 && zip -r ../xzitna02.zip ./ && cd ..
	rm -r ./xzitna02
	./is_it_ok.sh ./xzitna02.zip ./temp_folder
	rm -r ./temp_folder