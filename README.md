# Configuration Manager через D-Bus
Этот проект реализует систему удалённого управления конфигурацией приложений с использованием D-Bus (системы межпроцессного взаимодействия).

## Что такое D-Bus и зачем он нужен?
D-Bus — это система обмена сообщениями между процессами в Linux, позволяющая приложениям взаимодействовать друг с другом. Это один из способов IPC (Inter Process Communications) Она используется для:

- **Управления системными сервисами (например, NetworkManager, PulseAudio)**

- **Удалённой настройки параметров приложений**

- **Отправки и приёма событий между программами**

В этом проекте D-Bus используется для динамического изменения конфигурации приложения без его перезапуска.

## Технические требования
- **Стандарт: C++20**

- **ОС: Unix-подобные системы (тестировалось на Ubuntu 22.04 / 24.04)**

- **Необходимые библиотеки:**

    - **sdbus-c++**
    - **nlohmann/json** 
    - **Doxygen**
    - **GTest**
## Установка зависимостей
### 1. Автоматическая установка (рекомендуется) 
В корне проекта реализован bash-script для автоматической загрузки всех необходимых библиотек. 
```bash
    chmod +x setup_project.sh
    ./setup_project.sh
```
### 2. Ручная установка
```bash
    sudo apt update
    sudo apt install -y libdbus-1-dev libsd-bus-dev nlohmann-json3-dev doxygen g++ cmake
    sudo apt install -y libgtest-dev googletest
```

## Запуск и пример использования
После сборки проекта, в папке build будут лежать необходимые бинарники
### 1. Запуск сервера
```bash
    ./DialogueServer/DialogueServer
```
Что делает сервер?
    - **Загружает конфигурации из ~/.config/com.system.configurationManager/**
    - **Предоставляет D-Bus API для изменения настроек**

### 2. Запуск клиента
```bash
    ./DialogueClient/DialogueClient
```
Что делает клиент?
    - **Загружает конфигурацию из confManagerApplication1.json**
    - **Выводит сообщение (TimeoutPhrase) каждые Timeout миллисекунд**
    - **Подписывается на изменения через D-Bus**

### 3. Изменение конфигурации через D-Bus
```bash
    gdbus call --session \
    -d com.system.configurationManager \
    -o /com/system/configurationManager/Application/confManagerApplication1 \
    -m com.system.configurationManager.Application.Configuration.ChangeConfiguration \
    "TimeoutPhrase" "<'Новое сообщение'>"
```
Клиент мгновенно обновит текст и начнёт выводить новую фразу. 

## Тесты
Были также написаны тесты, которые лежат в папки Tests. В случае необходимости, их можно запустить:
```bash
    ./Tests/Tests
```