cmake_minimum_required(VERSION 4.0)
project(satisfying_ball_simulation)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ${CMAKE_SOURCE_DIR}/include

)

link_libraries(
        ${CMAKE_SOURCE_DIR}/lib/SDL3.lib
        ${CMAKE_SOURCE_DIR}/lib/SDL3_gfx_Static.lib
)


file(GLOB PROJECT_SOURCES
        src/*.cpp
)

set(IMGUI_SOURCES
        src/imgui/imgui.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_impl_sdl3.cpp
        src/imgui/imgui_impl_sdlrenderer3.cpp
        src/ParamsGUI.h
        src/EventHadler.h
        src/Time.h
)

add_executable(satisfying_ball_simulation ${PROJECT_SOURCES} ${IMGUI_SOURCES})


set(DLL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/bin)
if (EXISTS ${DLL_SOURCE_DIR})
    file(GLOB DLL_FILES "${DLL_SOURCE_DIR}/*.dll")
    foreach (DLL_FILE ${DLL_FILES})
        add_custom_command(
                TARGET satisfying_ball_simulation POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DLL_FILE}"
                "$<TARGET_FILE_DIR:satisfying_ball_simulation>"
                COMMENT "Copying DLL: ${DLL_FILE}"
        )
    endforeach ()
endif ()
