#!/bin/bash
export GOPATH=/home/chensong/dep/go/go_workspace

export GOROOT=/home/chensong/dep/go

export GOPROXY=https://goproxy.cn

export GO111MODULE=on

export PATH=$PATH:$GOROOT/bin:$GOPATH/bin


#go build -o signaling src/*.go
go run  src/*.go
