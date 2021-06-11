
set(SOURCES
    Convert.cpp
    Error.cpp
    Value.cpp
    Validator.cpp
    Argument.cpp
    Interpreter.cpp
    Printer.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)