#include "Mesh.h"
#include "Global.h"

#include "painlessMesh.h"



static painlessMesh  gMesh;

Mesh::Mesh()
{

}
void Mesh::Init()
{
    gMesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
    
    gMesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
    gMesh.onReceive([this](uint32_t from, String &msg){this->OnReceived(from, msg);});
    gMesh.onNewConnection([this](uint32_t nodeId){this->OnNewConnection(nodeId);});
    gMesh.onChangedConnections([this](){this->OnChangedConnection();});
    gMesh.onNodeTimeAdjusted([this](int32_t offset){this->OnNodeTimeAdjusted(offset);});
}

void Mesh::Update()
{
    gMesh.update();
}

uint32_t Mesh::GetTimeMs()
{
    uint32_t node_time_us = gMesh.getNodeTime();
    uint32_t node_time_ms = node_time_us / 1000ul;
    return node_time_ms;
}

void Mesh::OnReceived( uint32_t from, String &msg )
{

}

void Mesh::OnNewConnection(uint32_t nodeId)
{

}

void Mesh::OnChangedConnection()
{

}

void Mesh::OnNodeTimeAdjusted(int32_t offset)
{

}