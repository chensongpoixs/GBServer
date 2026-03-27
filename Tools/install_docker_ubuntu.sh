#!/bin/bash

#步骤 1: 拉取 Ubuntu 22.04 镜像
docker pull ubuntu:22.04


#步骤 2: 运行 Ubuntu 22.04 容器
docker pull ubuntu:22.04

#步骤 3: 进入容器
docker exec -it <container_id_or_name> bash

# docker run -it ubuntu:22.04 /bin/bash

#步骤 4: 更新和升级系统（可选）
apt update && apt upgrade -y

#步骤 5: 退出容器

exit
