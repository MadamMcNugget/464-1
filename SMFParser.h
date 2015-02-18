#ifndef _SMF_PARSER_H_
#define _SMF_PARSER_H_

#include <vector>

struct FVector3f
{
	float _x, _y, _z;
};

struct FFace
{
	int _idx0, _idx1, _idx2;
};

class SMFParser
{
public:
	SMFParser();
	~SMFParser();

	// Parser SMF Files
	bool Parser(const char* pFileName);

	std::vector<FVector3f>& GetVertexList(){return m_VertexList;}
	std::vector<FFace>& GetFaceList(){return m_FaceList;}

protected:
	bool ParserLine(const char* pLine);

	bool ParserVertex(const char* pLine);
	bool ParserFace(const char* pLine);

protected:
	std::vector<FVector3f> m_VertexList;
	std::vector<FFace> m_FaceList;
};


#endif // _SMF_PARSER_H_

