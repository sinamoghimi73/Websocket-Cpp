.PHONY: all
all: clean build format debug

.PHONY: format
format:
	clang-format src/*.cpp include/*.hpp -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl .. && \
	make

.PHONY: clean
clean:
	rm -rf build

.PHONY: style
style:
	clang-format -style=google -dump-config > .clang-format

.PHONY: run
run:
	cd build && ./UniverWebSocket && cd ..