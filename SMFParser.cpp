#include <stdlib.h>
#include "SMFParser.h"
#include <fstream>
using namespace std;

SMFParser::SMFParser()
{
}

SMFParser::~SMFParser()
{
}

bool SMFParser::Parser(const char* pFileName)
{
	m_VertexList.clear();
	m_FaceList.clear();

	fstream file(pFileName, ios::in);

	if(!file.is_open())
	{
		return false;
	}

	char buf[1024] = {0};

	while (file.getline(buf, 1024))
	{
		if (!ParserLine(buf))
		{
			return false;
		}
	}

	printf("Parser SMF File: \"%s\" successed\n", pFileName);
	return true;
}

bool SMFParser::ParserLine( const char* pLine )
{
	while (*pLine == ' ')
	{
		++pLine;
	}

	switch (*pLine)
	{
	case 'v':
	case 'V':
		return ParserVertex(pLine + 1);
	case 'f':
	case 'F':
		return ParserFace(pLine + 1);
	case '#':
		// comment
		//printf("Comment: %s\n", pLine);
		return true;
	default:
		//printf("unkonw: %s\n", pLine);
		return true;
	}

	return true;
}

bool SMFParser::ParserVertex( const char* pLine )
{
	//printf("Vertex: %s\n", pLine);

	float fValue[3] = {0.f};

	for (int i = 0; i < 3; ++i)
	{
		while (*pLine == ' ')
		{
			++pLine;
		}

		const char * pStart = pLine;
		const char * pEnd = pStart;
		while (*pLine != ' ' && *pLine != '\0')
		{
			++pLine;
		}
		pEnd = pLine;

		if (pEnd != pStart)
		{
			fValue[i] = static_cast<float>(atof(pStart));
			//printf("%f\t", fValue[i]);
		}
		else
		{
			if (i != 2)
			{
				//printf("\n");
				return false;
			}
		}
	}

	FVector3f vertex;
	vertex._x = fValue[0];
	vertex._y = fValue[1];
	vertex._z = fValue[2];
	m_VertexList.push_back(vertex);

	//printf("\n");
	return true;
}

bool SMFParser::ParserFace( const char* pLine )
{
	//printf("Face: %s\n", pLine);

	int iValue[3] = {0};

	for (int i = 0; i < 3; ++i)
	{
		while (*pLine == ' ')
		{
			++pLine;
		}

		const char * pStart = pLine;
		const char * pEnd = pStart;
		while (*pLine != ' ' && *pLine != '\0')
		{
			++pLine;
		}
		pEnd = pLine;

		if (pEnd != pStart)
		{
			iValue[i] = atoi(pStart);
			//printf("%d\t", iValue[i]);
		}
		else
		{
			if (i != 2)
			{
				return false;
			}
		}
	}

	FFace face;
	face._idx0 = iValue[0];
	face._idx1 = iValue[1];
	face._idx2 = iValue[2];
	m_FaceList.push_back(face);

	//printf("\n");
	return true;
}

