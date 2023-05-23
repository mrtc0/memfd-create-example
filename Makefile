all:
	gcc -o loader loader.c -ldl -lcurl -DCURL_STATICLIB
	gcc -o hello.so -shared -fPIC hello.c

