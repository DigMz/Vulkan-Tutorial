test_debug:
	${MAKE} compile
	${MAKE} run

test_release:
	${MAKE} compile
	${MAKE} run

conf_debug:
	cmake -B build -G Ninja \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_CPP20_MODULE=OFF \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

conf_release:
	cmake -B build -G Ninja \
		-DCMAKE_BUILD_TYPE=Release \
		-DENABLE_CPP20_MODULE=OFF \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

compile:
	cmake --build build

run:
	./build/VulkanTutorial/VulkanTutorial

clean:
	rm -rf build
