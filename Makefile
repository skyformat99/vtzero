
# Whether to turn compiler warnings into errors
export WERROR ?= true

default: release

release:
	mkdir -p build && cd build && cmake ../ -DCMAKE_BUILD_TYPE=Release -DWERROR=$(WERROR) && VERBOSE=1 cmake --build .

debug:
	mkdir -p build && cd build && cmake ../ -DCMAKE_BUILD_TYPE=Debug -DWERROR=$(WERROR) && VERBOSE=1 cmake --build .

test:
	@if [ -f ./build/unit-tests ]; then ./build/unit-tests; else echo "Please run 'make release' or 'make debug' first" && exit 1; fi

bench:
	@if [ -f ./build/bench-tests ]; then ./build/bench-tests; else echo "Please run 'make release' or 'make debug' first" && exit 1; fi

tidy:
	./scripts/clang-tidy.sh

coverage:
	./scripts/coverage.sh

clean:
	rm -rf build
	rm *.profdata
	rm *.profraw
	rm local.env

distclean: clean
	rm -rf .mason
	rm -rf mason_packages
	rm -rf .toolchain

format:
	./scripts/format.sh

.PHONY: test bench