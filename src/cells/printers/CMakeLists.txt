add_library(cells-printers STATIC
    StructPrinter.cc
    StructPrinter.h
    ValuePrinter.cc
    ValuePrinter.h
)

target_link_libraries(cells-printers
PUBLIC
    cells-lib
)