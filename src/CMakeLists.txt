# poph264-test target instructions

#####################################
# Define Target Information
#####################################
set(TARGET_NAME poph264_test)
set(TARGET_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TARGET_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/poph264_test.cpp)

#####################################
# Support IDE Visualization
#####################################
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_SOURCES})

#####################################
# Define Target according to options
#####################################
add_executable(${TARGET_NAME} ${TARGET_SOURCES})

#####################################
# Define Target Properties and Optional OpenSSL Dependency
#####################################

# add target properties such as debug postfix and MSVC runtime
set_target_properties(${TARGET_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "${MSVC_RUNTIME_TYPE}")

# set target compile options as defined in the cmake/compiler_options.cmake Module
target_compile_options(${TARGET_NAME} PRIVATE ${compiler_options})

# add compiler definition WITH_OPENSSL, if found
# target_compile_definitions(${TARGET_NAME} PRIVATE ${compiler_definitions} $<$<BOOL:${OPENSSL_FOUND}>:WITH_OPENSSL>)
target_compile_definitions(${TARGET_NAME} PRIVATE ${compiler_definitions})

# set target link options as defined in the cmake/compiler_options.cmake Module
target_link_options(${TARGET_NAME} PRIVATE ${linker_flags})

# include all the necessary directories
target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/exports>
  PRIVATE 
    ${TARGET_INCLUDE_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # Any other extra include required
)

# link
target_link_libraries(${TARGET_NAME} PUBLIC
        opencv_core
        opencv_imgproc
        opencv_objdetect
        opencv_aruco
        opencv_videoio
        opencv_highgui
        rapidjson
        poph264::poph264
)
