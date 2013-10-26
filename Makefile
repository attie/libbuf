SYS_PREFIX?=/usr
SYS_LIBDIR?=$(SYS_PREFIX)/lib
SYS_HEADERDIR?=$(SYS_PREFIX)/include

LIB_VERSION:=0
LIB_FULLREV:=$(LIB_VERSION).0.1
LIB_NAME:=buf

#DEFS+=EL_NO_VALID_CHECKS

LIBS:=
DEBUG:=-g

CFLAGS:=
LFLAGS:=

CFLAGS+=$(addprefix -DSDB_,$(DEFS))
CFLAGS+=$(DEBUG)
CFLAGS+=-fPIC
CFLAGS+=-fvisibility=hidden
CFLAGS+=-Wno-variadic-macros
LFLAGS+=$(DEBUG)
LFLAGS+=-fPIC
LFLAGS+=$(addprefix -l,$(LIBS))

LIB_DIR:=lib
OBJ_DIR:=obj
DIRS:=$(LIB_DIR) $(OBJ_DIR)

SRCS:=$(wildcard *.c)
OBJS:=$(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

.PRECIOUS: $(OBJ_DIR)/%.d


all: $(LIB_DIR)/lib$(LIB_NAME).so $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg

new: clean
	$(MAKE) all

clean:
	rm -rf $(addsuffix /*,$(DIRS))

distclean:
	rm -rf $(DIRS) $(addprefix .,$(addsuffix .dir,$(DIRS)))

test: all
	make -C test run


install: $(SYS_LIBDIR)/lib$(LIB_NAME).so $(SYS_LIBDIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg $(SYS_HEADERDIR)/buf.h
$(SYS_LIBDIR)/lib$(LIB_NAME).so: $(SYS_LIBDIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	ln -sf `basename $^` $@
$(SYS_LIBDIR)/lib$(LIB_NAME).so.$(LIB_FULLREV): $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	install -g root -o root -DT -m 755 $^ $@
$(SYS_LIBDIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg: $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg
	install -g root -o root -DT -m 755 $^ $@
$(SYS_HEADERDIR)/buf.h: buf.h
	install -g root -o root -DT -m 644 $^ $@


.%.dir:
	mkdir $*
	touch $@

$(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg: $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	objcopy --only-keep-debug $^ $@
	objcopy --add-gnu-debuglink=$@ $^
	objcopy --strip-debug $^
	touch $@

$(LIB_DIR)/lib$(LIB_NAME).so: $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	ln -sf `basename $^` $@

$(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV): $(LIB_DIR)/lib$(LIB_NAME).o
	gcc -shared -Wl,-soname,lib$(LIB_NAME).so.$(LIB_FULLREV) $(filter %.o,$^) $(LFLAGS) -o $@

$(LIB_DIR)/lib$(LIB_NAME).o: $(OBJS) .$(LIB_DIR).dir
	ld -r -o $@ $(filter %.o,$^)

$(OBJS): $(OBJ_DIR)/%.o: .$(OBJ_DIR).dir
	gcc -MMD $*.c -c -o $@ $(CFLAGS)
	
-include $(wildcard $(OBJ_DIR)/*.d)
