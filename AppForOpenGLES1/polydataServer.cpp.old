#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>
//OpenIGTLink Include
#include "igtlOSUtil.h"
#include "igtlClientSocket.h"
#include "igtlServerSocket.h"
#include "igtlMessageHeader.h"
#include "igtlPolyDataMessage.h"
#include "igtlTransformMessage.h"
#include "igtlMultiThreader.h"
#include "igtlConditionVariable.h"
#include "igtlTimeStamp.h"
#include "vtkRenderer.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkPoints.h"
#include "vtkCell.h"
#include "vtkPolyDataNormals.h"
#include "pch.h"


bool interactionActive;
igtl::ConditionVariable::Pointer conditionVar;
igtl::SimpleMutexLock * localMutex;
int frameNum = 0;
igtl::PolyDataPointArray::Pointer pointsArray;
igtl::PolyDataCellArray::Pointer polygonsArray;
igtl::PolyDataAttribute::Pointer normArray;
bool receivedData = false;

int ReceivePolyData(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

	std::cerr << "Receiving POLYDATA data type." << std::endl;

	// Create a message buffer to receive transform data
	igtl::PolyDataMessage::Pointer PolyData;
	PolyData = igtl::PolyDataMessage::New();
	PolyData->SetMessageHeader(header);
	PolyData->AllocatePack();

	// Receive transform data from the socket
	socket->Receive(PolyData->GetPackBodyPointer(), PolyData->GetPackBodySize());

	// Deserialize the transform data
	// If you want to skip CRC check, call Unpack() without argument.
	int c = PolyData->Unpack();

	if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
	{
		igtl::PolyDataPointArray::Pointer pointsArray        = PolyData->GetPoints();
		igtl::PolyDataCellArray::Pointer verticesArray       = PolyData->GetVertices();
		igtl::PolyDataCellArray::Pointer linesArray          = PolyData->GetLines();
		igtl::PolyDataCellArray::Pointer polygonsArray       = PolyData->GetPolygons();
		igtl::PolyDataCellArray::Pointer triangleStripsArray = PolyData->GetTriangleStrips();

		std::cerr << "========== PolyData Contents ==========" << std::endl;
		if (pointsArray.IsNotNull())
		{
			std::cerr << "  ------ Points ------" << std::endl;
			for (unsigned int i = 0; i < pointsArray->GetNumberOfPoints(); i ++)
			{
				igtlFloat32 point[3];
				pointsArray->GetPoint(i, point);
				std::cerr << "  point[" << i << "] = (" << point[0] << ", "
						<< point[1] << ", " << point[2] << ")" << std::endl;
			}
		}

		if (verticesArray.IsNotNull())
		{
			std::cerr << "  ------ Vertices ------" << std::endl;
			for (unsigned int i = 0; i < verticesArray->GetNumberOfCells(); i ++)
			{
				std::list<igtlUint32> cell;
				verticesArray->GetCell(i, cell);
				std::list<igtlUint32>::iterator iter;
				iter = cell.begin();
				if (iter != cell.end())
				{
					std::cerr << "  cell[" << i << "] = (" << *iter;
					for (; iter != cell.end(); iter ++)
					{
						std::cerr << ", " << *iter;
					}
					std::cerr << ")" << std::endl;
				}
			}
		}

		if (linesArray.IsNotNull())
		{
			std::cerr << "  ------ Lines ------" << std::endl;
			for (unsigned int i = 0; i < linesArray->GetNumberOfCells(); i ++)
			{
				std::list<igtlUint32> cell;
				linesArray->GetCell(i, cell);
				std::list<igtlUint32>::iterator iter;
				iter = cell.begin();
				if (iter != cell.end())
				{
					std::cerr << "  cell[" << i << "] = (" << *iter;
					for (; iter != cell.end(); iter ++)
					{
						std::cerr << ", " << *iter;
					}
					std::cerr << ")" << std::endl;
				}
			}
		}

		if (polygonsArray.IsNotNull())
		{
			std::cerr << "  ------ Polygons ------" << std::endl;
			for (unsigned int i = 0; i < polygonsArray->GetNumberOfCells(); i ++)
			{
				std::list<igtlUint32> cell;
				polygonsArray->GetCell(i, cell);
				std::list<igtlUint32>::iterator iter;
				iter = cell.begin();
				if (iter != cell.end())
				{
					std::cerr << "  cell[" << i << "] = (" << *iter;
					for (; iter != cell.end(); iter ++)
					{
						std::cerr << ", " << *iter;
					}
					std::cerr << ")" << std::endl;
				}
			}
		}

		if (triangleStripsArray.IsNotNull())
		{
			std::cerr << "  ------ TriangleStrips ------" << std::endl;
			for (unsigned int i = 0; i < triangleStripsArray->GetNumberOfCells(); i ++)
			{
				std::list<igtlUint32> cell;
				triangleStripsArray->GetCell(i, cell);
				std::list<igtlUint32>::iterator iter;
				iter = cell.begin();
				if (iter != cell.end())
				{
					std::cerr << "  cell[" << i << "] = (" << *iter;
					for (; iter != cell.end(); iter ++)
					{
						std::cerr << ", " << *iter;
					}
					std::cerr << ")" << std::endl;
				}
			}
		}

		unsigned int nAttr = PolyData->GetNumberOfAttributes();

		for (unsigned int i = 0; i < nAttr; i ++)
		{
			std::cerr << "  ------ Attributes #" << i << " ------" << std::endl;
			igtl::PolyDataAttribute * p = PolyData->GetAttribute(i);
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
				unsigned int size  = p->GetSize();
				unsigned int ncomp = p->GetNumberOfComponents();
				igtlFloat32 * data = new igtlFloat32[ncomp];
				for (unsigned int j = 0; j < size; j ++)
				{
					p->GetNthData(j, data);
					std::cerr << "  data[" << j << "] = (" << data[0];
					for (unsigned int k = 1; k < ncomp; k ++)
					{
						std::cerr << ", " << data[k];
					}
					std::cerr << ")" << std::endl;
				}
			}
		}

		std::cerr << "================================" << std::endl;
		return 1;
	}

	return 0;

}


int ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader::Pointer& header)
{
	std::cerr << "Receiving TRANSFORM data type." << std::endl;

	// Create a message buffer to receive transform data
	igtl::TransformMessage::Pointer transMsg;
	transMsg = igtl::TransformMessage::New();
	transMsg->SetMessageHeader(header);
	transMsg->AllocatePack();

	// Receive transform data from the socket
	socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());

	// Deserialize the transform data
	// If you want to skip CRC check, call Unpack() without argument.
	int c = transMsg->Unpack();

	if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
	{
		// Retrive the transform data
		igtl::Matrix4x4 matrix;
		transMsg->GetMatrix(matrix);
		igtl::PrintMatrix(matrix);
		std::cerr << std::endl;
		return 1;
	}

	return 0;
}


void ConnectionThread()
{
	char*  hostname = "localhost"; //		10.22.178.162	/////////////////////////////////////////////////////
	int    port = 18944;			//
	//------------------------------------------------------------
	// Establish Connection

	igtl::ServerSocket::Pointer serverSocket;
	serverSocket = igtl::ServerSocket::New();
	int r = serverSocket->CreateServer(port);
	igtl::Socket::Pointer socket;

	if (r < 0)
	{
		std::cerr << "Cannot create a server socket." << std::endl;
		exit(0);
	}

	//------------------------------------------------------------
	// Create a message buffer to receive header
	igtl::MessageHeader::Pointer headerMsg;
	headerMsg = igtl::MessageHeader::New();
    socket = serverSocket->WaitForConnection(1000);



	//if (serverSocket.IsNotNull())
	if (socket.IsNotNull())
		{
		std::cout << "A client is connected." << std::endl;

		while (1)
		{
			// Initialize receive buffer
			headerMsg->InitPack();

			// Receive generic header from the socket
			int rs = serverSocket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());

			if (rs == 0)
			{
				std::cout << "Socket closing" << std::endl;
				socket->CloseSocket();
			}
			if (rs != headerMsg->GetPackSize())
			{
				continue;
			}

			std::cout << "I have received a packet" << std::endl;
			// Deserialize the header
			headerMsg->Unpack();
			// Check data type and receive data body
			std::cout << "Header message name - " << headerMsg->GetDeviceType() << std::endl;
			if (strcmp(headerMsg->GetDeviceType(), "POLYDATA") == 0)
			{
				std::cerr << "Received a GET_POLYDATA message." << std::endl;
				ReceivePolyData(socket, headerMsg);
			}
			else if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
			{
				ReceiveTransform(socket, headerMsg);
			}
			else
			{
				// if the data type is unknown, skip reading.
				std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
				ReceivePolyData(socket, headerMsg);
				socket->Skip(headerMsg->GetBodySizeToRead(), 0);
			}
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

int main()
{
	ConnectionThread();
}