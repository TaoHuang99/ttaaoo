#include "dummy_nodes.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <etcd/Client.hpp>
#include <etcd/Response.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "cpr/cpr.h"
#include "json/json.h"
const float imbalance_threshold_computing = 0.8; //阈值设置为0.8，可以由管控中心指定
const float imbalance_threshold_store = 0.8;     //阈值设置为0.8，可以由管控中心指定
const int overload_frequency_threshold = 10;     // 假设我们检查了20个时间点，超过一半的时间点资源超载，我们才认为需要迁移
// #include <string>

// This function must be implemented in the .cpp file to create
// a plugin that can be loaded at run-time
void migrateKey(const std::string& sourceHost, int sourcePort,
                const std::string& targetHost, int targetPort,
                const std::string& key, const std::string& password) {
    // 连接到源Redis服务器
    redisContext* sourceContext = redisConnect(sourceHost.c_str(), sourcePort);
    if (sourceContext == NULL || sourceContext->err) {
        std::cout << "Error: cannot connect to the source Redis server." << std::endl;
        return;
    }

    // 在源服务器进行认证
    redisReply* reply = static_cast<redisReply*>(redisCommand(sourceContext, "AUTH %s", password.c_str()));
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Authentication failed on the source Redis server." << std::endl;
        return;
    }

    // 在源服务器上DUMP key
    reply = static_cast<redisReply*>(redisCommand(sourceContext, "DUMP %s", key.c_str()));
    if (reply == NULL) {
        std::cout << "Error: cannot dump the key on the source Redis server." << std::endl;
        return;
    }

    // 保存DUMP的结果
    std::string dumpedValue(reply->str, reply->len);

    // 连接到目标Redis服务器
    redisContext* targetContext = redisConnect(targetHost.c_str(), targetPort);
    if (targetContext == NULL || targetContext->err) {
        std::cout << "Error: cannot connect to the target Redis server." << std::endl;
        return;
    }

    // 在目标服务器进行认证
    reply = static_cast<redisReply*>(redisCommand(targetContext, "AUTH %s", password.c_str()));
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Authentication failed on the target Redis server." << std::endl;
        return;
    }

    // 在目标服务器上RESTORE key
    reply = static_cast<redisReply*>(redisCommand(targetContext, "RESTORE %s 0 %b", key.c_str(), dumpedValue.data(), dumpedValue.size()));
    if (reply == NULL) {
        std::cout << "Error: cannot restore the key on the target Redis server." << std::endl;
        return;
    }

    // 关闭连接
    redisFree(sourceContext);
    redisFree(targetContext);
}

void migrateAllKeys(const std::string& sourceHost, int sourcePort,
                    const std::string& targetHost, int targetPort,
                    const std::string& password) {
    // 连接到源Redis服务器
    redisContext* sourceContext = redisConnect(sourceHost.c_str(), sourcePort);
    if (sourceContext == NULL || sourceContext->err) {
        std::cout << "Error: cannot connect to the source Redis server." << std::endl;
        return;
    }

    // 在源服务器进行认证
    redisReply* reply = static_cast<redisReply*>(redisCommand(sourceContext, "AUTH %s", password.c_str()));
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Authentication failed on the source Redis server." << std::endl;
        return;
    }

    // 获取所有的键
    reply = static_cast<redisReply*>(redisCommand(sourceContext, "KEYS *"));
    if (reply == NULL) {
        std::cout << "Error: cannot get keys from the source Redis server." << std::endl;
        return;
    }

    // 对每一个键，进行迁移操作
    for (size_t i = 0; i < reply->elements; i++) {
        migrateKey(sourceHost, sourcePort, targetHost, targetPort, reply->element[i]->str, password);
    }

    // 关闭连接
    redisFree(sourceContext);
}
BT_REGISTER_NODES(factory)
{
    DummyNodes::RegisterNodes(factory);
}

namespace DummyNodes
{

    void global_init(struct global *g)
    {
        memset(g, 0, sizeof(struct global));
    }

    void node_init(struct node *n)
    {
        memset(n, 0, sizeof(struct node));
    }

    void global_push(struct global *g, struct node *n)
    {
        if (g->tail)
        {
            g->tail->next = n;
            n->prior = g->tail;
        }
        else
        {
            g->head = n;
            g->head->prior = NULL;
        }

        g->tail = n;
        g->tail->next = NULL;
        g->count++;
    }

    void global_del(struct global *g, struct node *n)
    {
        if (n == g->head)
        {
            g->head = n->next;
            g->head->prior = NULL;
        }
        if (n == g->tail)
        {
            g->tail = n->prior;
            g->tail->next = NULL;
        }
        else
        {
            n->next->prior = n->prior;
            n->prior->next = n->next;
        }
        free(n);
    }

    DummyNodes::localhost *get_localhost()
    {
        char buff[256];
        struct localhost *newlocalhost;
        newlocalhost = (struct localhost *)malloc(sizeof(struct localhost));

        FILE *fp = fopen("localhost.txt", "r");
        fgets(buff, sizeof(buff) - 1, fp);

        sscanf(buff, "%s : %f %f", &newlocalhost->ip, &newlocalhost->computing, &newlocalhost->store);

        fclose(fp);
        return newlocalhost;
    }

    int get_graph(struct graph *g)
    {
        char buff[1024];
        //获取图，从文件打开，后续应该是从接口中获取
        FILE *fp = fopen("graph.txt", "r");

        if (NULL == fp)
        {
            printf("open error\n");
            return 0;
        }
        fgets(buff, sizeof(buff) - 1, fp);

        sscanf(buff, "vertexnum,edgenum:%d %d\n", &(g->vexnum), &(g->edgenum));

        for (int i = 0; i < g->vexnum; i++)
        {
            g->vertex[i] = i;
        }

        for (int i = 0; i < g->vexnum; i++)
        {
            fgets(buff, sizeof(buff) - 1, fp);
            //printf("strlen(buff) = %d, V%d ip = %s", strlen(buff), i, buff);
            strncpy(*(g->ip + i), buff, strlen(buff) - 1);
            //strncpy((char *)ip+20*i, buff, strlen(buff)-1);
            // printf("%s", (char *)ip+20*i);
            // printf("%s", (char *)ip+20*i)
            *(*(g->ip + i) + strlen(buff) - 1) = '\0';
            //*((char *)ip + i*20 + (strlen(buff) - 1)) = '\0';
            //printf("%s\n", *(ip+i));
        }

        for (int i = 0; i < g->vexnum; i++)
        {
            for (int j = 0; j < g->vexnum; j++)
            {
                g->edge[i][j] = INFINITY;
            }
        }

        int v1, v2, w;

        while (1)
        {

            fgets(buff, sizeof(buff) - 1, fp);
            //printf("%s\n", buff);

            sscanf(buff, "%d %d %d\n", &v1, &v2, &w);
            //printf("%d %d %d\n", v1, v2, w);
            g->edge[v1][v2] = w;
            g->edge[v2][v1] = w;

            if (feof(fp))
            {
                break;
            }
        }

        fclose(fp);
        return 1;
    }

    BT::NodeStatus CheckIntent()
    {
        std::cout << "Intent: OK" << std::endl;
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus CheckTranslate()
    {
        std::cout << "Translate: OK" << std::endl;
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus IntentInterface::input()
    {
        _intent = true;
        std::cout << "IntentInterface::input" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus IntentInterface::translate()
    {
        std::cout << "translate: [element1], [element2], [element3]" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus PolicyInterface::pathcal()
    {
        std::cout << "path: ....." << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    void ShortestPath_Dijkstra(struct graph *G, int v0, PathMatrix *p, ShortPathTable *D)
    {
        int final[MAX_VERtEX_NUM]; //用于存储各顶点是否已经确定最短路径的数组
        //对各数组进行初始化
        for (int v = 0; v < G->vexnum; v++)
        {
            final[v] = 0;
            (*D)[v] = G->edge[v0][v];
            (*p)[v] = 0;
            std::cout<<G->edge[v0][v]<<std::endl;
        }
        //由于以v0位下标的顶点为起始点，所以不用再判断
        (*D)[v0] = 0;
        final[v0] = 1;
        int k = 0;
        for (int i = 0; i < G->vexnum; i++)
        {
            int min = INFINITY;
            //选择到各顶点权值最小的顶点，即为本次能确定最短路径的顶点
            for (int w = 0; w < G->vexnum; w++)
            {
                if (!final[w])
                {
                    if ((*D)[w] < min)
                    {
                        k = w;
                        min = (*D)[w];
                    }
                }
            }
            //设置该顶点的标志位为1，避免下次重复判断
            final[k] = 1;
            //对v0到各顶点的权值进行更新
            for (int w = 0; w < G->vexnum; w++)
            {
                if (!final[w] && (min + G->edge[k][w] < (*D)[w]))
                {
                    (*D)[w] = min + G->edge[k][w];
                    (*p)[w] = k; //记录各个最短路径上存在的顶点
                }
            }
        }
        std::cout<<  "测试dijstra" <<(*D)[0]<<(*D)[1]<<(*D)[2]<<std::endl;
    }

    void get_gain(struct global *g, char (*ip)[20], ShortPathTable *w, int num)
    {
        struct node *n = g->head;
        // char *value_ip;
        for (int i = 0; i < g->count; i++)
        {
            // value_ip = n->value.ip;
            // value_ip = &n->value.ip;

            for (int j = 0; j < num; j++)
            {
                if (strcmp(n->value.ip, *(ip + j)))
                {
                    continue;
                }
                
                //printf("V%d cost: %d\n", j, (*w)[j]);
                n->cost = (*w)[j];
                //printf("------%s get_gain: %d\n", n->value.ip, n->cost);
                //std::cout << "测试get_gain" << std::endl;
                n->gain = (n->benefit + n->cost) / 2;
                std::cout << "============================================" << std::endl;
                
                    
                    std::cout << n->value.ip << "  cost: " << n->cost << "  benifit: " << n->benefit << "  gain: " << n->gain << std::endl;
                    
                
                std::cout << "============================================" << std::endl;
            }

            n = n->next;
        }
    }

    struct node *get_bestnode(struct global *g)
    {
        struct node *n = g->head;
        struct node *best = g->head;

        for (int i = 0; i < g->count; i++)
        {
            if (best->gain <= n->gain)
            {
                n = n->next;
                continue;
            }
            best = n;
            n = n->next;
        }

        return best;
    }

    void print_cost(ShortPathTable *t, int num)
    {
        for (int i = 0; i < num; i++)
        {
            printf("V%d cost: %d\n", i, (*t)[i]);
        }
    }

    BT::NodeStatus PolicyInterface::policylevel1()
    {
        // PathMatrix *new_P;
        // new_P = (PathMatrix *)malloc(sizeof(PathMatrix));
        // _P = new_P;
        // ShortPathTable *new_D;
        // new_D = (ShortPathTable *)malloc(sizeof(ShortPathTable));
        // _D = new_D;

        // ShortestPath_Dijkstra(_graph, 0, _P, _D);
        // get_gain(_global, _graph->ip, _D, _graph->vexnum);
        // _best = get_bestnode(_global);
        // std::cout << "-----" << std::endl;
        // std::cout << "the best node:" << _best->value.ip << std::endl;
        // std::cout << "-----" << std::endl;

        // _policy = true;

        // if (_policy == true)
        //     return BT::NodeStatus::SUCCESS;
        
        PathMatrix *new_P;
        new_P = (PathMatrix *)malloc(sizeof(PathMatrix));
        _P = new_P;
        ShortPathTable *new_D;
        new_D = (ShortPathTable *)malloc(sizeof(ShortPathTable));
        _D = new_D;

        std::cout << "11111111" << std::endl;

        ShortestPath_Dijkstra(_graph, 0, _P, _D);
        std::cout << "22222222" << std::endl;

        get_gain(_global, _graph->ip, _D, _graph->vexnum);
        
        std::cout << "33333333" << std::endl;
        _best = get_bestnode(_global);
        std::cout << "44444444" << std::endl;
        std::cout << "============================================" << std::endl;
        std::cout << "the best node:" << _best->value.ip << std::endl;
        std::cout << "============================================" << std::endl;

        _policy = true;

        std::cout << "Migration policy specified successfully!!" << std::endl;
        //修改返回为失败，之前师兄为成功
        return BT::NodeStatus::FAILURE;
    }

    Json::Value parseToJson(std::string &info)
    {
        Json::Value res;
        JSONCPP_STRING errs;

        Json::CharReaderBuilder readerBuilder;
        const std::unique_ptr<Json::CharReader> jsonReader(readerBuilder.newCharReader());
        bool flag = jsonReader->parse(info.c_str(), info.c_str() + info.length(), &res, &errs);
        if (!flag || !errs.empty())
        {
            std::cout << "parseJson err. " << errs << std::endl;
        }
        return res;
    }

    bool activateApp(const std::string appId)
    {
        // if (isActive(appId)) return true;
        std::string url1 = "http://192.168.1.108:8181/onos/v1/applications/" + appId + "/active";
        cpr::Response r = cpr::Post(cpr::Url{url1},
                                    cpr::Header{{"Authorization", "Basic b25vczpyb2Nrcw=="}, {"Content-Type", "*/*"}});
        std::cout << r.text << '\n';
        Json::Value resp1; //= parseToJson(r.text);
        if (resp1["state"].asString() == "ACTIVE")
        {
            // printf("%s has been activated\n", appId.c_str());
            std::cout << appId.c_str() << " has been activated" << std::endl;
            return true;
        }
        else
        {
            std::cout << appId.c_str() << " activate failed" << std::endl;
        }
        return false;
    }

    BT::NodeStatus PolicyInterface::distribution()
    {
        // int n;
        // char buf[4096];
        // char str[INET_ADDRSTRLEN];
        // if (_policy) {
        //     std::string s1(_best->value.ip);
        //     std::string s2 = "the best node: ";
        //     s2 += s1;
        //     strcpy(buf, s2.c_str());
        //     write(_sockfd, buf, strlen(buf));

        //     _policy = false;
        // }
        // std::string appid = "org.onosproject.ONOS_AntiJamming_Routing_Forward";
        // // std::string appid = "org.onosproject.fwd";

        // bool re = activateApp(appid);

        // if (re = true) {
        //     std::cout << "policy distribute successfuly" << std::endl;
        //     return BT::NodeStatus::SUCCESS;
        // }

        std::string ip(_best->value.ip);
        std::string url = "http://" + ip + ":2375/containers/PISKES_Server/start";
        cpr::Response r = cpr::Post(cpr::Url{url});

        std::cout << r.text << std::endl;
        //测试返回容器数据
        std::string url22 = "http://" + ip + ":2375/containers/json";
        cpr::Response r22 = cpr::Get(cpr::Url{url22});

        std::cout << r22.text << std::endl;
        //测试
        _distribute = true;
        //数据迁移
        std::string ip_str(_localhost->ip); // 将字符数组转换为 std::string
        migrateAllKeys(ip_str, 6379, ip, 6379, "miyaoredis");

        std::cout << "policy distribute success" << std::endl;
        //添加本地piskes容器的关闭,本地ip如何设置需要考虑
        //todo
        // std::string ip_str(_localhost->ip); // 将字符数组转换为 std::string
        std::string url_stop = "http://" + ip_str + ":2375/containers/PISKES_Server/stop";
        cpr::Response r2 = cpr::Post(cpr::Url{url_stop});

        //DNS接口
        std::string url_dns = "http://192.168.0.200:5000/update_ip";
        std::ostringstream jsonStream;
        jsonStream << R"({"ip":")" << ip << R"("})";
        
        std::string jsonBody = jsonStream.str();  // 使用原始字符串字面量来创建JSON字符串
        cpr::Response r33 = cpr::Post(
        cpr::Url{url_dns},
        cpr::Body{jsonBody},
        cpr::Header{{"Content-Type", "application/json"}}  // 设置Content-Type为application/json
        );
        std::cout << r33.text << std::endl;
        std::cout << "DNS修改成功" << std::endl;

        return BT::NodeStatus::SUCCESS;
    }
    // void distribution_cmd(std::string dst_ip)
    // {
    //     std::string ip(dst_ip);
    //     std::string url = "http://" + ip + ":2375/containers/piskes/start";
    //     cpr::Response r = cpr::Post(cpr::Url{url});

    //     std::cout << r.text << std::endl;
    //     //测试返回容器数据
    //     std::string url22 = "http://" + ip + ":2375/containers/json";
    //     cpr::Response r22 = cpr::Get(cpr::Url{url22});

    //     std::cout << r22.text << std::endl;
    //     //测试
    //     //_distribute = true;

    //     std::cout << "policy distribute success" << std::endl;
    //     //添加本地piskes容器的关闭,本地ip如何设置需要考虑
    //     //todo
    //     std::string ip_str("192.168.0.150"); // 将字符数组转换为 std::string
    //     std::string url_stop = "http://" + ip_str + ":2375/containers/piskes/stop";
    //     cpr::Response r2 = cpr::Post(cpr::Url{url_stop});

    //     //DNS接口
    //     std::string url_dns = "http://192.168.0.200:5000/update_ip";
    //     std::ostringstream jsonStream;
    //     jsonStream << R"({"ip":")" << ip << R"("})";
        
    //     std::string jsonBody = jsonStream.str();  // 使用原始字符串字面量来创建JSON字符串
    //     cpr::Response r33 = cpr::Post(
    //     cpr::Url{url_dns},
    //     cpr::Body{jsonBody},
    //     cpr::Header{{"Content-Type", "application/json"}}  // 设置Content-Type为application/json
    //     );
    //     std::cout << r33.text << std::endl;
    //     std::cout << "DNS修改成功" << std::endl;
    // }
    BT::NodeStatus PolicyInterface::policylevel3()
    {
        std::cout << "migration policy level3" << std::endl;
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus PolicyInterface::policylevel2()
    {
        std::cout << "migration policy level2" << std::endl;
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus PolicyInterface::migration()
    {
        std::cout << "make migration policy successfully" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    void print_global(struct global *g)
    {
        struct node *n = g->head;
        // printf("--------\n");
        std::cout << "============================================" << std::endl;
        for (int i = 0; i < g->count; i++)
        {
            // printf("%s cost: %d, benifit: %f, gain: %f\n", n->value.ip, n->cost, n->benefit, n->gain);
            std::cout << n->value.ip << "  cost: " << n->cost << "  benifit: " << n->benefit << "  gain: " << n->gain << std::endl;
            n = n->next;
        }
        std::cout << "============================================" << std::endl;
        // printf("-------\n");
    }

    void standard_cal(struct node *n)
    {
        float ave1, ave2, stand1, stand2;

        ave1 = (n->value.computing + n->value.store ) / 2.0;
        ave2 = (n->value.computing + n->value.store + 0.5) / 2.0;
        stand1 = ((n->value.computing  - ave1) * (n->value.computing  - ave1) + (n->value.store  - ave1) * (n->value.store  - ave1)) / 2.0;
        stand2 = ((n->value.computing + 0.2 - ave1) * (n->value.computing + 0.2 - ave1) + (n->value.store + 0.3 - ave1) * (n->value.store + 0.3 - ave1)) / 2.0;

        n->benefit = (stand2 - stand1) / ( stand1 + 0.01);
    }

    void get_benefit(struct global *g)
    {
        struct node *n;
        n = g->head;

        do
        {
            if (NULL == n)
            {
                break;
            }
            standard_cal(n);
            if (n->benefit <= 0)
            {
                global_del(g, n);
            }
            n = n->next;
        } while (NULL != n);
    }

    BT::NodeStatus PolicyInterface::getsource()
    {
        // char buff[256];
        // //char ip[20];
        // struct global *newglobal;
        // struct node *newnode;
        // struct localhost *localhost;

        // localhost = get_localhost();
        // newglobal = (struct global *)malloc(sizeof(struct global));
        // global_init(newglobal);
        // FILE *fp = fopen("resource.txt", "r");

        // if (NULL == fp) {
        //     // printf("open error\n");
        //     std::cout << "open error" << std::endl;
        //     free(localhost);
        //     free(newglobal);
        //     return BT::NodeStatus::FAILURE;
        // }
        // while(1) {
        //     newnode = (struct node *)malloc(sizeof(struct node));
        //     node_init(newnode);
        //     fgets(buff ,sizeof(buff)-1, fp);
        //     //printf("%s\n", buff);

        //     sscanf(buff, "%s : %f %f\n", newnode->value.ip, &newnode->value.computing, &newnode->value.store);
        //     // printf("-----");
        //     // printf("%s: %d\n", ip, strlen(ip));
        //     // printf("----");
        //     //strncpy(&newnode->value.ip, ip, strlen(ip)-1);
        //     //printf("%s : %f %f\n", newnode->value.ip, newnode->value.computing, newnode->value.store);

        //     if (!strcmp(localhost->ip, newnode->value.ip)) {
        //         free(newnode);
        //         newnode = NULL;
        //         continue;
        //     }

        //     if (newnode->value.computing > 0.6 || newnode->value.store > 0.5) {
        //         free(newnode);
        //         newnode = NULL;
        //         continue;
        //     }
        //     global_push(newglobal, newnode);
        //     newnode = NULL;

        //     if (feof(fp)) {
        //         break;
        //     }
        // }

        // fclose(fp);
        // _global = newglobal;
        // _localhost = localhost;
        // print_global(newglobal);

        // get_benefit(_global);
        // getgraph();

        if (_resource == true)
            return BT::NodeStatus::FAILURE;

        struct global *newglobal;
        struct node *newnode;
        newglobal = (struct global *)malloc(sizeof(struct global));
            if (newglobal == NULL) {
                // handle error
                std::cout<<"newglobal == NULL"<<std::endl;
            }

        newnode = (struct node *)malloc(sizeof(struct node));
            if (newnode == NULL) {
                // handle error
                std::cout<<"newglobal == NULL"<<std::endl;
            }

        // struct localhost *localhost;

        std::string str_url = "http://192.168.0.155:2379";
        etcd::Client etcd(str_url);

        // localhost = get_localhost();
        newglobal = (struct global *)malloc(sizeof(struct global));
        global_init(newglobal);

        //后续从全网获取所有ip，从拓扑获取
        std::vector<std::string> nodes{
            "192.168.0.150", 
            "192.168.0.151",
            "192.168.0.152",
            "192.168.0.153",
            "192.168.0.154",
            "192.168.0.159",
            "192.168.0.156",
            "192.168.0.157",
            "192.168.0.158",  
            "192.168.0.200"};

        for (int i = 0; i < nodes.size(); i++)
        {
            newnode = (struct node *)malloc(sizeof(struct node));
            node_init(newnode);
            std::string ip = nodes[i];
            strcpy(newnode->value.ip, ip.c_str());

            if (!strcmp(_localhost->ip, newnode->value.ip))
            {
                free(newnode);
                newnode = NULL;
                continue;
            }

            // std::string index1 = "/" + ip + "/cal";
            // std::string index2 = "/" + ip + "/store";

            //格式：/IPaddress/type/res/param/,type是指四类资源："forward", "storage", "calculate", "safe";
            //res就是资源的名称； param就是“ctn”,"image", "disk"
            std::string index1 = "/" + ip + "/cal/cpuUsage/";
            std::string index2 = "/" + ip + "/sto/memUsage/";
            // std::string index1 = "/" + ip + "/cal/memUsage/";
            // std::string index2 = "/" + ip + "/storage/diskUsage/";

            etcd::Response response1 = etcd.get(index1).get();
            etcd::Response response2 = etcd.get(index2).get();

            newnode->value.computing = stof(response1.value().as_string());
            newnode->value.store = stof(response2.value().as_string());
            //dubug
            std::cout<<newnode->value.ip<<":"<<newnode->value.computing<<";"<<newnode->value.store<<std::endl;

            if (newnode->value.computing > 0.6 || newnode->value.store > 0.5)
            {
                free(newnode);
                newnode = NULL;
                continue;
            }

            global_push(newglobal, newnode);
            newnode = NULL;
        }
        
        _global = newglobal;
        // _localhost = localhost;
        get_benefit(_global);
        
        //print_global(newglobal);

        // get_benefit(_global);
        getgraph();
        
        std::cout << "get source success" << std::endl;
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus PolicyInterface::get_local_resource()
    {
        std::cout << "trigger get_local_resource" << std::endl;

        // struct localhost *localhost;
        _localhost = get_localhost();
        // _localhost = localhost;

        std::string str_url = "http://192.168.0.155:2379";
        etcd::Client etcd(str_url);

        std::string ip(_localhost->ip);

        //格式：/IPaddress/type/res/param/,type是指四类资源："forward", "storage", "calculate", "safe";
        //res就是资源的名称； param就是“ctn”,"image", "disk"
        std::string index1 = "/" + ip + "/cal/cpuUsage/";
        std::string index2 = "/" + ip + "/sto/memUsage/";
        // std::string index1 = "/" + ip + "/cal/memUsage/";
        // std::string index2 = "/" + ip + "/storage/diskUsage/";

        std::cout << "trigger get_local_resource -----1" << std::endl;

        etcd::Response response1 = etcd.get(index1).get();
        etcd::Response response2 = etcd.get(index2).get();

        std::cout << "trigger get_local_resource -----2" << std::endl;

        std::cout << "local_cal:" <<response1.value().as_string() << std::endl;
        std::cout << "local_store:" <<response2.value().as_string() << std::endl;

        _localhost->computing = stof(response1.value().as_string());
        _localhost->store = stof(response2.value().as_string());

        std::cout << "trigger get_local_resource -----3" << std::endl;

        std::cout << "Get LocalResource Success!!" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    void PolicyInterface::getgraph()
    {
        struct graph *G;
        G = (struct graph *)malloc(sizeof(struct graph));
        _graph = G;
        get_graph(_graph);
    }

    BT::NodeStatus PolicyInterface::checksource()
    {

        std::cout << "trigger step3" << std::endl;

        if (_localhost->computing > imbalance_threshold_computing || _localhost->store > imbalance_threshold_store)
        {
            std::cout << "Resource exceeds threshold" << std::endl;
            return BT::NodeStatus::FAILURE;
        }

        std::cout << "Resource did not exceed threshold" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus PolicyInterface::checklevel1()
    {
        std::cout << "source change level1" << std::endl;
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus PolicyInterface::checklevel2()
    {
        std::cout << "source change level2" << std::endl;
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus PolicyInterface::checklevel3()
    {
        std::cout << "source change level3" << std::endl;
        return BT::NodeStatus::FAILURE;
    }
    //迁移判决
    BT::NodeStatus PolicyInterface::migration_jude()
    {
        //添加迁移判决
        std::cout << "migration_jude测试" << std::endl;
        // float imbalance = this->get_imbalance();
        // int overload_frequency = this->get_overload_frequency();
        if (get_overload_frequency() > overload_frequency_threshold)
        {
            if (!get_imbalance())
            {
                return BT::NodeStatus::FAILURE; // 如果资源利用严重不均衡，并且频繁超载，那么我们返回成功，表示需要进行迁移
            }
            else
            {
                return BT::NodeStatus::SUCCESS; // 否则，我们返回失败，表示不需要进行迁移
            }
        }
        else
        {
            return BT::NodeStatus::SUCCESS;
        }
        //分隔，下面为师兄原本代码
        // std::cout << "Migration judgment success" << std::endl;
        // return BT::NodeStatus::FAILURE;
    }
    //添加迁移判决内部函数
    int PolicyInterface::get_imbalance()
    {
        //应该需要重新获取本地节点资源信息，只是使用对象的数值应该是不会变的
        float call;
        float stoo;
        std::string str_url = "http://192.168.0.155:2379";
        etcd::Client etcd(str_url);
        std::string ip(_localhost->ip);
        std::string index1 = "/" + ip + "/cal/cpuUsage/";
        std::string index2 = "/" + ip + "/sto/memUsage/";
        etcd::Response response1 = etcd.get(index1).get();
        etcd::Response response2 = etcd.get(index2).get();
        call = stof(response1.value().as_string());
        stoo = stof(response2.value().as_string());
        if (call > imbalance_threshold_computing || stoo > imbalance_threshold_store)
        {
            std::cout << "get_imbalance测试" << std::endl;
            return 0;
        }
        else
        {

            std::cout << "get_imbalance测试2" << std::endl;
            return 1;
        }
    };
    int PolicyInterface::get_overload_frequency()
    {
        int cunt = 0;
        int i;
        if (!get_imbalance())
        {
            cunt++;
            for (i = 0; i < 20; i++)
            {
                if (!get_imbalance())
                {
                    cunt++;
                }
                sleep(0.2);
            }
        }
        std::cout << "get_overload_frequency测试" << std::endl;
        return cunt;
    };
    //分隔代码
    BT::NodeStatus CheckBattery()
    {
        std::cout << "[ Battery: OK ]" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus CheckTemperature()
    {
        std::cout << "[ Temperature: OK ]" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus SayHello()
    {
        std::cout << "Robot says: Hello World" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus GripperInterface::open()
    {
        _opened = true;
        std::cout << "GripperInterface::open" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus GripperInterface::close()
    {
        std::cout << "GripperInterface::close" << std::endl;
        _opened = false;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus ApproachObject::tick()
    {
        std::cout << "ApproachObject: " << this->name() << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus SaySomething::tick()
    {
        auto msg = getInput<std::string>("message");
        if (!msg)
        {
            throw BT::RuntimeError("missing required input [message]: ", msg.error());
        }

        std::cout << "Robot says: " << msg.value() << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus SaySomethingSimple(BT::TreeNode &self)
    {
        auto msg = self.getInput<std::string>("message");
        if (!msg)
        {
            throw BT::RuntimeError("missing required input [message]: ", msg.error());
        }

        std::cout << "Robot says: " << msg.value() << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

}
