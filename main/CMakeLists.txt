add_executable(cells main.cc)
target_link_libraries(cells
	PRIVATE
		cellslib
)
target_include_directories(cells
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/src
)