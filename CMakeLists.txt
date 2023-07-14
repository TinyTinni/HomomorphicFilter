cmake_minimum_required(VERSION 3.10)

project(dft_filters LANGUAGES CXX)

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    "src/main.cpp"
    "src/homomorphic_filter.cpp"
    "src/ui/mainwindow.cpp"
    "src/ui/mainwindow.ui")

qt_add_executable(dft_filters ${SOURCES})
target_include_directories(dft_filters PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(dft_filters PUBLIC ${OpenCV_LIBS} Qt6::Core Qt6::Widgets)

install(TARGETS dft_filters
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET dft_filters
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})