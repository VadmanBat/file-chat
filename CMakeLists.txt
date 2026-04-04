cmake_minimum_required(VERSION 3.31)
project(file-chat VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------------------------
# Основные настройки стандарта и сборки
# ------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)       # Выключаем компилятор-специфичные расширения

option(STATIC_LINKING "Статическая линковка libstdc++/libgcc и runtime на MSVC" ON)

# Принудительно собираем все внешние библиотеки как статические (если их CMakeLists.txt это поддерживает)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# ------------------------------------------------------------------
# Зависимости
# ------------------------------------------------------------------
add_subdirectory(../ansi-console ansi-console-build)
add_subdirectory(../../classes/net net-build)

# ------------------------------------------------------------------
# Исполняемый файл
# ------------------------------------------------------------------
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/chat-session/chat-session.cpp
        src/chat-session/handle-send.cpp
        src/chat-session/run.cpp
        src/progress-display.cpp
        src/protocol-utils.cpp
        src/file-utils.cpp
        resources/resources.rc
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE ansi net)

# ------------------------------------------------------------------
# Общие предупреждения (для всех конфигураций)
# ------------------------------------------------------------------
target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wshadow -Wnon-virtual-dtor -Wold-style-cast
        -Wcast-align -Wunused -Woverloaded-virtual
        -Wconversion -Wsign-conversion -Wnull-dereference
        -Wdouble-promotion -Wformat=2
        -Wimplicit-fallthrough
)

# ------------------------------------------------------------------
# Опции только для Release
# ------------------------------------------------------------------
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
        -O3                 # Максимальная скорость
        #-march=native      # Генерирует код под текущий процессор (использует все доступные инструкции CPU)
        -DNDEBUG            # Отключает assertions (assert()) в релизе
        -ffunction-sections # Помещает каждую функцию в отдельную секцию (позволяет линковщику удалять неиспользуемые)
        -fdata-sections     # Помещает каждые статические данные в отдельную секцию (аналогично функциям)
        -fno-exceptions     # Отключает поддержку исключений C++ (уменьшает размер и повышает производительность)
        -fno-rtti           # Отключает Run-Time Type Information (dynamic_cast, typeid) — экономит место и ускоряет
        >
)

target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
        -flto               # Link-Time Optimization — оптимизация на этапе линковки (может сильно уменьшить размер и ускорить)
        -Wl,--gc-sections   # Удаляет неиспользуемые секции (работает вместе с -ffunction-sections и -fdata-sections)
        -Wl,--strip-all     # Удаляет всю отладочную информацию и символы из исполняемого файла (уменьшает размер)
        >
)

# ------------------------------------------------------------------
# Опции только для Debug
# ------------------------------------------------------------------
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
        -O0
        -g
        >
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" AND NOT MINGW)
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            >
    )
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
            -fsanitize=address,undefined
            >
    )
endif ()

# ------------------------------------------------------------------
# Статическая линковка (runtime и стандартная библиотека)
# ------------------------------------------------------------------
if (STATIC_LINKING)
    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE
                /MT$<$<CONFIG:Debug>:d>
        )
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_link_options(${PROJECT_NAME} PRIVATE
                -static-libstdc++
                -static-libgcc
        )
    endif ()

    if (MINGW)
        target_link_options(${PROJECT_NAME} PRIVATE
                -static
        )
    endif ()
endif ()

# ------------------------------------------------------------------
# Копирование иконки в директорию сборки (Windows)
# ------------------------------------------------------------------
if (WIN32)
    configure_file(
            ${CMAKE_SOURCE_DIR}/resources/icon.ico
            ${CMAKE_CURRENT_BINARY_DIR}/icon.ico
            COPYONLY
    )
endif ()