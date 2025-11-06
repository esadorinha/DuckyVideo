cmake_minimum_required(VERSION 3.10)
project(DuckyVideo)

# Especificar a versão do C++
set(CMAKE_CXX_STANDARD 17)

# Encontrar pacotes OpenCV e Qt
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Charts)

# Incluir diretórios com headers
include_directories(${OpenCV_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

# Adicionar os arquivos fonte do projeto
add_executable(DuckyVideo main.cpp VideoManager.cpp)

# Linkar as bibliotecas OpenCV e Qt
target_link_libraries(DuckyVideo ${OpenCV_LIBS} Qt5::Widgets Qt5::Charts)

# Para incluir PThreads
find_package(Threads REQUIRED)
target_link_libraries(DuckyVideo ${OpenCV_LIBS} Threads::Threads)
