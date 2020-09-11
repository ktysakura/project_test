

myapi_lib=libmyapi.so
myapi_major_version=1
myapi_minor_version=0
myapi=$(myapi_lib).$(myapi_major_version).$(myapi_minor_version)
src = test.c
all: $(myapi) mylib.so 

$(myapi): myapi.o
	gcc -shared -Wl,-soname,$(@:%.$(myapi_minor_version)=%) -o $@ $^  
	
myapi.o: myapi.c
	gcc -fPIC -o $@ -c $^

mylib.so: test_api.c
	gcc -shared -fPIC -o $@ $^
clean:
	rm -rf *.so* *.o
