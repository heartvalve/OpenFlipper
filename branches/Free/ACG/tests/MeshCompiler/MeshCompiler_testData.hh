
#ifndef MESHCOMPILER_TEST_DATA
#define MESHCOMPILER_TEST_DATA

class MeshTestData 
{
public:
  MeshTestData();

  int numFaces_,
    numVerts_,
    numTexcoords_,
    numNormals_,
    numIndices_;

  // face size (number of corners)
  const unsigned char* fsize_;

  // face data: indices for position, texcoords, normals
  const int* fdata_pos;
  const int* fdata_t;
  const int* fdata_n;

  // float3 positions
  const float* vdata_pos;
  const float* vdata_t;
  const float* vdata_n;
};

// get input buffer
void MeshCompilerTest_GetInput0(MeshTestData* input_);
void MeshCompilerTest_GetInput1(MeshTestData* input_);




#endif