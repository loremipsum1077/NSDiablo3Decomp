cmake_minimum_required(VERSION 3.13)
project(d3_desktop_subsystems LANGUAGES C)

# This host-only project is independent of the Switch matching build at the root.
set(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(LUA_SOURCE "${REPO_ROOT}/src/Contrib/Contrib/lua/src")

enable_testing()

function(add_lua_behavior_test target fixture)
    add_executable(${target} "${REPO_ROOT}/${fixture}" ${ARGN})
    target_include_directories(${target} PRIVATE "${REPO_ROOT}/src")
    set_target_properties(${target} PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)
    # The existing fixtures use assert for their checks, including calls under test.
    if(MSVC)
        target_compile_options(${target} PRIVATE /UNDEBUG)
    else()
        target_compile_options(${target} PRIVATE -UNDEBUG)
    endif()
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_lua_behavior_test(lua_object_behavior tests/lua_runtime/object_test.c
    "${LUA_SOURCE}/lobject.c")
add_lua_behavior_test(lua_allocation_behavior tests/lua/objects_test.c
    "${LUA_SOURCE}/lfunc.c" "${LUA_SOURCE}/lstring.c")
