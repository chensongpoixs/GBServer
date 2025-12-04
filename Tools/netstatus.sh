# 1. nload – 简单直观的带宽监控工具
安装：
bash
复制
sudo apt update
sudo apt install nload
使用：
bash
复制
nload
方向键切换网卡，按 q 退出。
✅ 2. iftop – 查看每个连接的带宽使用
安装：
bash
复制
sudo apt install iftop
使用（需 root 权限）：
bash
复制
sudo iftop
显示每个 IP/端口的实时流量，按 q 退出。
✅ 3. bmon – 图形化+命令行混合带宽监控
安装：
bash
复制
sudo apt install bmon
使用：
bash
复制
bmon
✅ 4. vnstat – 长期统计带宽使用量（非实时）
安装：
bash
复制
sudo apt install vnstat
启动服务：
bash
复制
sudo systemctl enable vnstat
sudo systemctl start vnstat
查看统计：
bash
复制
vnstat
✅ 5. ip -s link – 系统自带，快速查看网卡流量
bash
复制
ip -s link
显示每个接口的收发包和字节数（累计值，非实时）。
✅ 推荐组合：
实时监控：nload 或 iftop
长期统计：vnstat
快速查看：ip -s link