#Modules
XTELE_EXT=test_ext.so term_keyb_input.so x11.so xv_out.so channels.so
INCLUDE=-I../libxtele
X_LIBS=-L/usr/X11R6/lib -lX11 -lXext
X_INCLUDE=$(INCLUDE) -I/usr/X11R6/include
XV_LIBS=$(X_LIBS) -lXv

all: $(XTELE_EXT) 
	
test_ext.so: test_ext.c
	gcc -o $@ -shared $< $(CFLAGS) $(INCLUDE)

term_keyb_input.so: term_keyb_input.c
	gcc -o $@ -shared $< $(CFLAGS) $(INCLUDE) -lpthread

x11.so: x11.c 
	gcc -o $@ -shared $< $(X_INCLUDE) $(X_LIBS) $(CFLAGS) -lpthread

xv_out.so: xv_out.c
	gcc -o $@ -shared $< $(X_INCLUDE) $(XV_LIBS) $(CFLAGS) 

channels.so: channels.c
	gcc -o $@ -shared $< $(CFLAGS) $(INCLUDE)

xml-test.so: xml-test.c
	gcc -o $@ -shared $< $(CFLAGS) $(INCLUDE)

clean:
	rm -f $(XTELE_EXT)  

