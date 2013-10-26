BIN_NAME:=test
BIN_ARGS:=

TEST_LIB_NAME:=libbuf

LIBS:=pthread
DEBUG:=-g

CFLAGS:=
LFLAGS:=

CFLAGS+=$(DEBUG)
LFLAGS+=$(DEBUG)
LFLAGS+=$(addprefix -l,$(LIBS))

BIN_DIR:=bin
OBJ_DIR:=obj
DIRS:=$(BIN_DIR) $(OBJ_DIR)

SRCS:=$(wildcard *.c)
OBJS:=$(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

.PRECIOUS: $(OBJ_DIR)/%.d


all: $(BIN_DIR)/$(BIN_NAME)

new: clean
	$(MAKE) all

clean:
	rm -rf $(addsuffix /*,$(DIRS))

distclean:
	rm -rf $(DIRS) $(addprefix .,$(addsuffix .dir,$(DIRS)))

run: $(BIN_DIR)/$(BIN_NAME)
	$^ $(BIN_ARGS)


.%.dir:
	mkdir $*
	touch $@

$(BIN_DIR)/$(BIN_NAME): $(OBJS) ../lib/$(TEST_LIB_NAME).o .$(BIN_DIR).dir
	gcc -o $@ $(filter %.o,$^) $(LFLAGS)

../lib/$(TEST_LIB_NAME).o:
	$(MAKE) -C .. ./lib/$(TEST_LIB_NAME).o

$(OBJS): $(OBJ_DIR)/%.o: .$(OBJ_DIR).dir
	gcc -MMD $*.c -c -o $@ $(CFLAGS)

-include $(wildcard $(OBJ_DIR)/*.d)
