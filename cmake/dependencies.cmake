# Dependencies
find_package(Threads REQUIRED)
find_package(OpenCV COMPONENTS opencv_core opencv_imgproc opencv_objdetect opencv_aruco opencv_videoio opencv_highgui REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(poph264 REQUIRED)

set(TCN_PLATFORM_DEPS)
set(TCN_TEST_DEPS)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # Enable Metal backend instead of OpenGL Later
  # see here: https://stackoverflow.com/questions/70632495/how-to-build-apples-metal-cpp-example-using-cmake

  find_library(FoundationLib Foundation REQUIRED)
  find_library(CoreFoundationLib CoreFoundation REQUIRED)
  find_package(opengl_system CONFIG REQUIRED)
  set(TCN_PLATFORM_DEPS ${TCN_PLATFORM_DEPS} ${FoundationLib} ${CoreFoundationLib} opengl::opengl)

endif()


# other platforms win/linux missing..
