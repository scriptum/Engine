LUA_CFLAGS=`pkg-config lua --cflags`

all: stringy.so

stringy.so: stringy.c
	gcc $(LUA_CFLAGS) -O3 -fPIC -o stringy.o -c stringy.c
	gcc -shared -O3 -s stringy.o -o stringy.so

clean:
	rm stringy.so stringy.o
