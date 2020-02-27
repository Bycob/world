add_executable(rcc ${CMAKE_CURRENT_LIST_DIR}/RCCompiler.cpp)
set(RCLIB_DIR ${CMAKE_CURRENT_LIST_DIR})


function(rclib_add_resource_library RC_LIB_NAME RC_NAMESPACE)
    set(RC_HEADER_FILE "${RC_LIB_NAME}_resources.rc")
    set(RESOURCES_FILES ${ARGN})

    # Create RCLib files
    function(copy_rclib_file PATH DEST)
        file(READ ${PATH} CONTENT)
        string(REPLACE "%namespace%" ${RC_NAMESPACE} CONTENT "${CONTENT}")
        string(REPLACE "\"RCLib.h\"" "\"RCLib-${RC_LIB_NAME}.h\"" CONTENT "${CONTENT}")
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${DEST} "${CONTENT}")
    endfunction()

    copy_rclib_file(${RCLIB_DIR}/RCLib.h RCLib-${RC_LIB_NAME}.h)
    copy_rclib_file(${RCLIB_DIR}/RCLib.cpp RCLib-${RC_LIB_NAME}.cpp)

    # Create resource library
    add_library(${RC_LIB_NAME} STATIC ${CMAKE_CURRENT_BINARY_DIR}/RCLib-${RC_LIB_NAME}.cpp)
    set_target_properties(${RC_LIB_NAME} PROPERTIES build_dir "${CMAKE_CURRENT_BINARY_DIR}")
    target_compile_definitions(${RC_LIB_NAME} PRIVATE
            RCLIB_RESOURCES_HEADER="${RC_HEADER_FILE}")

    # Compile resources only at build time
    add_custom_target(${RC_LIB_NAME}_files DEPENDS ${RESOURCES_FILES})
    add_dependencies(${RC_LIB_NAME}_files rcc)

    add_custom_command(
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        TARGET ${RC_LIB_NAME}_files
        PRE_BUILD
        COMMAND $<TARGET_FILE:rcc>
        ARGS ${RC_HEADER_FILE} ${RESOURCES_FILES}
        DEPENDS ${RESOURCES_FILES}
        COMMENT "Building resource header ${RC_HEADER_FILE}"
        VERBATIM
    )

    add_dependencies(${RC_LIB_NAME} ${RC_LIB_NAME}_files)
endfunction()

function(rclib_include_directory TARGET)
    include_directories($<TARGET_PROPERTY:${TARGET},build_dir>)
endfunction()