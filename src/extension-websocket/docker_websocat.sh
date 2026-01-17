#!/bin/bash

docker run \
    -it \
    --rm \
    --platform linux/amd64 \
    --add-host=host.docker.internal:host-gateway \
    solsson/websocat ws://host.docker.internal:9001
