cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(example)
set(
        xtl_DIR
        /Users/debrajray/MyComputer/RTS/xtl/build
        CACHE STRING "" FORCE
)
set(
        xtensor_DIR
        /Users/debrajray/MyComputer/RTS/xtensor/build
        CACHE STRING "" FORCE
)
set(
        xsimd_DIR
        /Users/debrajray/MyComputer/RTS/xsimd/build
        CACHE STRING "" FORCE
)

set (CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")

set(Torch_DIR "/Users/debrajray/MyComputer/libtorch-latest-1.12/libtorch/share/cmake/Torch")

find_package(Torch REQUIRED)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

find_package(xsimd REQUIRED)
find_package(OpenMP REQUIRED)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
find_package(xtensor REQUIRED PATHS /Users/debrajray/MyComputer/RTS/xtensor/build)
find_package(SFML COMPONENTS
        system window graphics network audio REQUIRED PATHS /Users/debrajray/MyComputer/softwares/SFML/build)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR} "./" ${SFML_INCLUDE_DIR})

add_definitions(-DXTENSOR_ENABLE_XSIMD)
add_definitions(-DXTENSOR_USE_OPENMP)

add_executable(example example.cpp fixedobstacles.h fixedobstacles.cpp gameConstants.h enemy/enemy.h enemy/enemy.cpp gameSimulation.cpp gameSimulation.h findPath.cpp findPath.h player.cpp player.h state.h state.cpp rl_control.cpp rl_control.h observation.h coordinatesUtil.cpp coordinatesUtil.h
        plot/pbPlots.hpp plot/pbPlots.cpp plot/supportLib.hpp plot/supportLib.cpp objectLocator.cpp objectLocator.h observation.cpp AStar_.cpp AStar_.h AStarOpenList.cpp AStarOpenList.h node_.cpp node_.h TestResult.h DQN/DQN_interface.h DQN/ReplayMemory.cpp DQN/ReplayMemory.h
        #DQN/dueling-DQN/DQNNet.cpp DQN/dueling-DQN/DQNNet.h DQN/dueling-DQN/RLNN_Agent.cpp DQN/dueling-DQN/RLNN_Agent.h
        #DQN/DDQN/DQNNet.cpp DQN/DDQN/DQNNet.h DQN/DDQN/RLNN_Agent.cpp DQN/DDQN/RLNN_Agent.h
        DQN/Vanilla-DQN/DQNNet.cpp DQN/Vanilla-DQN/DQNNet.h DQN/Vanilla-DQN/RLNN_Agent.cpp DQN/Vanilla-DQN/RLNN_Agent.h
        #DQN/dueling-DQN-bounded/DQNNet.cpp DQN/dueling-DQN-bounded/DQNNet.h DQN/dueling-DQN-bounded/RLNN_Agent.cpp DQN/dueling-DQN-bounded/RLNN_Agent.h
        Logger.cpp Logger.h trainingMaps.cpp trainingMaps.h testing.h FOV_CNN/CNN_controller.cpp FOV_CNN/CNN_controller.h FOV_CNN/FOV_Filters.cpp FOV_CNN/FOV_Filters.h dist/json/json.h dist/json/json-forwards.h dist/jsoncpp.cpp JsonParser.cpp JsonParser.h
        UI/SimpleUIView.cpp UI/SimpleUIView.h GameController.cpp GameController.h UI/TileMap.h enemy/enemyUIData.h)

target_include_directories(example PUBLIC ${xtensor_INCLUDE_DIRS})
target_link_libraries(example "${TORCH_LIBRARIES}" xtensor ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${OPENGL_LIBRARY} OpenMP::OpenMP_CXX sfml-graphics)

set_property(TARGET example PROPERTY CXX_STANDARD 14)
