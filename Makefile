BINARY_RELEASE = aidenoc-2022
BINARY_DEBUG   = $(BINARY_RELEASE)-debug

PKG_CONFIG =

INCLUDE = -I include
CFLAGS_COMMON = $(INCLUDE) -Wall -Wextra -MD -MP
CFLAGS_RELEASE  = -O3 -s -fexpensive-optimizations $(CFLAGS_COMMON)
CFLAGS_DEBUG = -O0 -g $(CFLAGS_COMMON)
# LDFLAGS = `pkg-config --libs $(PKG_CONFIG)` -lm
LDFLAGS = -lm

SRC = $(shell find src -type f)
OBJ_RELEASE = $(patsubst src/%.c, .build/%.o, $(SRC))
DEP_RELEASE = $(patsubst src/%.c, .build/%.d, $(SRC))
OBJ_DEBUG = $(patsubst src/%.c, .build/debug/%.o, $(SRC))
DEP_DEBUG = $(patsubst src/%.c, .build/debug/%.d, $(SRC))

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

all : release

release : $(BINARY_RELEASE)

debug : $(BINARY_DEBUG)

.build/%.o: src/%.c
	mkdir -p $(basename $@)
	$(CC) -c $(CFLAGS_RELEASE) -o $@ $<

.build/debug/%.o: src/%.c
	mkdir -p $(basename $@)
	$(CC) -c $(CFLAGS_DEBUG) -o $@ $<

$(BINARY_RELEASE) : $(OBJ_RELEASE)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BINARY_DEBUG) : $(OBJ_DEBUG)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf .build $(BINARY_RELEASE) $(BINARY_DEBUG)

install: $(BINARY_RELEASE)
	install -d $(BINDIR)
	install $(BINARY_RELEASE) $(BINDIR)

uninstall:
	rm -rf $(addprefix $(BINDIR)/,$(BINARY_RELEASE))

valgrind : $(BINARY_DEBUG)
	valgrind -v --leak-check=full ./$<

cppcheck.log : $(SRC)
	cppcheck -I include --enable=all --force $^ > $@ 2>&1

-include $(DEP) $(DEP_DEBUG)

.PHONY : all release debug clean install uninstall valgrind
