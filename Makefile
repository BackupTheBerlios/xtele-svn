#Dossiers
LIBXTELE_DIR=libxtele
MODULES_DIR=modules

OBJ=main.o core.o core-module.o
XTELE_LIB= -Wl,--whole-archive -lxtele -Wl,--no-whole-archive
LIBS=-L$(LIBXTELE_DIR) -ldl $(XTELE_LIB)
INCLUDE= -I$(LIBXTELE_DIR)
LD_FLAGS=-Wl,--export-dynamic

all: $(MODULES_DIR) $(OBJ) $(LIBXTELE_DIR)
	gcc -o xtele $(OBJ) $(LIBS) $(LD_FLAGS) 

main.o: main.c
	gcc -c $< $(CFLAGS) $(INCLUDE)

core.o: core.c
	gcc -c $< $(CFLAGS) $(INCLUDE)
	
core-module.o:  core-module.c
	gcc -c $< $(CFLAGS) $(INCLUDE)

$(LIBXTELE_DIR):
	$(MAKE) -C $(LIBXTELE_DIR)

.PHONY: $(LIBXTELE_DIR) $(MODULES_DIR) clean
$(MODULES_DIR): $(LIBXTELE_DIR)
	$(MAKE) -C $@

clean: $(DIRS_CLEAN)
	$(MAKE) -C $(LIBXTELE_DIR) clean
	$(MAKE) -C $(MODULES_DIR) clean
	rm -f $(OBJ) xtele 

