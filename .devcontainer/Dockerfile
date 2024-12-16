FROM stepanove/qt_ci:6.8.0

# Install additional dependencies
RUN sudo apt update && \
    sudo apt install -y protobuf-compiler protobuf-compiler-grpc libgrpc++-dev && \
    sudo rm -rf /var/lib/apt/lists/*
