#include "BT_BTCtrl.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "create.h"

// #include "dummy_nodes.h"
#include "sai_node.h"

using namespace BTtool;

// Add definition of your processing function here

void BTCtrl::BTrun(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback, std::string &&service)
{

    // auto json = req.getJsonObject();
    std::string str = service;
    int status_num = stoi(str);

    const char * filename = "create.xml";
    int iRet = createXML(filename,status_num);
    if(iRet != XML_SUCCESS)
        std::cout<<"create xml fail!"<<std::endl;
    //iRet = addXML(filename);
    if(iRet != XML_SUCCESS)
        std::cout<<"add xml fail!"<<std::endl;
    //iRet = modifyXML(filename);
    if(iRet != XML_SUCCESS)
        std::cout<<"modify or delte xml fail!"<<std::endl;



     

    using namespace BT;

    // static const char* xml_text = R"(

    // <root main_tree_to_execute = "MainTree" >

    //     <BehaviorTree ID="MainTree">
    //         <Sequence name="root">
    //             <Sequence name="left">
    //                 <Fallback name="intent_put">
    //                     <CheckIntent name="check_intent" />
    //                     <IntentPut name="intent_put" />
    //                 </Fallback>
    //                 <Fallback name="intent_translate">
    //                     <CheckTranslate name="check_translate" />
    //                     <Sequence name="translate">
    //                         <IntentTranslate name="intent_translate" />
    //                         <PathCalculate name="path_calculate" />
    //                         <ServiceMigration name="service_migration" />
    //                         <PolicyDistribution name="policy_distribution" />
    //                     </Sequence>
    //                 </Fallback>
    //             </Sequence>
    //             <Sequence name="right">
    //                 <GetSource name="get_source" />
    //                 <Fallback name="auxiliary_policy">
    //                     <CheckSource name="check_source" />
    //                     <Fallback name="level3">
    //                         <CheckLevel3 name="check_level3" />
    //                         <Policy3 name="policy3" />
    //                     </Fallback>
    //                     <Fallback name="level2">
    //                         <CheckLevel2 name="check_level2" />
    //                         <Policy2 name="policy2" />
    //                     </Fallback>
    //                     <Fallback name="level1">
    //                         <CheckLevel1 name="check_level1" />
    //                         <Policy1 name="policy1" />
    //                     </Fallback>
    //                 </Fallback>
    //                 <Fallback name="policydist">
    //                     <MigrationJudge name="migration_judge" />
    //                     <PolicyDistribution name="policy_distribution" />
    //                 </Fallback>
    //             </Sequence>
    //         </Sequence>
    //     </BehaviorTree>

    // </root>
    // )";

    // using namespace DummyNodes;

    // BehaviorTreeFactory factory;
    // PolicyInterface policy;

    // factory.registerSimpleCondition("CheckIntent", std::bind(CheckIntent));
    // IntentInterface intent;
    // factory.registerSimpleAction("IntentPut", std::bind(&IntentInterface::input, &intent));
    // factory.registerSimpleCondition("CheckTranslate", std::bind(CheckTranslate));
    // factory.registerSimpleAction("IntentTranslate", std::bind(&IntentInterface::translate, &intent));
    // // PolicyInterface policy;
    // factory.registerSimpleAction("PathCalculate", std::bind(&PolicyInterface::pathcal, &policy));
    // factory.registerSimpleAction("ServiceMigration", std::bind(&PolicyInterface::migration, &policy));
    // factory.registerSimpleAction("PolicyDistribution", std::bind(&PolicyInterface::distribution, &policy));
    // // PolicyInterface policy;
    // factory.registerSimpleAction("GetSource", std::bind(&PolicyInterface::getsource, &policy));
    // factory.registerSimpleAction("CheckSource", std::bind(&PolicyInterface::checksource, &policy));
    // factory.registerSimpleAction("CheckLevel3", std::bind(&PolicyInterface::checklevel3, &policy));
    // factory.registerSimpleAction("CheckLevel2", std::bind(&PolicyInterface::checklevel2, &policy));
    // factory.registerSimpleAction("CheckLevel1", std::bind(&PolicyInterface::checklevel1, &policy));
    // factory.registerSimpleAction("Policy3", std::bind(&PolicyInterface::policylevel3, &policy));
    // factory.registerSimpleAction("Policy2", std::bind(&PolicyInterface::policylevel2, &policy));
    // factory.registerSimpleAction("Policy1", std::bind(&PolicyInterface::policylevel1, &policy));
    // factory.registerSimpleAction("MigrationJudge", std::bind(&PolicyInterface::migration_jude, &policy));

    // auto tree = factory.createTreeFromText(xml_text);

    // tree.tickRoot();  



    static const char* xml_text = R"(
    <root main_tree_to_execute = "MainTree">
        <!--------------------------------------->
        <BehaviorTree ID="MainTree">
            <Fallback name="root_Fallback">
                <IsServiceFlowInput/>
                <Sequence name="BT_process_sequence">
                    <Fallback name="Sub1_Fallback">
                        <IsContainCollaborativeSchedulingModel/>
                        <Sequence name="Collaborative Scheduling Sequence 1">
                            <IsParseServiceFlowCompleted/>
                            <PrintServiceContent/>
                            <Sequence name="Collaborative Scheduling Sequence 2">
                                <IsServiceSchedulingEnvironmentDeployedSuccessfully/>
                                <RunningCollaborativeScheduling/>
                            </Sequence>
                        </Sequence>
                    </Fallback>  
                
                    <Fallback name="Sub2_Fallback">
                        <IsContainResourceAwarenessModel/>
                        <Sequence name="Resource Awareness Sequence 1">
                            <IsCurrentResourcesCanSupportServiceCompletion/>
                            <PrintCurrentResources/>
                            <WhatResourcesAreNeeded/>
                            <PrintRequiredResourceName/>
                            <Sequence name="Resource Awareness Sequence 2">
                                <IsOpenResourceAwarenessArchitecture/>
                                <RunningResourceAwareness/>
                            </Sequence>
                        </Sequence>
                    </Fallback>    
                    
                    <Fallback name="Sub3_Fallback">
                        <IsContainServiceMigrationModel/>
                        <Sequence name="Service Migration Sequence 1">
                            <PrintCurrentResourcesInformation/>
                            <IsCurrentResourceExceedstheThreshold/>
                            <Sequence name="Service Migration Sequence 2">
                                <RunningServiceMigration/>
                                <PrintMigrationPath/>
                            </Sequence>
                        </Sequence>
                    </Fallback>     
                    
                </Sequence>
            </Fallback>
        </BehaviorTree>
        <!---------------------------------------> 
    </root>
    )";


    BT::BehaviorTreeFactory factory;
    SAI::RegisterNodes(factory);
    auto tree = factory.createTreeFromFile("/home/song/drogon/BT_server/BT_server/build/create.xml");
    // auto tree = factory.createTreeFromText(xml_text);
    tree.tickRoot();
    remove("/home/song/drogon/BT_server/BT_server/build/create.xml");


    Json::Value ret;
    ret["service"] = str;
    ret["result"]="success";
    // ret["user_id"]=userId;
    // ret["gender"]=1;
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    callback(resp);

    // auto resp=HttpResponse::newHttpResponse();
    // resp->setStatusCode(k200OK);
    // resp->setContentTypeCode(CT_TEXT_HTML);
    // resp->setBody("Hello World!");
    // callback(resp);

}
