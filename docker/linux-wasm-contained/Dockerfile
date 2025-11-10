FROM linux-wasm-base:dev

WORKDIR /linux-wasm

COPY patches patches
COPY runtime runtime
COPY linux-wasm.sh linux-wasm.sh

ENTRYPOINT ["/linux-wasm/linux-wasm.sh", "all"]
