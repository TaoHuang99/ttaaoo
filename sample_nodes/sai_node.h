#pragma once

#include "behaviortree_cpp_v3/bt_factory.h"

using namespace BT;

namespace SAI
{

    //----------------------------bt node
    BT::NodeStatus IsServiceFlowInput();
    BT::NodeStatus IsContainCollaborativeSchedulingModel();
    BT::NodeStatus IsParseServiceFlowCompleted();
    BT::NodeStatus PrintServiceContent();
    BT::NodeStatus IsServiceSchedulingEnvironmentDeployedSuccessfully();
    BT::NodeStatus RunningCollaborativeScheduling();
    BT::NodeStatus IsContainResourceAwarenessModel();
    BT::NodeStatus IsCurrentResourcesCanSupportServiceCompletion();
    BT::NodeStatus PrintCurrentResources();
    BT::NodeStatus WhatResourcesAreNeeded();
    BT::NodeStatus PrintRequiredResourceName();
    BT::NodeStatus IsOpenResourceAwarenessArchitecture();
    BT::NodeStatus RunningResourceAwareness();
    BT::NodeStatus IsContainServiceMigrationModel();
    BT::NodeStatus PrintCurrentResourcesInformation();
    BT::NodeStatus IsCurrentResourceExceedstheThreshold();
    BT::NodeStatus RunningServiceMigration();
    BT::NodeStatus PrintMigrationPath();

    //----------------------------end



    //BT::NodeStatus IsDoorOpen();

    //BT::NodeStatus IsDoorLocked();

    //BT::NodeStatus UnlockDoor();

    //BT::NodeStatus PassThroughDoor();

    //BT::NodeStatus PassThroughWindow();

    //BT::NodeStatus OpenDoor();

    //BT::NodeStatus CloseDoor();
    //beginning
    BT::NodeStatus IsIntentInput();

    BT::NodeStatus IsIntentNULL();
    //IntentModel
    BT::NodeStatus IsIntentModelFinish();

    BT::NodeStatus IsIntentModelWaiting();
    //    perception
    BT::NodeStatus IsIntentPerceptionFinish();

    BT::NodeStatus IsIntentPerceptionWaiting();

    BT::NodeStatus IntentPerceptionRunning();
    //    classifier
    BT::NodeStatus IsIntentTranslationFinish();

    BT::NodeStatus IsIntentTranslationWaiting();

    BT::NodeStatus IntentTranslationRunning();
    //    detection
    BT::NodeStatus IsIntentConfilctDetectionFinish();

    BT::NodeStatus IsIntentConfilctDetectionWaiting();

    BT::NodeStatus ConfilctDetectionRunning();
    //ActionModel
    BT::NodeStatus IsActionModelFinish();

    BT::NodeStatus IsActionModelWaiting();
    //    Mapping
    BT::NodeStatus IsIntentPolicyMappingFinish();

    BT::NodeStatus IsIntentPolicyMappingWaiting();

    BT::NodeStatus IntentPolicyMappingRunning();
    //    Formulation
    BT::NodeStatus IsPolicyRefinementFinish();

    BT::NodeStatus IsPolicyRefinementWaiting();

    BT::NodeStatus PolicyRefinementRunning();
    //    Detection
    BT::NodeStatus IsPolicyConfilctDetectionFinish();

    BT::NodeStatus IsPolicyConfilctDetectionWaiting();

    BT::NodeStatus PolicyConfilctDetectionRunning();
    //StateModel
    BT::NodeStatus IsStateModelFinish();

    BT::NodeStatus IsStateModelWaiting();
    //    DeploymentAndVerification
    BT::NodeStatus IsPolicyDeploymentAndVerificationFinish();

    BT::NodeStatus IsPolicyDeploymentAndVerificationWaiting();

    BT::NodeStatus PolicyDeploymentAndVerificationRunning();
    //    Feedback
    BT::NodeStatus IsCurrentSituationFeedback();

    BT::NodeStatus DisplayCurrentNetworkSituation();

    void RegisterNodes(BT::BehaviorTreeFactory& factory);
}
