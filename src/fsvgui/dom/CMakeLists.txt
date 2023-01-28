add_library(fsvguilib STATIC
    border.cpp
    box.cpp
    box.h
    box_helper.cpp
    box_helper.h
    clear_under.cpp
    color.cpp
    color.h
    composite_decorator.cpp
    dbox.cpp
    elements.h
    flex.cpp
    flexbox.cpp
    flexbox_config.cpp
    flexbox_config.h
    flexbox_helper.cpp
    flexbox_helper.h
    gridbox.cpp
    hbox.cpp
    node.cpp
    node.h
    node_decorator.cpp
    node_decorator.h
    paragraph.cpp
    rect.cpp
    rect.h
    reflect.cpp
    requirement.h
    screen.cpp
    screen.h
    size.cpp
    string.h
    take_any_args.h
    text.cpp
    util.cpp
    vbox.cpp
)

if(MSVC)
    target_compile_options(applib PUBLIC "/Zc:__cplusplus")
endif()

target_include_directories(fsvguilib
    PUBLIC
        ${HEADER_ROOT_DIR}
)

target_link_libraries(fsvguilib
	PRIVATE
        CONAN_PKG::freetype
        CONAN_PKG::boost

)