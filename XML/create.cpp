// #include <iostream>
// #include <fstream>
// #include <regex>
// #include <string>
// #include "tinyxml2.h"
// #include "tinyxml2.cpp"

#include "create.h"

// using namespace std;
// using namespace tinyxml2;

// #define CollaborativeScheduling 1
// #define ResourceAwareness       10
// #define ServiceMigration        100


int createXML(const char * xmlPath,int status_num)
{
  int type;
  XMLDocument doc;
  if(XML_ERROR_FILE_NOT_FOUND != doc.LoadFile(xmlPath))
  {
    cout<<"file is exits！"<<endl;
    return 0;
  }
  //1.添加声明 方法一
  //const char* declaration ="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
  //doc.Parse(declaration);//会覆盖xml所有内容
  //2.添加声明 方法二
  //<?xml version="1.0" encoding="UTF-8"?>
  //XMLDeclaration *declaration = doc.NewDeclaration();
  //doc.InsertFirstChild(declaration);
  //3.新建root根节点
  /***************************************************************/
  XMLElement *root = doc.NewElement("root");
  doc.InsertFirstChild(root);
  doc.InsertEndChild(root);
  //4.给root添加节点
  XMLElement* userNode = doc.NewElement("BehaviorTree");
  userNode->SetAttribute("ID","MainTree");
  root->InsertEndChild(userNode);

  XMLElement* userNode1 = doc.NewElement("Fallback");
  userNode1->SetAttribute("name","root_Fallback");
  userNode->InsertEndChild(userNode1);

  XMLElement* userNode2 = doc.NewElement("Condition");
  userNode2->SetAttribute("ID","IsServiceFlowInput");
  userNode1->InsertEndChild(userNode2);

  XMLElement* userNode3 = doc.NewElement("Sequence");
  userNode3->SetAttribute("name","BT_process_sequence");
  userNode1->InsertEndChild(userNode3);

  

/************************Collaborative**Scheduling**Model************************/
  type = status_num % 10;
  if (type)
  {
    XMLElement* userNode4 = doc.NewElement("Fallback");
    userNode4->SetAttribute("name","Sub1_Fallback");
    userNode3->InsertEndChild(userNode4);

    XMLElement* userNode5 = doc.NewElement("Condition");
    userNode5->SetAttribute("ID","IsContainCollaborativeSchedulingModel");
    userNode4->InsertEndChild(userNode5);

    XMLElement* userNode6 = doc.NewElement("Sequence");
    userNode6->SetAttribute("name","Collaborative Scheduling Sequence 1");
    userNode4->InsertEndChild(userNode6);

    XMLElement* userNode7 = doc.NewElement("Condition");
    userNode7->SetAttribute("ID","IsParseServiceFlowCompleted");
    userNode6->InsertEndChild(userNode7);

    XMLElement* userNode8 = doc.NewElement("Action");
    userNode8->SetAttribute("ID","PrintServiceContent");
    userNode6->InsertEndChild(userNode8);

    XMLElement* userNode9 = doc.NewElement("Sequence");
    userNode9->SetAttribute("name","Collaborative Scheduling Sequence 2");
    userNode6->InsertEndChild(userNode9);

    XMLElement* userNode10 = doc.NewElement("Condition");
    userNode10->SetAttribute("ID","IsServiceSchedulingEnvironmentDeployedSuccessfully");
    userNode9->InsertEndChild(userNode10);

    XMLElement* userNode11 = doc.NewElement("Action");
    userNode11->SetAttribute("ID","RunningCollaborativeScheduling");
    userNode9->InsertEndChild(userNode11);
  }
  status_num /= 10;

  /*************************Resource**Awareness**Model*****************************/
  type = status_num % 10;
  if (type)
  {
    XMLElement* userNode12 = doc.NewElement("Fallback");
    userNode12->SetAttribute("name","Sub2_Fallback");
    userNode3->InsertEndChild(userNode12);

    XMLElement* userNode13 = doc.NewElement("Condition");
    userNode13->SetAttribute("ID","IsContainResourceAwarenessModel");
    userNode12->InsertEndChild(userNode13);

    XMLElement* userNode14 = doc.NewElement("Sequence");
    userNode14->SetAttribute("name","Resource Awareness Sequence 1");
    userNode12->InsertEndChild(userNode14);

    XMLElement* userNode15 = doc.NewElement("Condition");
    userNode15->SetAttribute("ID","IsCurrentResourcesCanSupportServiceCompletion");
    userNode14->InsertEndChild(userNode15);

    XMLElement* userNode16 = doc.NewElement("Action");
    userNode16->SetAttribute("ID","PrintCurrentResources");
    userNode14->InsertEndChild(userNode16);

    XMLElement* userNode17 = doc.NewElement("Condition");
    userNode17->SetAttribute("ID","WhatResourcesAreNeeded");
    userNode14->InsertEndChild(userNode17);

    XMLElement* userNode18 = doc.NewElement("Action");
    userNode18->SetAttribute("ID","PrintRequiredResourceName");
    userNode14->InsertEndChild(userNode18);

    XMLElement* userNode19 = doc.NewElement("Sequence");
    userNode19->SetAttribute("name","Resource Awareness Sequence 2");
    userNode14->InsertEndChild(userNode19);

    XMLElement* userNode20 = doc.NewElement("Condition");
    userNode20->SetAttribute("ID","IsOpenResourceAwarenessArchitecture");
    userNode19->InsertEndChild(userNode20);

    XMLElement* userNode21 = doc.NewElement("Action");
    userNode21->SetAttribute("ID","RunningResourceAwareness");
    userNode19->InsertEndChild(userNode21);
  }
  status_num /= 10;

  /*************************Service**Migration**Model***************************/
  type = status_num % 10;
  if (type)
  {
    XMLElement* userNode22 = doc.NewElement("Fallback");
    userNode22->SetAttribute("name","Sub3_Fallback");
    userNode3->InsertEndChild(userNode22);

    XMLElement* userNode23 = doc.NewElement("Condition");
    userNode23->SetAttribute("ID","IsContainServiceMigrationModel");
    userNode22->InsertEndChild(userNode23);

    XMLElement* userNode24 = doc.NewElement("Sequence");
    userNode24->SetAttribute("name","Service Migration Sequence 1");
    userNode22->InsertEndChild(userNode24);

    XMLElement* userNode25 = doc.NewElement("Action");
    userNode25->SetAttribute("ID","PrintCurrentResourcesInformation");
    userNode24->InsertEndChild(userNode25);

    XMLElement* userNode26 = doc.NewElement("Condition");
    userNode26->SetAttribute("ID","IsCurrentResourceExceedstheThreshold");
    userNode24->InsertEndChild(userNode26);

    XMLElement* userNode27 = doc.NewElement("Sequence");
    userNode27->SetAttribute("name","Service Migration Sequence 2");
    userNode24->InsertEndChild(userNode27);

    XMLElement* userNode28 = doc.NewElement("Action");
    userNode28->SetAttribute("ID","RunningServiceMigration");
    userNode27->InsertEndChild(userNode28);

    XMLElement* userNode29 = doc.NewElement("Action");
    userNode29->SetAttribute("ID","PrintMigrationPath");
    userNode27->InsertEndChild(userNode29);
  }
  status_num /= 10;

  return doc.SaveFile(xmlPath);
}
int addXML(const char *xmlPath)
{
  XMLDocument doc;
  if( XML_SUCCESS != doc.LoadFile(xmlPath))
  {
    cout<<"load xml file failed"<<endl;
    return -1;
  }
  //再添加一个没有属性的name = MenAngel的 person节点
  XMLElement *newPerson = doc.NewElement("person");
  newPerson->InsertFirstChild(doc.NewText("MenAngel"));
  XMLElement *root = doc.RootElement();
  root->InsertEndChild(newPerson);
  return doc.SaveFile(xmlPath);
}
int modifyXML(const char *xmlPath)
{
  XMLDocument doc;
  if( XML_SUCCESS != doc.LoadFile(xmlPath))
  {
    cout<<"load xml file failed"<<endl;
    return -1;
  }
  //1.获得根节点
  XMLElement *root = doc.RootElement();
  //2.获得根节点root指定名称的第一个子元素
  //获得根节点第一个子元素 root->FirstChild();
  XMLElement *newPerson = root->FirstChildElement("person");
  newPerson->SetAttribute("name","user");
  newPerson->SetText("MenAngel");
  newPerson->DeleteAttribute("passwd");
  //3.查询属性及值
  cout<<"<person> name = "<<newPerson->Attribute("name")<<" value = "<<newPerson->GetText()<<endl;
  newPerson = newPerson->NextSiblingElement();
  root->DeleteChild(newPerson);
  return doc.SaveFile(xmlPath);
}
// int main(int argv,char *argc[])
// {
//   int status_num = 0;
//   /*******************read txt******************/
//   string path = "./runtime.txt";
//   ifstream in_file(path, ios::in); //按照指定mode打开文件

//   if (in_file.is_open()) { // 或者if (in_file)
//     cout << "Open File Successfully" << endl;
//     string line;
//     while(getline(in_file, line)) 
//     {
//  	  cout << line << endl;
//       // 定义匹配模式
//       regex re("\"([^\"]*)\", \"([^\"]*)\", \"([^\"]*)\"");

//       // 进行匹配
//       smatch match;
//       if (regex_search(line, match, re) && match.size() > 3) 
//       {
//           string third_elem = match.str(3);
//           if (third_elem == "CollaborativeScheduling")
//           {
//             status_num = status_num + CollaborativeScheduling;
//           }
//           if (third_elem == "ResourceAwareness")
//           {
//             status_num = status_num + ResourceAwareness; 
//           }
//           if (third_elem == "ServiceMigration")
//           {
//             status_num = status_num + ServiceMigration;
//           }
//           cout << "Third element: " << third_elem << std::endl;
//           cout << "status_num: " << status_num << std::endl;
//       } else {
//           cerr << "No match found!" << endl;
//       }

// 	}		
//   } else {
// 	  cout << "Cannot Open File: " << path << endl;
//     getchar();
//   }
//   in_file.close(); // 关闭与in_file绑定的文件，返回void

//   /************************************************/
  
//   const char * filename = "create.xml";
//   int iRet = createXML(filename,status_num);
//   if(iRet != XML_SUCCESS)
//     cout<<"create xml fail!"<<endl;
//   //iRet = addXML(filename);
//   if(iRet != XML_SUCCESS)
//     cout<<"add xml fail!"<<endl;
//   //iRet = modifyXML(filename);
//   if(iRet != XML_SUCCESS)
//     cout<<"modify or delte xml fail!"<<endl;
//   return iRet;
// }