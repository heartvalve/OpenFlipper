// returns picking ids of vertices

#version 150

in vec4 vertexPickID;
out vec4 outPickID;

void main()
{
  outPickID = vertexPickID;
}

