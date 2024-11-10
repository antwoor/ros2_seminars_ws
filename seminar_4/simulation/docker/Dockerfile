# Или :general, если у кого-то стоит не nvidia
FROM nickodema/hsl_2022:nvidia  

# Обновление списка пакетов и установка необходимых пакетов
RUN sudo apt-get update &&\
    sudo apt-get -y install libzbar0 &&\
    apt install -y python3-pip --fix-missing &&\
    pip3 install opencv-python pyzbar 
    

WORKDIR /workspace

RUN sudo apt-get update && \
    sudo apt-get install -y curl gnupg2 lsb-release && \
    curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add - && \
    echo "deb http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/ros2-latest.list && \
    sudo apt-get update && \
    sudo apt-get install -y ros-humble-navigation2 ros-humble-nav2-bringup && \
    sudo apt-get install -y python3-colcon-common-extensions 

RUN sudo apt-get update && \
    sudo apt-get install -y ros-humble-rmw-cyclonedds-cpp && \
    sudo apt-get install -y build-essential cmake git

# Очистка кеша apt для уменьшения размера образа
RUN sudo apt-get clean && rm -rf /var/lib/apt/lists/*