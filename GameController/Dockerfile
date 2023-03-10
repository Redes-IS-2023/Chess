# Base image
FROM alpine:latest

# Install build dependencies
RUN apk update && apk add build-base

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY . .

# Compile the program
RUN gcc -o gamecontroller gamecontroller.c

# Set the entrypoint for the container
ENTRYPOINT [ "./gamecontroller" ]
