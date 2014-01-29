/*
 * BSP_test.cc
 *
 *  Created on: 28.12.2013
 *      Author: tenter
 */

#include <gtest/gtest.h>

#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/MeshCompiler.hh>

#include "MeshCompiler_testData.hh"


// Importer for custom binary file format developed to debug/profe MeshCompiler

class SBFReader : public ACG::MeshCompilerDefaultFaceInput
{
public:
  SBFReader(const char* szObjFile);
  virtual ~SBFReader(void) {}

  int getNumVertices() {return m_Vertices.size() / 3;}
  int getNumTexcoords() {return m_TexCoords.size() / 2;}
  int getNumNormals() {return m_Normals.size() / 3;}

  float* getVertices() {return m_Vertices.empty() ? 0 : &m_Vertices[0];}
  float* getNormals() {return m_Normals.empty() ? 0 : &m_Normals[0];}
  float* getTexCoords() {return m_TexCoords.empty() ? 0 : &m_TexCoords[0];}

  virtual int getVertexAdjCount(int _vertexID)
  {
    if (m_AdjVertexCount.size()) return m_AdjVertexCount[_vertexID];
    else return -1;
  }
  virtual int getVertexAdjFace(int _vertexID, int _k)
  {
    if (m_AdjVertexOffset.size()) return m_AdjVertexBuf[m_AdjVertexOffset[_vertexID] + _k];
    else return -1;
  }
  virtual int getFaceAdjCount(int _faceID)
  {
    if (m_AdjFaceCount.size()) return m_AdjFaceCount[_faceID];
    else return -1;
  }
  virtual int getFaceAdjFace(int _faceID, int _k)
  {
    if (m_AdjFaceOffset.size()) return m_AdjFaceBuf[m_AdjFaceOffset[_faceID] + _k];
    else return -1;
  }

  void writeFile(const char* szFile);

private:

  int m_NumVerts,
    m_NumNormals, 
    m_NumTexCoords;


  std::vector<float> m_Vertices;
  std::vector<float> m_Normals;
  std::vector<float> m_TexCoords;

  int m_PosAttrID,
    m_NormAttrID,
    m_TexCAttrID;



  // adjacency
  std::vector<unsigned char> m_AdjVertexCount;
  std::vector<int>           m_AdjVertexOffset;
  std::vector<int>           m_AdjVertexBuf;

  std::vector<int>           m_AdjFaceOffset;
  std::vector<unsigned char> m_AdjFaceCount;
  std::vector<int>           m_AdjFaceBuf;
};

SBFReader::SBFReader(const char* szObjFile)
  : MeshCompilerDefaultFaceInput(0, 0),
  m_NumVerts(0), m_NumNormals(0), m_NumTexCoords(0),
  m_PosAttrID(0), m_NormAttrID(-1), m_TexCAttrID(-1)
{
  if (szObjFile)
  {
    FILE* pFile = fopen(szObjFile, "rb");

    // count # tris, verts

    numFaces_ = 0;
    m_NumVerts = 0; m_NumNormals = 0; m_NumTexCoords = 0;

    if (pFile)
    {
      fread(&numFaces_, 4, 1, pFile);
      fread(&m_NumVerts, 4, 1, pFile);
      fread(&m_NumNormals, 4, 1, pFile);
      fread(&m_NumTexCoords, 4, 1, pFile);

      int faceBufSize = 0;
      fread(&faceBufSize, 4, 1, pFile);


      m_Vertices.resize(m_NumVerts * 3);
      m_Normals.resize(m_NumNormals * 3);
      m_TexCoords.resize(m_NumTexCoords * 3);
      faceOffset_.resize(numFaces_);
      faceSize_.resize(numFaces_);

      fread(&faceSize_[0], 4, numFaces_, pFile);

      faceOffset_[0] = 0;
      for (int i = 1; i < numFaces_; ++i)
        faceOffset_[i] = faceOffset_[i-1] + faceSize_[i-1];

      if (m_NumVerts)
      {
        faceData_[m_PosAttrID].resize(faceBufSize);
        fread(&faceData_[m_PosAttrID][0], 4, faceBufSize, pFile);
      }

      if (m_NumTexCoords)
      {
        m_TexCAttrID = 1;
        faceData_[m_TexCAttrID].resize(faceBufSize);
        fread(&faceData_[m_TexCAttrID][0], 4, faceBufSize, pFile);
      }

      if (m_NumNormals)
      {
        m_NormAttrID = m_TexCAttrID >= 0 ? m_TexCAttrID + 1 : m_PosAttrID + 1;
        faceData_[m_NormAttrID].resize(faceBufSize);
        fread(&faceData_[m_NormAttrID][0], 4, faceBufSize, pFile);
      }


      if (m_NumVerts)
        fread(&m_Vertices[0], 12, m_NumVerts, pFile);

      if (m_NumTexCoords)
        fread(&m_TexCoords[0], 8, m_NumTexCoords, pFile);

      if (m_NumNormals)
        fread(&m_Normals[0], 12, m_NumNormals, pFile);


      int numVertAdj, numFaceAdj, vertAdjSize, faceAdjSize;

      if (fread(&numVertAdj, 4, 1, pFile) == 1)
      {
        fread(&vertAdjSize, 4, 1, pFile);

        m_AdjVertexOffset.resize(numVertAdj);
        m_AdjVertexCount.resize(numVertAdj);
        m_AdjVertexBuf.resize(vertAdjSize);

        fread(&m_AdjVertexOffset[0], 4, numVertAdj, pFile);
        fread(&m_AdjVertexCount[0], 1, numVertAdj, pFile);
        fread(&m_AdjVertexBuf[0], 4, vertAdjSize, pFile);

        if (fread(&numFaceAdj, 4, 1, pFile) == 1)
        {
          fread(&faceAdjSize, 4, 1, pFile);

          m_AdjFaceOffset.resize(numFaceAdj);
          m_AdjFaceCount.resize(numFaceAdj);
          m_AdjFaceBuf.resize(faceAdjSize);

          fread(&m_AdjFaceOffset[0], 4, numFaceAdj, pFile);
          fread(&m_AdjFaceCount[0], 1, numFaceAdj, pFile);
          fread(&m_AdjFaceBuf[0], 4, faceAdjSize, pFile);
        }
      }

      fclose(pFile);
    }

  }

}



class MeshCompilerTest : public testing::Test {

public:
  MeshCompilerTest()
  {
    // create mesh 0
    MeshTestData input0;
    MeshCompilerTest_GetInput0(&input0);
    mesh0_ = CreateMesh(input0);

    // create mesh 1
    MeshTestData input1;
    MeshCompilerTest_GetInput1(&input1);
    mesh1_ = CreateMesh(input1);
  }

protected:

  // This function is called before each test is run
  virtual void SetUp() {

  }

  // This function is called after all tests are through
  virtual void TearDown() {

    delete mesh0_; mesh0_ = 0;
    delete mesh1_; mesh1_ = 0;
  }


  ACG::MeshCompiler* CreateMesh(const MeshTestData& input) {

    ACG::VertexDeclaration decl;
    decl.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_POSITION);
    if (input.numTexcoords_)
      decl.addElement(GL_FLOAT, 2, ACG::VERTEX_USAGE_TEXCOORD);
    if (input.numNormals_)
      decl.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_NORMAL);

    
    ACG::MeshCompiler* mesh = new ACG::MeshCompiler(decl);

    // vertex data input
    mesh->setVertices(input.numVerts_, input.vdata_pos);
    mesh->setTexCoords(input.numTexcoords_, input.vdata_t);
    mesh->setNormals(input.numNormals_, input.vdata_n);

    // face input
    mesh->setNumFaces(input.numFaces_, input.numIndices_);

    int offset = 0;
    for (int i = 0; i < input.numFaces_; ++i)
    {
      int fsize = input.fsize_[i];
      mesh->setFaceVerts(i, fsize, ((int*)input.fdata_pos) + offset);

      if (input.numTexcoords_ && input.fdata_t)
        mesh->setFaceTexCoords(i, fsize, ((int*)input.fdata_t) + offset);

      if (input.numNormals_ && input.fdata_n)
        mesh->setFaceNormals(i, fsize, ((int*)input.fdata_n) + offset);

      offset += fsize;
    }

    // test group id sorting

    const int numGroups = 5;

    //srand(GetTickCount());

    for (int i = 0; i < input.numFaces_; ++i)
      mesh->setFaceGroup(i, rand()% numGroups);

    return mesh;
  }


  ACG::MeshCompiler* mesh0_;
  ACG::MeshCompiler* mesh1_;

};


TEST_F(MeshCompilerTest, npoly_vpos__fff ) {

  mesh0_->build(false, false, false);
  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, npoly_vpos__tff ) {

  mesh0_->build(true, false, false);

  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, npoly_vpos__ftf ) {

  mesh0_->build(false, true, false);

  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, npoly_vpos__ttf ) {

  mesh0_->build(true, true, false);

  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}





TEST_F(MeshCompilerTest, npoly_vpos__fft ) {

  mesh0_->build(false, false, true);

  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, npoly_vpos__tft ) {

  mesh0_->build(true, false, true);

  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, npoly_vpos__ftt ) {

  mesh0_->build(false, true, true);

  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, npoly_vpos__ttt ) {

  mesh0_->build(true, true, true);

  EXPECT_EQ(mesh0_->dbgVerify(0), true) << "compiled mesh contains errors";
}





TEST_F(MeshCompilerTest, tri_vpos_texc__fff ) {

  mesh1_->build(false, false, false);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, tri_vpos_texc__tff ) {

  mesh1_->build(true, false, false);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, tri_vpos_texc__ftf ) {

  mesh1_->build(false, true, false);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, tri_vpos_texc__ttf ) {

  mesh1_->build(true, true, false);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}





TEST_F(MeshCompilerTest, tri_vpos_texc__fft ) {

  mesh1_->build(false, false, true);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, tri_vpos_texc__tft ) {

  mesh1_->build(true, false, true);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, tri_vpos_texc__ftt ) {

  mesh1_->build(false, true, true);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}

TEST_F(MeshCompilerTest, tri_vpos_texc__ttt ) {

  mesh1_->build(true, true, true);

  EXPECT_EQ(mesh1_->dbgVerify(0), true) << "compiled mesh contains errors";
}
