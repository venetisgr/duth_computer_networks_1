/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer n0n1;
  n0n1.Create (2);

  NodeContainer n1n2;
  n1n2.Add (n0n1.Get (1));
  n1n2.Create (1);

  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("41ms"));

  NetDeviceContainer dev0 = pointToPoint1.Install (n0n1);
  NetDeviceContainer dev1 = pointToPoint1.Install (n1n2);

  InternetStackHelper stack;
  stack.Install (n0n1.Get(0));
  stack.Install (n1n2);

  Ipv4AddressHelper adress;
  adress.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer adn0n1;
  adn0n1 = adress.Assign (dev0);
  adress.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer adn1n2;
  adn1n2 = adress.Assign (dev1);


  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (n0n1.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (adn0n1.GetAddress (0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  //echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (8192));

  ApplicationContainer clientApps = echoClient.Install (n1n2.Get (1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop(Seconds(11.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
