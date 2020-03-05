default: build
	./caster

build:
	g++ -o caster main.cpp -lncurses

run:
	./caster