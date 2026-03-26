src = main.c
flags = -Wall -Wpedantic -Wextra -ggdb -O0
out = build/main

all: build main

main:
	gcc $(src) -o $(out) $(flags) -Iraylib-5.5_linux_amd64/include/ -Lraylib-5.5_linux_amd64/lib/ -l:libraylib.a -lm

build:
	mkdir build

clean:
	rm -rf build
