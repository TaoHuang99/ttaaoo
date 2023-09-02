# ttaaoo&Song
service_migration
<!--
 * @Author: Tao
 * @Date: 2023-08-31 15:11:45
 * @LastEditors: Tao
 * @LastEditTime: 2023-09-01 16:37:58
 * @Description: 
 * @FilePath: \migration_new\readme.md
-->
文件结构包含bulid、include、lib、models、src等
主要迁移代码位于main.cc controllers/ src/migration-process/ 这些文件下
其中主函数为main.cc
Drogon监听的端口定义在controllers文件夹下的migration-register.cpp以及migration-register.h中，注册了两个post请求接口，分别为trriger以及cmd
src/migration-process/ 文件下的migration-process.cpp以及migration-process.h中定义以及实现行为树绑定的函数





有两种远程操作容器的方法，建议使用第二种方法
一、
主机docker 接口修改步骤：
1.	修改
Sudo vim /lib/systemd/system/docker.service
注释下面的并添加新的
 # ExecStart=/usr/bin/dockerd -H fd://
ExecStart=/usr/bin/dockerd -H tcp://0.0.0.0:2375 -H unix:///var/run/docker.sock
2.	重启服务
$ sudo systemctl daemon-reload
$ sudo systemctl restart docker.service
二、
或者本机创建python文件


from flask import Flask, jsonify
import docker

app = Flask(__name__)
client = docker.DockerClient(base_url='unix://var/run/docker.sock')

@app.route("/containers/<container_name>/stop", methods=['POST'])
def stop_container(container_name):
    try:
        container = client.containers.get(container_name)
        container.stop()
        return jsonify({'message': f'Container {container_name} stopped successfully!'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route("/containers/<container_name>/start", methods=['POST'])
def start_container(container_name):
    try:
        container = client.containers.get(container_name)
        container.start()
        return jsonify({'message': f'Container {container_name} started successfully!'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500
if __name__ == "__main__":
    app.run(host='0.0.0.0', port=2375)  # Listening on all interfaces on port 5000utocmd FileType python set expandtab
