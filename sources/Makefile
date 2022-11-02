include ../makeup.mk

SOURCES=\
    Convert.cpp \
    Error.cpp \
    Value.cpp \
    Validator.cpp \
    Argument.cpp \
    Interpreter.cpp \
    Printer.cpp

$(call include_directories,../headers)

$(call add_library,${PROJECT_NAME},STATIC SHARED,${SOURCES})

$(call install_targets,${PROJECT_NAME},${ROOT_INSTALL_DIR})
