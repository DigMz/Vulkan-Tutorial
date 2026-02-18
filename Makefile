CFLAGS = -std=c++17 -O2
CFLAGS_DEBUG = -std=c++17 -O2 -UNDEBUG
CFLAGS_RELEASE = -std=c++17 -O2 -DNDEBUG
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanTest: main.cpp
	g++ $(CFLAGS) -o VulkanTest main.cpp $(LDFLAGS)

.PHONY: test debug release clean

test: VulkanTest
	./VulkanTest

debug:
	$(MAKE) clean
	$(MAKE) VulkanTest CFLAGS="$(CFLAGS_DEBUG)"

release:
	$(MAKE) clean
	$(MAKE) VulkanTest CFLAGS="$(CFLAGS_RELEASE)"

clean:
	rm -f VulkanTest

