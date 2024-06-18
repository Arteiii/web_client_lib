set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIB_SOURCES
        src/webClient.cpp
)

add_library(webClient STATIC ${LIB_SOURCES})
target_include_directories(webClient PUBLIC lib/include)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CROSS_COMPILING TRUE)
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    set(WINAPI_INCLUDE_DIR "/usr/x86_64-w64-mingw32/include")

    target_include_directories(webClient PRIVATE ${WINAPI_INCLUDE_DIR})

    set_target_properties(webClient PROPERTIES
            LINK_FLAGS_RELEASE "-static -static-libgcc -static-libstdc++"
            LINK_FLAGS_DEBUG "-static -static-libgcc -static-libstdc++ -g"
    )

    # Link against Windows APIs
    target_link_libraries(webClient PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
            -lwinhttp
            -lwinmm
            -lgdi32
            -lcomdlg32
            -lkernel32
            -luser32
            -lshell32
            -lole32
            -loleaut32
            -luuid
            -lcomctl32
            -ladvapi32
    )
endif ()
