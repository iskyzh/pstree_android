LOG_PATH = ../burger_buddies/result/log.txt

test: test.cpp
	g++ $< -o $@ -std=c++11

run: test
	./$< $(LOG_PATH)

clean:
	rm test

.PHONY: clean
