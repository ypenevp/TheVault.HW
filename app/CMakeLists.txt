cmake_minimum_required(VERSION 4.3)

project(TheVaultHW VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


file(GLOB SOURCES "src/*.cpp")

add_executable(vault_hw ${SOURCES})

target_include_directories(vault_hw PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(vault_hw PRIVATE -Wall -Wextra)
endif()

add_custom_target(run
    COMMAND vault_hw
    DEPENDS vault_hw
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Starting TheVault.HW..."
)