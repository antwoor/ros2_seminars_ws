xhost +local:docker
docker run -it \
    --rm \
    --network host \
    --env DISPLAY=$DISPLAY \
    --env QT_X11_NO_MITSHM=1 \
    --gpus all
    --volume /tmp/.X11-unix:/tmp/.X11-unix \
    --volume $PWD:/workspace \
    stankin_humble