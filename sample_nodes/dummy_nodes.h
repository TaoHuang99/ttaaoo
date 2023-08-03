#ifndef SIMPLE_BT_NODES_H
#define SIMPLE_BT_NODES_H

#include <string>
#include <hiredis/hiredis.h>
#include <iostream>
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"

#define MAX_VERtEX_NUM 30
#define INFINITY 65535
//增加新接口的函数
//
// void distribution_cmd(std::string dst_ip);
void migrateKey(const std::string& sourceHost, int sourcePort,
                const std::string& targetHost, int targetPort,
                const std::string& key, const std::string& password);

void migrateAllKeys(const std::string& sourceHost, int sourcePort,
                    const std::string& targetHost, int targetPort,
                    const std::string& password);
namespace DummyNodes
{

struct graph {
    int vertex[MAX_VERtEX_NUM];
    int edge[MAX_VERtEX_NUM][MAX_VERtEX_NUM];
    int vexnum, edgenum;
    char ip[MAX_VERtEX_NUM][20];
};

typedef int PathMatrix[MAX_VERtEX_NUM];     //用于存储最短路径中经过的顶点的下标
typedef int ShortPathTable[MAX_VERtEX_NUM]; //用于存储各个最短路径的权值和

// typedef char ip[MAX_VERtEX_NUM][20];   //用于存储各个节点的ip

struct localhost {
  char ip[20];
	float computing;
	float store;
};

struct node {
    struct node *next;
    struct node *prior;
    struct localhost value;
    float benefit;
    int cost;
    float gain;
};

struct global {
    struct node *head;
    struct node *tail;
    int count;
};


using BT::NodeStatus;

NodeStatus CheckIntent();
NodeStatus CheckTranslate();

NodeStatus CheckBattery();

NodeStatus CheckTemperature();
NodeStatus SayHello();

class IntentInterface
{
  public:
    IntentInterface() : _intent(true)
    {
    }

    NodeStatus input();

    NodeStatus translate();

    // NodeStatus close();

  private:
    bool _intent;
};

// class GraphInterface
// {
//   public:
//     GraphInterface() : _graph(true)
//     {
//     }

//     // NodeStatus input();

//     // NodeStatus translate();

//     // NodeStatus close();

//   private:
//     bool _graph;
//     // graph G;
//     ip I;
//     PathMatrix P;
//     ShortPathTable D;

// };

// class PolicyInterface
// {
//   public:
//     PolicyInterface() : _policy(true)
//     {
//     }

//     NodeStatus pathcal();

//     NodeStatus migration();

//     NodeStatus distribution();

//     NodeStatus  policylevel1();

//     NodeStatus  policylevel2();

//     NodeStatus  policylevel3();

//   private:
//     bool _policy;
//     PathMatrix *_P;
//     ShortPathTable *_D;

// };

class PolicyInterface
{
  // friend class PolicyInterface;

  public:
    //添加迁移判决类成员
    int get_imbalance();
    int get_overload_frequency();
    //分隔
    PolicyInterface() : _policy(false) {}

    NodeStatus getsource();

    NodeStatus get_local_resource();

    void getgraph();

    NodeStatus migration_jude();

    NodeStatus checksource();

    NodeStatus checklevel3();

    NodeStatus checklevel2();

    NodeStatus checklevel1();



    NodeStatus pathcal();

    NodeStatus migration();

    NodeStatus distribution();

    NodeStatus  policylevel1();

    NodeStatus  policylevel2();

    NodeStatus  policylevel3();

    int _sockfd;

    struct node *_best;

    bool _policy = false;

    bool _distribute = false;

    bool _resource = false;

  private:
    // bool _source;

    struct localhost *_localhost;

    struct global *_global;

    struct graph *_graph;

    // bool _policy;

    PathMatrix *_P;

    ShortPathTable *_D;

    // struct node *_best;
    
    // int _sockfd;

};

class GripperInterface
{
  public:
    GripperInterface() : _opened(true)
    {
    }

    NodeStatus open();

    NodeStatus close();

  private:
    bool _opened;
};

//--------------------------------------

// Example of custom SyncActionNode (synchronous action)
// without ports.
class ApproachObject : public BT::SyncActionNode
{
  public:
    ApproachObject(const std::string& name) :
        BT::SyncActionNode(name, {})
    {
    }

    // You must override the virtual function tick()
    NodeStatus tick() override;
};

// Example of custom SyncActionNode (synchronous action)
// with an input port.
class SaySomething : public BT::SyncActionNode
{
  public:
    SaySomething(const std::string& name, const BT::NodeConfiguration& config)
      : BT::SyncActionNode(name, config)
    {
    }

    // You must override the virtual function tick()
    NodeStatus tick() override;

    // It is mandatory to define this static method.
    static BT::PortsList providedPorts()
    {
        return{ BT::InputPort<std::string>("message") };
    }
};

//Same as class SaySomething, but to be registered with SimpleActionNode
NodeStatus SaySomethingSimple(BT::TreeNode& self);

// Example os Asynchronous node that use StatefulActionNode as base class
class SleepNode : public BT::StatefulActionNode
{
  public:
    SleepNode(const std::string& name, const BT::NodeConfiguration& config)
      : BT::StatefulActionNode(name, config)
    {}

    static BT::PortsList providedPorts()
    {
        // amount of milliseconds that we want to sleep
        return{ BT::InputPort<int>("msec") };
    }

    NodeStatus onStart() override
    {
        int msec = 0;
        getInput("msec", msec);
        if( msec <= 0 )
        {
            // no need to go into the RUNNING state
            return NodeStatus::SUCCESS;
        }
        else {
            using namespace std::chrono;
            // once the deadline is reached, we will return SUCCESS.
            deadline_ = system_clock::now() + milliseconds(msec);
            return NodeStatus::RUNNING;
        }
    }

    /// method invoked by an action in the RUNNING state.
    NodeStatus onRunning() override
    {
        if ( std::chrono::system_clock::now() >= deadline_ )
        {
            return NodeStatus::SUCCESS;
        }
        else {
            return NodeStatus::RUNNING;
        }
    }

    void onHalted() override
    {
        // nothing to do here...
        std::cout << "SleepNode interrupted" << std::endl;
    }

  private:
    std::chrono::system_clock::time_point deadline_;
};

inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
{
    static GripperInterface grip_singleton;

    factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));
    factory.registerSimpleCondition("CheckTemperature", std::bind(CheckTemperature));
    factory.registerSimpleAction("SayHello", std::bind(SayHello));
    factory.registerSimpleAction("OpenGripper", std::bind(&GripperInterface::open, &grip_singleton));
    factory.registerSimpleAction("CloseGripper", std::bind(&GripperInterface::close, &grip_singleton));
    factory.registerNodeType<ApproachObject>("ApproachObject");
    factory.registerNodeType<SaySomething>("SaySomething");
}

} // end namespace

#endif   // SIMPLE_BT_NODES_H
