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

add_executable(dft_filters ${SOURCES})
target_include_directories(dft_filters PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(dft_filters PUBLIC ${OpenCV_LIBS} Qt6::Core Qt6::Widgets)
