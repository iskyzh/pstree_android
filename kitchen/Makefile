all: kitchen

kitchen: kitchen.c
	gcc $< -o $@ -lpthread -Wall

run: kitchen
	./$<

clean:
	rm kitchen
