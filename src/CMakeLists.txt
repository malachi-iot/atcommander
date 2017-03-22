# not ready for primetime just yet
# want this to be a convenience include, I guess like a package

INCLUDE_DIRECTORIES(.)

file(GLOB atcommander_source "*.cpp" "*.h"
    "CMakeLists.txt")

message("Debug2: ${atcommander_source}")
add_library(lib_atcommander ${atcommander_source})
