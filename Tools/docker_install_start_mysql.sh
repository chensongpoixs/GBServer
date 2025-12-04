#!/bin/bash


sudo docker run --name mysql-container -d \
  -p 3306:3306 \
  -v /host/data:/var/lib/mysql \
  -v /host/conf:/etc/mysql/conf.d \
  -e MYSQL_ROOT_PASSWORD=chensong \
  mysql