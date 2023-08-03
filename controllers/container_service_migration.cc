#include "container_service_migration.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "dummy_nodes.h"
#include "cpr/cpr.h"
#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
using namespace container;

// Add definition of your processing function here

void service_migration::trigger(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback) {

    std::cout << "trigger start" << std::endl;

    using namespace BT;

    static const char* xml_text = R"(

    <root main_tree_to_execute = "MainTree" >

        <BehaviorTree ID="MainTree">
            <Sequence name="root">
                <GetLocSource name="get_local_source" />
                <Fallback name="auxiliary_policy">
                    <CheckSource name="check_source" />
                    <GetSource name="get_source" />
                    <Policy1 name="policy1" />
                    <Fallback name="policydist">
                        <MigrationJudge name="migration_judge" />
                        <PolicyDistribution name="policy_distribution" />
                    </Fallback>
                </Fallback>
            </Sequence>
        </BehaviorTree>

    </root>
    )";

    using namespace DummyNodes;

    BehaviorTreeFactory factory;
    PolicyInterface policy;

    factory.registerSimpleAction("GetLocSource", std::bind(&PolicyInterface::get_local_resource, &policy));
    factory.registerSimpleAction("GetSource", std::bind(&PolicyInterface::getsource, &policy));
    factory.registerSimpleAction("CheckSource", std::bind(&PolicyInterface::checksource, &policy));
    factory.registerSimpleAction("Policy1", std::bind(&PolicyInterface::policylevel1, &policy));
    factory.registerSimpleAction("MigrationJudge", std::bind(&PolicyInterface::migration_jude, &policy));
    factory.registerSimpleAction("PolicyDistribution", std::bind(&PolicyInterface::distribution, &policy));

    auto tree = factory.createTreeFromText(xml_text);

    std::cout << "trigger step1" << std::endl;

    while (!policy._distribute) {

        tree.tickRoot();
        
    }

    // tree.tickRoot();

    Json::Value ret;
    ret["code"] = 200;
    std::string ip(policy._best->value.ip);
    ret["dstnode"] = ip;
    ret["msg"] = "success";

    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);

}
//管控中心强制服务迁移接口
void service_migration::cmd(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback) {
    std::cout << "service_migration cmd start" << std::endl;
    auto json = req->getJsonObject();
    if(json)
    {
        std::string container_name = (*json)["container_name"].asString();
        std::string dst_ip = (*json)["dstnode"].asString();

        // TODO: 你的服务迁移逻辑
        std::string ip(dst_ip);
        std::string url = "http://" + ip + ":2375/containers/PISKES_Server/start";
        cpr::Response r = cpr::Post(cpr::Url{url});

        std::cout << r.text << std::endl;
        //测试返回容器数据
        std::string url22 = "http://" + ip + ":2375/containers/json";
        cpr::Response r22 = cpr::Get(cpr::Url{url22});

        std::cout << r22.text << std::endl;
        //测试
        //_distribute = true;
        //数据迁移，本地ip地址需要修改
        std::string ip_str("192.168.0.150"); // 将字符数组转换为 std::string
        migrateAllKeys(ip_str, 6379, ip, 6379, "miyaoredis");
        std::cout << "Data Migration！" << std::endl;

        std::cout << "policy distribute success" << std::endl;
        //添加本地piskes容器的关闭,本地ip如何设置需要考虑
        //todo
        // std::string ip_str("192.168.0.150"); // 将字符数组转换为 std::string
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
        //
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);

        // 使用Json::Value来构建JSON响应
        Json::Value res;
        res["code"] = 200;
        res["msg"] = "成功";

        std::string jsonStr = res.toStyledString();
        resp->setBody(jsonStr);

        callback(resp);
    }
    else
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);

        // 使用Json::Value来构建JSON响应
        Json::Value res;
        res["code"] = 400;
        res["msg"] = "Bad Request: JSON object not found or malformed";

        std::string jsonStr = res.toStyledString();
        resp->setBody(jsonStr);

        callback(resp);
    }

}
