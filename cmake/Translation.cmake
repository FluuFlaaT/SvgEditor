# Qt translation support

function(add_translation_targets out_qm_files)
    find_package(Qt5 COMPONENTS LinguistTools REQUIRED)
    
    file(GLOB TS_FILES ${CMAKE_SOURCE_DIR}/translations/*.ts)
    
    # Generate/update TS files using lupdate
    add_custom_target(lupdate
        COMMAND Qt5::lupdate
            ${CMAKE_SOURCE_DIR}/src
            -ts ${TS_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Updating translation files"
    )
    
    # Generate QM files using lrelease
    qt5_add_translation(QM_FILES ${TS_FILES})
    
    # Copy QM files to binary directory
    foreach(QM_FILE ${QM_FILES})
        get_filename_component(QM_FILENAME ${QM_FILE} NAME)
        add_custom_command(
            OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/${QM_FILENAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations
            COMMAND ${CMAKE_COMMAND} -E copy ${QM_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/${QM_FILENAME}
            DEPENDS ${QM_FILE}
        )
    endforeach()
    
    # Create custom target for QM files
    add_custom_target(translations ALL
        DEPENDS ${QM_FILES} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations
        COMMENT "Generating QM files and copying to output directory"
    )
    
    # Return generated QM files
    set(${out_qm_files} ${QM_FILES} PARENT_SCOPE)
endfunction()