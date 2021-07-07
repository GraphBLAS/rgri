file(GLOB_RECURSE SRC "*")

add_library (RGRI ${SRC})
target_include_directories(RGRI PUBLIC .)

set_target_properties(RGRI PROPERTIES LINKER_LANGUAGE CXX)

# Should install library properly here, but I'm focusing on just the docs
