#libxtele
LIBXTELE_OBJ=lib.o objects.o list.o messages.o xml-parser.o xml-writer.o conf.o

all: libxtele.a

#libxtele.so: $(LIBXTELE_OBJ) $(ADD_LIBS_DIR)
#	gcc -o $@ -shared $(LIBXTELE_OBJ)

libxtele.a: $(LIBXTELE_OBJ) 
	ar rc $@ $(LIBXTELE_OBJ) 

objects.o: objects.c
	gcc -c $< $(CFLAGS)

lib.o: lib.c
	gcc -c $< $(CFLAGS)

list.o: list.c
	gcc -c $< $(CFLAGS)

xml-parser.o: xml-parser.c
	gcc -c $< $(CFLAGS)

xml-writer.o: xml-writer.c
	gcc -c $< $(CFLAGS)

messages.o: messages.c
	gcc -c $< $(CFLAGS)

conf.o: conf.c
	gcc -c $< $(CFLAGS)


.PHONY: clean

clean: 
	rm -f $(LIBXTELE_OBJ) libxtele.so libxtele.a

