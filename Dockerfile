# Build
FROM gcc:13-bookworm AS builder

WORKDIR /build

COPY . .

RUN make

# Runtime
FROM debian:bookworm-slim

WORKDIR /app

COPY --from=builder /build/hiveParse .

# Mount for log data
VOLUME ["/app/log_data"]

ENTRYPOINT ["./hiveParse"]
