# Detect the operating system
OS := $(shell uname)

.PHONY: all
all: setup format build

# Automatically select the correct setup target based on the OS
.PHONY: setup
setup:
ifeq ($(OS), Linux)
	$(MAKE) setup-debian
else ifeq ($(OS), Darwin)
	$(MAKE) setup-mac
else
	@echo "Unsupported OS: $(OS)"
endif

.PHONY: setup-debian
setup-debian:
	mkdir -p build && \
	cd build && \
	sudo apt-get update && \
	sudo apt-get install -y libsdl2-2.0-0 libsdl2-dev cmake make clang-format

.PHONY: setup-mac
setup-mac:
	which brew || /bin/bash -c "$$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" && \
	brew update || true && \
	brew install sdl2 cmake make clang-format && \
	export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$$PKG_CONFIG_PATH

.PHONY: format
format:
	clang-format src/* include/* -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	cmake --build .

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=Debug .. && \
	make

.PHONY: clean
clean:
	rm -rf build
