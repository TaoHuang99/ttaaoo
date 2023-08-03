#include "sai_node.h"

#include "cpr/cpr.h"
#include "json/json.h"

#include <string>

// This function must be implemented in the .cpp file to create
// a plugin that can be loaded at run-time
BT_REGISTER_NODES(factory)
{
//    CrossDoor::RegisterNodes(factory);
    SAI::RegisterNodes(factory);
}

inline void SleepMS(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// For simplicity, in this example the status of the door is not shared
// using ports and blackboards
//static bool _door_open   = false;
//static bool _door_locked = true;
static bool _sai_finish   = false;
static bool _sai_null   = true;




//-----------------------------------------------------------behavior tree node as follow:
NodeStatus SAI::IsServiceFlowInput()
{
    std::cout << "[ Service Flow not null ! ]" << std::endl;
    return BT::NodeStatus::FAILURE;
    /* if can get pga_result
    if pga_res == null:
        return BT::NodeStatus::FAILURE;
    else:
        return BT::NodeStatus::SUCCESS;
    */
}

NodeStatus SAI::IsContainCollaborativeSchedulingModel()
{
    SleepMS(500);
    std::cout << "[ Collaborative Scheduling Model is here ! ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::IsParseServiceFlowCompleted()
{
    SleepMS(500);
    std::cout << "[ Finished parsing the service flow ! ]" << std::endl;
    return BT::NodeStatus::FAILURE;
}

NodeStatus SAI::PrintServiceContent()
{
    SleepMS(500);
    std::cout << "[ Service Content: Routing service ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::IsServiceSchedulingEnvironmentDeployedSuccessfully()
{
    SleepMS(500);
    std::cout << "[ The scheduling environment has been deployed successfully! ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::RunningCollaborativeScheduling()
{
    SleepMS(5000);
    std::cout << "[ Collaborative scheduling is Running! ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::IsContainResourceAwarenessModel()
{
    std::cout << "[ Resource Awareness Model is here ! ]" << std::endl;
    return BT::NodeStatus::FAILURE;
}

NodeStatus SAI::IsCurrentResourcesCanSupportServiceCompletion()
{
    std::cout << "[ Resources can support service completion! ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::PrintCurrentResources()
{
    std::cout << "[ Current Resources: ]" << std::endl; //need further write
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::WhatResourcesAreNeeded()
{
    std::cout << "[ Searching for resources ... ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::PrintRequiredResourceName()
{
    std::cout << "[ Required resource name: ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::IsOpenResourceAwarenessArchitecture()
{
    std::cout << "[ Opening resource awareness architecture ... ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::RunningResourceAwareness()
{
    std::cout << "[ Resource awareness is Running! ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::IsContainServiceMigrationModel()
{
    std::cout << "[ Service Migration Model is here! ]" << std::endl;
    return BT::NodeStatus::FAILURE;
}

NodeStatus SAI::PrintCurrentResourcesInformation()
{
    std::cout << "[ Current Resources Information: ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::IsCurrentResourceExceedstheThreshold()
{
    std::cout << "[ Threshold exceeded, warning!warning!warning! ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

Json::Value parseToJson(std::string &info) {
    Json::Value res;
    JSONCPP_STRING errs;

    Json::CharReaderBuilder readerBuilder;
    const std::unique_ptr<Json::CharReader>  jsonReader(readerBuilder.newCharReader());
    bool flag = jsonReader->parse(info.c_str(), info.c_str()+info.length(), &res, &errs);
    if (!flag || !errs.empty()) {
        std::cout << "parseJson err. " << errs << std::endl;
    }
    return res;
}

bool activateApp(const std::string appId) {
    // if (isActive(appId)) return true;
    std::string url1 = "http://192.168.0.104:8181/onos/v1/applications/" + appId + "/active";
    cpr::Response r = cpr::Post(cpr::Url{url1},
                                cpr::Header{{
                                                    "Authorization", "Basic b25vczpyb2Nrcw=="
                                            },{
                                                    "Content-Type", "*/*"
                                            }}
    );
    std::cout << r.text << '\n';
    Json::Value resp1 = parseToJson(r.text);
    if (resp1["state"].asString() == "ACTIVE")
    {
        // printf("%s has been activated\n", appId.c_str());
        std::cout << appId.c_str() << " has been activated" << std::endl;
        return true;
    } else {
        std::cout << appId.c_str() << " activate failed" << std::endl;
    }
    return false;
}

NodeStatus SAI::RunningServiceMigration()
{

    std::string appid = "org.onosproject.ONOS_AntiJamming_Routing_Forward";
    // std::string appid = "org.onosproject.fwd";

    bool re = activateApp(appid);

    if (re = true) {
        std::cout << "policy distribute successfuly" << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
    std::cout << "policy distribute failed" << std::endl;
    return BT::NodeStatus::SUCCESS;

    // std::cout << "[ Service Migration is running! ]" << std::endl;
    // return BT::NodeStatus::SUCCESS;
}

NodeStatus SAI::PrintMigrationPath()
{
    std::cout << "[ Migration Path result is: ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

//-----------------------------------------------------------------end



//beginning
NodeStatus SAI::IsIntentInput()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsIntentNULL()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//IntentModel
NodeStatus SAI::IsIntentModelFinish()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsIntentModelWaiting()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//    perception
NodeStatus SAI::IsIntentPerceptionFinish()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsIntentPerceptionWaiting()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IntentPerceptionRunning()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//    classifier
NodeStatus SAI::IsIntentTranslationFinish()
{
    SleepMS(500);
    return _sai_null? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsIntentTranslationWaiting()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IntentTranslationRunning()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//    detection
NodeStatus SAI::IsIntentConfilctDetectionFinish()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsIntentConfilctDetectionWaiting()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::ConfilctDetectionRunning()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//ActionModel
NodeStatus SAI::IsActionModelFinish()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsActionModelWaiting()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//    Mapping
NodeStatus SAI::IsIntentPolicyMappingFinish()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsIntentPolicyMappingWaiting()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IntentPolicyMappingRunning()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//    Formulation
NodeStatus SAI::IsPolicyRefinementFinish()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsPolicyRefinementWaiting()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::PolicyRefinementRunning()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

//    Detection
NodeStatus SAI::IsPolicyConfilctDetectionFinish()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsPolicyConfilctDetectionWaiting()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::PolicyConfilctDetectionRunning()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//StateModel
NodeStatus SAI::IsStateModelFinish()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsStateModelWaiting()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//    DeploymentAndVerification
NodeStatus SAI::IsPolicyDeploymentAndVerificationFinish()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::IsPolicyDeploymentAndVerificationWaiting()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::PolicyDeploymentAndVerificationRunning()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
//    Feedback
NodeStatus SAI::IsCurrentSituationFeedback()
{
    SleepMS(500);
    return _sai_finish ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus SAI::DisplayCurrentNetworkSituation()
{
    SleepMS(500);
    return _sai_null ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}



// Register at once all the Actions and Conditions in this file
void SAI::RegisterNodes(BehaviorTreeFactory& factory)
{

//----------------------------bt node status:
    factory.registerSimpleCondition("IsServiceFlowInput", std::bind(IsServiceFlowInput));
    factory.registerSimpleCondition("IsContainCollaborativeSchedulingModel", std::bind(IsContainCollaborativeSchedulingModel));
    factory.registerSimpleCondition("IsParseServiceFlowCompleted", std::bind(IsParseServiceFlowCompleted));
    factory.registerSimpleCondition("IsServiceSchedulingEnvironmentDeployedSuccessfully", std::bind(IsServiceSchedulingEnvironmentDeployedSuccessfully));
    factory.registerSimpleCondition("IsContainResourceAwarenessModel", std::bind(IsContainResourceAwarenessModel));
    factory.registerSimpleCondition("IsCurrentResourcesCanSupportServiceCompletion", std::bind(IsCurrentResourcesCanSupportServiceCompletion));
    factory.registerSimpleCondition("WhatResourcesAreNeeded", std::bind(WhatResourcesAreNeeded));
    factory.registerSimpleCondition("IsOpenResourceAwarenessArchitecture", std::bind(IsOpenResourceAwarenessArchitecture));
    factory.registerSimpleCondition("IsContainServiceMigrationModel", std::bind(IsContainServiceMigrationModel));
    factory.registerSimpleCondition("IsCurrentResourceExceedstheThreshold", std::bind(IsCurrentResourceExceedstheThreshold));

    factory.registerSimpleAction("PrintServiceContent", std::bind(PrintServiceContent));
    factory.registerSimpleAction("RunningCollaborativeScheduling", std::bind(RunningCollaborativeScheduling));
    factory.registerSimpleAction("PrintCurrentResources", std::bind(PrintCurrentResources));
    factory.registerSimpleAction("PrintRequiredResourceName", std::bind(PrintRequiredResourceName));
    factory.registerSimpleAction("RunningResourceAwareness", std::bind(RunningResourceAwareness));
    factory.registerSimpleAction("PrintCurrentResourcesInformation", std::bind(PrintCurrentResourcesInformation));
    factory.registerSimpleAction("RunningServiceMigration", std::bind(RunningServiceMigration));
    factory.registerSimpleAction("PrintMigrationPath", std::bind(PrintMigrationPath));

    //----------------------------end

//    factory.registerSimpleCondition("IsDoorOpen", std::bind(IsDoorOpen));
//    factory.registerSimpleAction("PassThroughDoor", std::bind(PassThroughDoor));
//    factory.registerSimpleAction("PassThroughWindow", std::bind(PassThroughWindow));
//    factory.registerSimpleAction("OpenDoor", std::bind(OpenDoor));
//    factory.registerSimpleAction("CloseDoor", std::bind(CloseDoor));
//    factory.registerSimpleCondition("IsDoorLocked", std::bind(IsDoorLocked));
//    factory.registerSimpleAction("UnlockDoor", std::bind(UnlockDoor));
    factory.registerSimpleCondition("IsIntentInput", std::bind(IsIntentInput));
    factory.registerSimpleCondition("IsIntentNULL", std::bind(IsIntentNULL));
    factory.registerSimpleCondition("IsIntentModelFinish", std::bind(IsIntentModelFinish));
    factory.registerSimpleCondition("IsIntentModelWaiting", std::bind(IsIntentModelWaiting));
    factory.registerSimpleCondition("IsIntentPerceptionFinish", std::bind(IsIntentPerceptionFinish));
    factory.registerSimpleCondition("IsIntentPerceptionWaiting", std::bind(IsIntentPerceptionWaiting));
    factory.registerSimpleAction("IntentPerceptionRunning", std::bind(IntentPerceptionRunning));
    factory.registerSimpleCondition("IsIntentTranslationFinish", std::bind(IsIntentTranslationFinish));
    factory.registerSimpleCondition("IsIntentTranslationWaiting", std::bind(IsIntentTranslationWaiting));
    factory.registerSimpleAction("IntentTranslationRunning", std::bind(IntentTranslationRunning));
    factory.registerSimpleCondition("IsIntentConfilctDetectionFinish", std::bind(IsIntentConfilctDetectionFinish));
    factory.registerSimpleCondition("IsIntentConfilctDetectionWaiting", std::bind(IsIntentConfilctDetectionWaiting));
    factory.registerSimpleAction("ConfilctDetectionRunning", std::bind(ConfilctDetectionRunning));
    factory.registerSimpleCondition("IsActionModelFinish", std::bind(IsActionModelFinish));
    factory.registerSimpleCondition("IsActionModelWaiting", std::bind(IsActionModelWaiting));
    factory.registerSimpleCondition("IsIntentPolicyMappingFinish", std::bind(IsIntentPolicyMappingFinish));
    factory.registerSimpleCondition("IsIntentPolicyMappingWaiting", std::bind(IsIntentPolicyMappingWaiting));
    factory.registerSimpleAction("IntentPolicyMappingRunning", std::bind(IntentPolicyMappingRunning));
    factory.registerSimpleCondition("IsPolicyRefinementFinish", std::bind(IsPolicyRefinementFinish));
    factory.registerSimpleCondition("IsPolicyRefinementWaiting", std::bind(IsPolicyRefinementWaiting));
    factory.registerSimpleAction("PolicyRefinementRunning", std::bind(PolicyRefinementRunning));
    factory.registerSimpleCondition("IsPolicyConfilctDetectionFinish", std::bind(IsPolicyConfilctDetectionFinish));
    factory.registerSimpleCondition("IsPolicyConfilctDetectionWaiting", std::bind(IsPolicyConfilctDetectionWaiting));
    factory.registerSimpleAction("PolicyConfilctDetectionRunning", std::bind(PolicyConfilctDetectionRunning));
    factory.registerSimpleCondition("IsStateModelFinish", std::bind(IsStateModelFinish));
    factory.registerSimpleCondition("IsStateModelWaiting", std::bind(IsStateModelWaiting));
    factory.registerSimpleCondition("IsPolicyDeploymentAndVerificationFinish", std::bind(IsPolicyDeploymentAndVerificationFinish));
    factory.registerSimpleCondition("IsPolicyDeploymentAndVerificationWaiting", std::bind(IsPolicyDeploymentAndVerificationWaiting));
    factory.registerSimpleAction("PolicyDeploymentAndVerificationRunning", std::bind(PolicyDeploymentAndVerificationRunning));
    factory.registerSimpleCondition("IsCurrentSituationFeedback", std::bind(IsCurrentSituationFeedback));
    factory.registerSimpleAction("DisplayCurrentNetworkSituation", std::bind(DisplayCurrentNetworkSituation));
}
