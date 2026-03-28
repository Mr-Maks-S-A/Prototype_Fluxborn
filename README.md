# Команда для сборки и выгрузки в твою папку
# Включаем BuildKit (если не включен)
export DOCKER_BUILDKIT=1

# Собираем и выгружаем результат в локальную папку ./bin
docker build --output type=local,dest=./bin .


# Создаем папку для экспорта, если её нет
mkdir -p bin

# Запуск сборки с использованием BuildKit для экспорта файлов напрямую в систему
DOCKER_BUILDKIT=1 docker build --output type=local,dest=./bin .
