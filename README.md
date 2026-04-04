# file-chat

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/CMake-3.31+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

**Консольное приложение для обмена сообщениями и файлами по сети.**

## Описание

`file-chat` — это лёгкое и быстрое консольное приложение на современном **C++23**, предназначенное для чата и передачи
файлов между компьютерами.

Приложение использует:

- собственный протокол обмена
- прогресс-бар загрузки файлов
- красивый ANSI-вывод в терминале (через библиотеку `ansi-console`)
- сетевую библиотеку `net`

Подходит для локальной сети или тестирования сетевого взаимодействия.

## Возможности

- ✅ Обмен текстовыми сообщениями в реальном времени
- ✅ Передача файлов любого размера с индикатором прогресса
- ✅ Удобные сессии чата (`chat-session`)
- ✅ Современный C++23 + строгие предупреждения компилятора
- ✅ Кросс-платформенная сборка через CMake
- ✅ Чистая архитектура (разделение на `file-utils`, `protocol-utils`, `progress-display`)

## Как собрать и запустить

### 1. Клонирование репозитория

```bash
git clone https://github.com/VadmanBat/file-chat.git
cd file-chat
```

### 2. Сборка (рекомендуемый способ)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 3. Запуск

```bash
# В папке build
./file-chat
```

> **Примечание:** проект зависит от библиотек `ansi-console` и `net`, которые подключаются через `add_subdirectory`.
> Убедись, что они находятся в правильных относительных путях при сборке.

## Структура проекта

```
file-chat/
├── src/
│   ├── main.cpp
│   ├── chat-session/
│   ├── file-utils.cpp
│   ├── protocol-utils.cpp
│   └── progress-display.cpp
├── include/          # заголовочные файлы
├── resources/        # ресурсы (иконки, rc-файл и т.д.)
├── CMakeLists.txt
├── .clang-format
└── .gitignore
```

## Требования

- **Компилятор:** поддерживающий C++23 (MSVC 2022+, GCC 14+, Clang 18+)
- **CMake** 3.31 или новее
- Библиотеки `ansi-console` и `net` (внешние для этого репозитория)

---

**Автор:** VadmanBat  
**Дата создания:** 2026  
**Лицензия:** MIT