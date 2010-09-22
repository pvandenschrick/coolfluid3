// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <QtCore>
#include <QtTest>

#include "Common/XmlHelpers.hpp"

#include "GUI/Client/Core/ClientRoot.hpp"
#include "GUI/Client/Core/CNode.hpp"
#include "GUI/Client/Core/NBrowser.hpp"
#include "GUI/Client/Core/NGroup.hpp"
#include "GUI/Client/Core/NLog.hpp"
#include "GUI/Client/Core/NLink.hpp"
#include "GUI/Client/Core/NMesh.hpp"
#include "GUI/Client/Core/NMethod.hpp"
#include "GUI/Client/Core/NRoot.hpp"
#include "GUI/Client/Core/NTree.hpp"

#include "GUI/Client/uTests/CommonFunctions.hpp"
#include "GUI/Client/uTests/ExceptionThrowHandler.hpp"
#include "GUI/Client/uTests/MyNode.hpp"
#include "GUI/Client/uTests/TreeHandler.hpp"

#include "GUI/Client/uTests/CNodeTest.hpp"

using namespace CF::Common;
using namespace CF::GUI::Client;
using namespace CF::GUI::ClientTest;

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_getComponentType()
{
  MyNode node("Node");

  QCOMPARE(node.getComponentType(), QString("MyNode"));
}

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_isClientComponent()
{
  MyNode node("Node");
  NBrowser browser;
  NGroup group("Group");
  NLink link("Link");
  NLog log;
  NMesh mesh("Mesh");
  NMethod method("Method");
  NRoot root("Root");
  NTree tree;

  QVERIFY(browser.isClientComponent());
  QVERIFY(!group.isClientComponent());
  QVERIFY(!link.isClientComponent());
  QVERIFY(log.isClientComponent());
  QVERIFY(!mesh.isClientComponent());
  QVERIFY(!method.isClientComponent());
  QVERIFY(!root.isClientComponent());
  QVERIFY(!node.isClientComponent());
  QVERIFY(tree.isClientComponent());
}

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_getType()
{
  MyNode node("Node");
  NBrowser browser;
  NLog log;

  QCOMPARE(node.getType(), CNode::GROUP_NODE);
  QCOMPARE(browser.getType(), CNode::BROWSER_NODE);
  QCOMPARE(log.getType(), CNode::LOG_NODE);
}

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_checkType()
{
  NMesh mesh("Mesh");
  NMethod method("Method");
  NRoot root("Root");
  NTree tree;

  QVERIFY(mesh.checkType(CNode::MESH_NODE));
  QVERIFY(!mesh.checkType(CNode::GROUP_NODE));
  QVERIFY(!mesh.checkType(CNode::LINK_NODE));
  QVERIFY(method.checkType(CNode::METHOD_NODE));
  QVERIFY(root.checkType(CNode::ROOT_NODE));
  QVERIFY(tree.checkType(CNode::TREE_NODE));
}

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_setOptions()
{
  MyNode node("Node");

  boost::shared_ptr<XmlDoc> wrongOpt = XmlOps::parse(std::string("<valuemap>"
      "<value key=\"options\">"
      " <valuemap>"
      " 	<value key=\"pi\" descr=\"Pi value\">"
      "   	<real>3.141592</real>"
      "  </value>"
      "	 <value key=\"fakePi\" descr=\"Pi value in an unknown type\">"
      "   	<type>3.141592</type>"
      "  </value>"
      " </valuemap>"
      "</value>"
      "</valuemap>"));

  boost::shared_ptr<XmlDoc> correctOpt = XmlOps::parse(std::string("<valuemap>"
      "<value key=\"options\">"
      " <valuemap>"
      " 	<value key=\"pi\" descr=\"Pi value\">"
      "   	<real>3.141592</real>"
      "  </value>"
      "	 <value key=\"fakePi\" descr=\"Pi value in an unknown type\">"
      "   	<string>Hello world!</string>"
      "  </value>"
      " </valuemap>"
      "</value>"
      "</valuemap>"));


  GUI_CHECK_THROW(MyNode("Node").setOptions(*wrongOpt->first_node()), ShouldNotBeHere);

  GUI_CHECK_NO_THROW(node.setOptions(*correctOpt->first_node()));
}

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_getOptions()
{
  TreeHandler th;
  MyNode::Ptr node(new MyNode("Node"));

  QList<NodeOption> nodeOptList;

  boost::shared_ptr<XmlDoc> options = XmlOps::parse(std::string("<valuemap>"
      "<value key=\"options\">"
      " <valuemap>"
      " 	<value key=\"pi\" descr=\"Pi value\">"
      "   	<real>3.141592</real>"
      "  </value>"
      "	 <value key=\"hello\" descr=\"Some bool\">"
      "   	<bool>false</bool>"
      "  </value>"
      " </valuemap>"
      "</value>"
      "</valuemap>"));

  th.add(node);

  node->setOptions(*options->first_node());

  GUI_CHECK_NO_THROW(node->getOptions(nodeOptList));

  // there were already 2 options in MyNode + 2 new options => 4 options
  QCOMPARE(nodeOptList.count(), 4);
}

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_createFromXml()
{
  CNode::Ptr node;
  NRoot::Ptr root;
  NGroup::Ptr group;
  QList<NodeOption> optList;
  NodeOption option;
  boost::shared_ptr<XmlDoc> tree = XmlOps::parse(
      std::string("<CRoot name=\"Simulator\">"
                  "  <SomeComponent name=\"Flow\">" // comp. type does not exist
                  "    <valuemap>"
                  "      <value key=\"options\">"
                  "        <valuemap>"
                  "          <value key=\"pi\" descr=\"Pi in a CGroup\">"
                  "            <real>3.1415920000000002</real>"
                  "          </value>"
                  "        </valuemap>"
                  "      </value>"
                  "    </valuemap>"
                  "  </SomeComponent>"
                  "</CGroup>"));

 // GUI_CHECK_NO_THROW(doc = XmlOps::parse(boost::filesystem::path("./tree.xml")));
  GUI_CHECK_NO_THROW(root = makeTreeFromFile());
  GUI_CHECK_THROW(CNode::createFromXml(*tree->first_node())->convertTo<NRoot>(), XmlError);

  GUI_CHECK_NO_THROW(node = boost::dynamic_pointer_cast<CNode>(root->root()->get_child("Flow")));
  GUI_CHECK_NO_THROW(group = node->convertTo<NGroup>());

  GUI_CHECK_NO_THROW(group->getOptions(optList));

  QCOMPARE(optList.count(), 1);

  option = optList.at(0);

  QCOMPARE(option.m_paramAdv, true);
  QCOMPARE(option.m_paramName, QString("pi"));
  QCOMPARE(option.m_paramDescr, QString("Pi in a CGroup"));
  QCOMPARE(option.m_paramValue, QString("3.14159"));
  QCOMPARE(option.m_paramType, OptionType::TYPE_DOUBLE);
}

///////////////////////////////////////////////////////////////////////////

void CNodeTest::test_addNode()
{
  NRoot::Ptr root(new NRoot("Root"));
  NLog::Ptr log(new NLog());
  QSignalSpy spy(root->getNotifier(), SIGNAL(childCountChanged()));

  GUI_CHECK_NO_THROW(root->addNode(log));

  QCOMPARE(spy.count(), 1);

  spy.clear();

  GUI_CHECK_THROW(root->addNode(log), ValueExists); // try to add the log again

  QCOMPARE(spy.count(), 0);
}

//////////////////////////////////////////////////////////////////////////

void CNodeTest::test_removeNode()
{

}
