include ../makeup.mk

$(call import_modules,Boost)

$(call find_boost,LIBS:unit_test_framework PATH:${HOME})

ifeq (${BOOST_LIBRARIES},)
$(warning boost unit test framework is not found - tests are not available)
else

$(call include_directories,${ROOT_SOURCE_DIR}/headers ${BOOST_INCLUDE_DIR})
$(call link_directories,${ROOT_BINARY_DIR}/sources ${BOOST_LIBRARY_DIR})
$(call link_libraries,${PROJECT_NAME} ${BOOST_LIBRARIES})

SOURCES=\
	ErrorTests.cpp \
	ValueTests.cpp \
	ValidatorTests.cpp \
	ArgumentTests.cpp \
	InterpreterTests.cpp \
	PrinterTests.cpp \
	tests.cpp

$(call add_program,${PROJECT_NAME}-test,${SOURCES})

$(call add_dependencies,${PROJECT_NAME}-test,../sources/lib${PROJECT_NAME}.a)

$(call add_test_program,unit-tests,${PROJECT_NAME}-test)

VALGRIND=$(call find_program,valgrind)
ifneq (${VALGRIND},)
$(call add_test_command,leak-tests,${VALGRIND} --leak-check=summary --error-exitcode=1 ./${PROJECT_NAME}-test)
endif

endif
