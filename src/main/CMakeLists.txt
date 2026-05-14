add_executable(infocell main.cc)

target_link_libraries(infocell
PRIVATE
    applib
)

add_executable(rsvg-convert rsvg-convert.cc)
