config:
	mkdir -p build
	cd build && cmake ..

build: config
	cd build && cmake --build . -j${nproc}
clean:
	rm -rf build