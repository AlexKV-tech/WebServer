# ---------- builder stage ----------
FROM debian:stable-slim AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates pkg-config wget \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy CMakeLists and sources
COPY . /app

# Create build directory, configure and build
RUN mkdir -p build \
 && cd build \
 && cmake .. -DCMAKE_BUILD_TYPE=Release \
 && cmake --build . -- -j$(nproc)

# ---------- runtime stage ----------
FROM debian:stable-slim

# Create a non-root user for safety
RUN apt-get update \
 && apt-get install -y --no-install-recommends ca-certificates \
 && rm -rf /var/lib/apt/lists/* \
 && useradd --no-create-home --shell /bin/false appuser

WORKDIR /app

# Copy binary(s) & static files from builder to runtime image
# Adjust 'webserver' and 'static' paths according to your build layout
COPY --from=builder /app/build/webserver /app/webserver
# If you have a 'static' folder, copy it
COPY --from=builder /app/static /app/static

# Expose port your server uses
EXPOSE 7777

# Run as non-root
USER appuser

# Healthcheck (optional) — basic TCP test via bash
HEALTHCHECK --interval=30s --timeout=3s --start-period=10s --retries=1 \
  CMD bash -c "cat < /dev/null > /dev/tcp/127.0.0.1/7777 2>/dev/null || exit 1"

# Default command
ENTRYPOINT ["/app/webserver"]
CMD ["/app/static/index.html"]
