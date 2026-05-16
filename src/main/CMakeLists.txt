add_executable(infocell-tui main.cc)

target_link_libraries(infocell-tui
PRIVATE
    tuilib
)

add_executable(rsvg-convert rsvg-convert.cc)
