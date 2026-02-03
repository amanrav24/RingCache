# ---- Stage 1: Build ----
FROM ubuntu:24.04 AS builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    clang \
    build-essential \
    libstdc++6 \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
WORKDIR /app
COPY . .

RUN pwd
RUN ls -la

# Build using your Makefile
RUN make startNode

# ---- Stage 2: Runtime ----
FROM ubuntu:24.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy the compiled binary from builder
COPY --from=builder /app/bin/startNode /usr/local/bin/startNode

# Set working dir
WORKDIR /usr/local/bin

# Default command (can be overridden)
CMD ["./startNode"]


