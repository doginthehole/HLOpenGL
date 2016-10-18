//
//  PolyDataReceiver.cpp
//  Hololens polydata receiver
//
//  Created by Longquan Chen on 8/22/16.
//
//

#include <stdio.h>

#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>
//OpenIGTLink Include
#include "igtlOSUtil.h"
#include "igtlClientSocket.h"
#include "igtlMessageHeader.h"
#include "igtlPolyDataMessage.h"
#include "igtlTransformMessage.h"
#include "igtlMultiThreader.h"
#include "igtlConditionVariable.h"
#include "igtlTimeStamp.h"

bool interactionActive;
igtl::ConditionVariable::Pointer conditionVar;
igtl::SimpleMutexLock * localMutex;
int frameNum = 0;
igtl::PolyDataPointArray::Pointer pointsArray;
igtl::PolyDataCellArray::Pointer polygonsArray;
igtl::PolyDataAttribute::Pointer normArray;
igtlFloat32 colors[4] = { NULL };
bool receivedData = false;

bool ReceivePolyDataStream(igtl::Socket * socket, igtl::MessageHeader::Pointer header)
{
	//std::cerr << "Receiving TRANSFORM data type. " << header->GetDeviceName()<< std::endl;

	// Create a message buffer to receive transform data
	igtl::MessageBase::Pointer buffer = igtl::MessageBase::New();
	buffer->SetMessageHeader(header);
	buffer->AllocatePack();
	// Receive transform data from the socket
	//igtl::TimeStamp::Pointer timer = igtl::TimeStamp::New();
	//long timePre = timer->GetTimeStampInNanoseconds();
	int read = socket->Receive(buffer->GetPackBodyPointer(), buffer->GetPackBodySize());
	std::cerr << " read " << buffer->GetPackBodySize() << std::endl;
	if (read != buffer->GetPackBodySize())
	{
		std::cerr << "Only read " << read << " but expected to read "
			<< buffer->GetPackBodySize() << "\n";
	}
	// Create a message buffer to receive image data
	igtl::PolyDataMessage::Pointer polyDataMsg;
	polyDataMsg = igtl::PolyDataMessage::New();
	polyDataMsg->Copy(buffer); // !! TODO: copy makes performance issue.

							   // Deserialize the data
							   // If CheckCRC==0, CRC check is skipped.
	int c = polyDataMsg->Unpack(0);

	if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
	{
		std::cerr << "Unable to create MRML node from incoming POLYDATA message. Failed to unpack the message";
		return 0;
	}

	// Points
	pointsArray = polyDataMsg->GetPoints();
	int npoints = pointsArray->GetNumberOfPoints();
	if (npoints > 0)
	{
		for (int i = 0; i < npoints; i++)
		{
			igtlFloat32 point[3];
			pointsArray->GetPoint(i, point);
		}
	}
	else
	{
		// ERROR: No points defined
	}

	//// Vertices
	//igtl::PolyDataCellArray::Pointer verticesArray =  polyDataMsg->GetVertices();
	//int nvertices = verticesArray.IsNotNull() ? verticesArray->GetNumberOfCells() : 0;
	//if (nvertices > 0)
	//{
	//  for (int i = 0; i < nvertices; i ++)
	//  {
	//glClearColor(1.0f, 1.f, 0.f, 0.f);
	//    std::list<igtlUint32> cell;
	//    verticesArray->GetCell(i, cell);
	//    std::list<igtlUint32>::iterator iter;
	//    iter = cell.begin();
	//  }
	//}
	//
	//// Lines

	//igtl::PolyDataCellArray::Pointer linesArray = polyDataMsg->GetNumberOfAttributes();
	
	// Normals
	int nAttribute = polyDataMsg->GetNumberOfAttributes();
	if (nAttribute > 0)
	{
	  for(int i = 0; i < nAttribute; i++)
	  {
	    std::list<igtlUint32> cell;
		normArray = polyDataMsg->GetAttribute(i);
	    int normSize = normArray->GetSize();
		igtlFloat32 * data = new igtlFloat32[3];
	    for (int j = 0; j< normSize; j++)
	    {
			normArray->GetNthData(j, data);
	    }
	  }
	}

	// Polygons
	polygonsArray = polyDataMsg->GetPolygons();
	int npolygons = polygonsArray.IsNotNull() ? polygonsArray->GetNumberOfCells() : 0;
	//int npolygons = polygonsArray->GetNumberOfCells();
	if (npolygons > 0)
	{
		for (int i = 0; i < npolygons; i++)
		{
			std::list<igtlUint32> cell;
			polygonsArray->GetCell(i, cell);
			std::list<igtlUint32>::iterator iter;
			int j = 0;
			for (iter = cell.begin(); iter != cell.end(); iter++)
			{
				j++;
			}
		}
	}
	
	unsigned int nAttr = polyDataMsg->GetNumberOfAttributes();
	
	for (unsigned int i = 0; i < nAttr; i++)
	{
		std::cerr << "  ------ Attributes #" << i << " ------" << std::endl;
		igtl::PolyDataAttribute * p = polyDataMsg->GetAttribute(i);
		if (p)
		{
			std::cerr << "  Name = " << p->GetName() << std::endl;
			std::cerr << "  Type = ";
			switch (p->GetType())
			{
			case igtl::PolyDataAttribute::POINT_SCALAR:
				std::cerr << "POINT_SCALAR" << std::endl;
				break;
			case igtl::PolyDataAttribute::POINT_VECTOR:
				std::cerr << "POINT_VECTOR" << std::endl;
				break;
			case igtl::PolyDataAttribute::POINT_NORMAL:
				std::cerr << "POINT_NORMAL" << std::endl;
				break;
			case igtl::PolyDataAttribute::POINT_TENSOR:
				std::cerr << "POINT_TENSOR" << std::endl;
				break;
			case igtl::PolyDataAttribute::POINT_RGBA:
				///storing the color value here.
				p->GetNthData(i, colors);
				std::cerr << "POINT_RGBA" << std::endl;
				break;
			case igtl::PolyDataAttribute::CELL_SCALAR:
				std::cerr << "CELL_SCALAR" << std::endl;
				break;
			case igtl::PolyDataAttribute::CELL_VECTOR:
				std::cerr << "CELL_VECTOR" << std::endl;
				break;
			case igtl::PolyDataAttribute::CELL_NORMAL:
				std::cerr << "CELL_NORMAL" << std::endl;
				break;
			case igtl::PolyDataAttribute::CELL_TENSOR:
				std::cerr << "CELL_TENSOR" << std::endl;
				break;
			case igtl::PolyDataAttribute::CELL_RGBA:
				std::cerr << "CELL_RGBA" << std::endl;
				break;
			}
			unsigned int size = p->GetSize();
			unsigned int ncomp = p->GetNumberOfComponents();
			igtlFloat32 * data = new igtlFloat32[ncomp];
			for (unsigned int j = 0; j < size; j++)
			{
				p->GetNthData(j, data);
				std::cerr << "  data[" << j << "] = (" << data[0];
				for (unsigned int k = 1; k < ncomp; k++)
				{
					std::cerr << ", " << data[k];
				}
				std::cerr << ")" << std::endl;
			}
		}
	}
	
	// Triangle Strips
	//igtl::PolyDataCellArray::Pointer triangleStripsArray = polyDataMsg->GetTriangleStrips();
	//int ntstrips = triangleStripsArray.IsNotNull() ? triangleStripsArray->GetNumberOfCells() : 0;
	//if (ntstrips > 0)
	//{
	//  for(int i = 0; i < ntstrips; i++)
	//  {
	//    std::list<igtlUint32> cell;
	//    triangleStripsArray->GetCell(i, cell);
	//    std::list<igtlUint32>::iterator iter;
	//    int j = 0;
	//    for (iter = cell.begin(); iter != cell.end(); iter ++)
	//    {
	//      j++;
	//    }
	//  }
	//}

	// Points RGB
	//std::vector<igtlUint8> pointsRGB = polyDataMsg->GetPointsRGB();
	//int nPointsRGB = polyDataMsg->GetNumberOfPointsRGB();
	return true;

}


void ConnectionThread()
{
	char*  hostname =  "localhost"; //		10.238.135.57	//////////////////////////////////////
	int    port = 18944;			//
									//------------------------------------------------------------
									// Establish Connection

	igtl::ClientSocket::Pointer socket;
	socket = igtl::ClientSocket::New();
	int r = socket->ConnectToServer(hostname, port);

	if (r != 0)
	{
		std::cerr << "Cannot connect to the server." << std::endl;
		exit(0);
	}

	//------------------------------------------------------------
	// Create a message buffer to receive header
	igtl::MessageHeader::Pointer headerMsg;
	headerMsg = igtl::MessageHeader::New();

	//------------------------------------------------------------
	// Allocate a time stamp
	igtl::TimeStamp::Pointer ts;
	ts = igtl::TimeStamp::New();


	while (1)
	{
		// Initialize receive buffer
		headerMsg->InitPack();

		// Receive generic header from the socket
		int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
		if (r == 0)
		{
			socket->CloseSocket();
			exit(0);
		}
		if (r != headerMsg->GetPackSize())
		{
			continue;
		}

		// Deserialize the header
		headerMsg->Unpack();

		// Get time stamp
		igtlUint32 sec;
		igtlUint32 nanosec;

		headerMsg->GetTimeStamp(ts);
		ts->GetTimeStamp(&sec, &nanosec);


		// Check data type and receive data body
		if (strcmp(headerMsg->GetDeviceType(), "POLYDATA") == 0)
		{
			bool success = ReceivePolyDataStream(socket, headerMsg);
			if (success)
			{
				receivedData = true;
				conditionVar->Signal();
			}
		}
		else
		{
			std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
			socket->Skip(headerMsg->GetBodySizeToRead(), 0);
		}
	}

	//------------------------------------------------------------
	// Close connection (The example code never reaches this section ...)

	socket->CloseSocket();
}

int StartClient()
{
	conditionVar = igtl::ConditionVariable::New();
	localMutex = igtl::SimpleMutexLock::New();
	interactionActive = false;
	pointsArray = igtl::PolyDataPointArray::New();
	igtlFloat32 * point0 = new igtlFloat32[3];
	point0[0] = 5; point0[1] = 5; point0[2] = 5;
	//pointsArray->SetPoint(0, point0);
	// Create a mapper

	igtl::MultiThreader::Pointer threadViewer = igtl::MultiThreader::New();
	// Begin mouse interaction

	threadViewer->SpawnThread((igtl::ThreadFunctionType) &ConnectionThread, NULL);
	return 1;
}