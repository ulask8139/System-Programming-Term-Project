FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ cmake make pkg-config \
    libpqxx-dev libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -S . -B build && cmake --build build -j

CMD ["./build/sis_app"]
