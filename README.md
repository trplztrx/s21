
# Simple Bash Utils

## Описание

Проект представляет собой разработку утилит для работы с текстом в командной строке: `cat` и `grep`.

## Содержание

1. **Введение**:
   - Краткий обзор проекта и его целей.
   
2. **Часть 1: Работа с утилитой `cat`**:
   - Реализация утилиты `cat`, ее основные функции и примеры использования.

3. **Часть 2: Работа с утилитой `grep`**:
   - Реализация утилиты `grep`, ее основные функции и примеры использования.

4. **Часть 3: Дополнительные флаги утилиты `grep`**:
   - Описание реализации и использования дополнительных флагов утилиты `grep`.

5. **Часть 4: Комбинации флагов утилиты `grep`**:
   - Описание реализации и использования комбинаций флагов для утилиты `grep`.

## Установка

1. Склонируйте репозиторий:
   ```bash
   git clone <repository-url>
   ```
2. Перейдите в директорию проекта:
   ```bash
   cd SimpleBashUtils
   ```
3. Соберите проект с помощью `make`:
   ```bash
   make
   ```

## Использование

1. **`cat`**: Используйте утилиту для просмотра содержимого файлов.
   ```bash
   ./cat [options] [file...]
   ```

2. **`grep`**: Используйте утилиту для поиска текста в файлах.
   ```bash
   ./grep [options] pattern [file...]
   ```

## Тестирование

Для запуска тестов используйте команду:
```bash
make test
```
