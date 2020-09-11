

myapi_lib=libmyapi.so
myapi_version=1.0
myapi=$(myapi_lib).$(myapi_version)
src = test.c
all: $(myapi) mylib.so

$(myapi): myapi.o
	gcc -shared -Wl,-soname,$(@:%.0=%) -o $@ $^  
	
myapi.o: api.c
	gcc -fPIC -o $@ -c $^

#mylib.o: mylib.c
#	gcc -fPIC -o $@ -c $^ 
clean:
	rm -rf *.so *.o
