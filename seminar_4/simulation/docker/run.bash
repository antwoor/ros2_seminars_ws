#!/bin/bash

# Written by Nikolay Dema <ndema2301@gmail.com>, September 2022

SIM_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

xhost +local:docker > /dev/null || true

IMG_NAME="stankin_humble"
GPU_FLAG=""

### Check if NVIDIA GPU flag is needed ----------------------------------- #

if [ -n "$(which nvidia-smi)" ] && [ -n "$(nvidia-smi)" ]; then
    GPU_FLAG=(--gpus all)
    IMG_NAME="stankin_humble"
else
    GPU_FLAG=(--gpus all)
    IMG_NAME="${IMG_NAME}"
fi


### DOCKER RUN ----------------------------------------------------------- #

docker run  ${GPU_FLAG[@]} \
            -d -ti --rm \
            -e "DISPLAY" \
            -e "QT_X11_NO_MITSHM=1" \
            -e XAUTHORITY \
            -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
            -v /etc/localtime:/etc/localtime:ro \
            -v ${SIM_ROOT}/workspace:/workspace \
            --net=host \
            --privileged \
            --name "seminar_4" ${IMG_NAME} \
            > /dev/null
