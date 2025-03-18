# Library that holds ring buffer logic

I have created a small library that holds the logic for a ring buffer.
My focus have been on making it easy to use and it being compatible with STL features like ranges and algorithms.
I have chosen not to make the library thread safe, I consider that the user's responsibility (in the same way as std::vector does).
Performance: read and write are O(1) and iterating is O(N).

# Building
The build system is CMake. To enable tests add flag BUILD_TESTS=ON
