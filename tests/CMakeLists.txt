if (${NOTEST})
    message(STATUS "Skipping test targets")
else ()
    #
    # PARAMETERS
    #
    set(project acquire-driver-common) # CMAKE_PROJECT_NAME gets overridden if this is a subtree of another project
    set(driver acquire-driver-common)

    #
    # Copy driver to tests
    #
    add_custom_target(${project}-copy-for-tests
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${driver}>
            ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${driver}
            COMMENT "Copying ${driver} to ${CMAKE_CURRENT_BINARY_DIR}"
            )

    #
    # Tests
    #
    foreach (name
            unit-tests
            can-load-driver-interface
            )
        set(tgt ${project}-${name})
        add_executable(${tgt} ${name}.cpp)
        target_compile_definitions(${tgt} PUBLIC "TEST=\"${tgt}\"")
        set_target_properties(${tgt} PROPERTIES
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
                )
        target_include_directories(${tgt} PRIVATE "../src/simcams")
        target_link_libraries(${tgt}
                acquire-core-platform
                acquire-core-logger
                )
        add_dependencies(${tgt} ${project}-copy-for-tests)

        add_test(NAME test-${tgt} COMMAND ${tgt})
        set_tests_properties(test-${tgt} PROPERTIES LABELS anyplatform)
    endforeach ()

endif ()
