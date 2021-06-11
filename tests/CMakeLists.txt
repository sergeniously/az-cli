
find_package(Boost COMPONENTS unit_test_framework)

if(Boost_FOUND)
    find_program(VALGRIND valgrind)

    add_executable(${PROJECT_NAME}-test
        ErrorTests.cpp
        ValueTests.cpp
        ValidatorTests.cpp
        ArgumentTests.cpp
        InterpreterTests.cpp
        PrinterTests.cpp
        tests.cpp
    )
    #add_dependencies(${PROJECT_NAME}-test ${PROJECT_NAME})

    target_link_libraries(${PROJECT_NAME}-test
        ${PROJECT_NAME} ${Boost_LIBRARIES}
    )

    add_test(NAME unit-tests COMMAND ${PROJECT_NAME}-test)
    if(VALGRIND)
        add_test(NAME leak-tests COMMAND valgrind --leak-check=summary --error-exitcode=1 ./${PROJECT_NAME}-test)
    endif()
else()
    message(STATUS
        "Boost unit test framework is not found; testing targets will not be available")
endif()
