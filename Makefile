src = main.c
flags = -Wall -Wpedantic -Wextra -ggdb -O0
out = build/main

all: build main

main:
	gcc $(src) -o $(out) $(flags)

build:
	mkdir build

clean:
	rm -rf build
