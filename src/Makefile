# Makefile for IFJ11 project.
# ===========================
CC= gcc
#XXX: odstranit a ostatne pred odovzdanim!  
#XXX: odstranene -Wswitch-enum koli parser.c 
CFLAGS+=  -Wall -std=c99 -pedantic
# CFLAGS for executable.
CFLAGS_EXEC= -fomit-frame-pointer -DNDEBUG -O2
# CFLAGS for executable which has compiled in debug support.
CFLAGS_DEBUG = -ggdb -DDEBUG -O0
LDFLAGS+= -lm

OBJS_EXEC= $(addsuffix .o, $(OBJS))
OBJS_DEBUG= $(addsuffix -debug.o, $(OBJS))

# XXX: Pri kazdom vytvoreni noveho modulu, pridat zaznam aj sem (bez .o)!
OBJS+= ./debug ./ial ./xwrappers ./str ./scanner ./fbuffer ./stack \
	   ./instruction ./pstack ./variable ./buildin ./interpret ./pparser \
	   ./parser
ROOTFILES+= *.c *.h Makefile rozsireni rozdeleni

# Output pack name for `make pack'
PACKNAME= xpokor32.tgz
PACKFILES= dokumentace.pdf

################################################################################

all:    ifj11
debug:  ifj11-debug

# Set up CFLAGS depending on output.
ifj11:          CFLAGS+=$(CFLAGS_EXEC)
ifj11-debug:    CFLAGS+=$(CFLAGS_DEBUG)

.PHONY: all debug clean help

clean:
	@echo "> Cleaning up executable build..."
	@rm -rf $(OBJS_EXEC) $(OBJS_DEBUG) $(PACKNAME) \
		ifj11 ifj11-debug *~ run.log && \
		echo "> DONE"

help:
	@echo 'Makefile for IFJ11 project.'
	@echo ''
	@echo 'make'
	@echo 'make ifj11		- build an executable'
	@echo ''
	@echo 'make debug'
	@echo 'make ifj11-debug	- build an executable and enable debug features'
	@echo ''
	@echo 'make clean		- clean up all files after build process'
	@echo 'make pack		- create a compressed archive'
	@echo 'make help		- print this simple help'

pack:
	@echo '> Making and compressing pack...'
	@cp DOC/dokumentace.pdf .
	@tar -zcf $(PACKNAME) $(ROOTFILES) $(PACKFILES) && rm DOC/dokumentace.pdf && \
		echo '> DONE'

################################################################################
# Uz len samotny projekt IFJ11

ifj11: $(OBJS_EXEC) main.c
	@echo "	LD	$@"
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

ifj11-debug: $(OBJS_DEBUG) main.c
	@echo "	LD	$@"
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.c
	@echo "	CC	$@"
	@$(CC) $(CFLAGS) $< -c -o $@

%-debug.o: %.c
	@echo "	CC	$@"
	@$(CC) $(CFLAGS) $< -c -o $@

# End of Makefile
