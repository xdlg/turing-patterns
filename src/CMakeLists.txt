add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE 
    main.cpp
    pattern_generator.cpp
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    SDL3::SDL3
)

target_compile_definitions(${PROJECT_NAME}
PUBLIC
    SDL_MAIN_USE_CALLBACKS
)
