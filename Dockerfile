# Dockerfile
FROM debian:bullseye-slim

# Install AVR tools (avr-gcc, avrdude, etc.)
RUN apt-get update && apt-get install -y \
    gcc-avr \
    binutils-avr \
    avr-libc \
    avrdude \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set up the working directory
WORKDIR /app

# Copy source files into the container
COPY . /app

# Default entry point
CMD ["make"]