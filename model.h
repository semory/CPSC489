#ifndef __CPSC489_MODEL_H
#define __CPSC489_MODEL_H

struct MeshUTFJoint {
 STDSTRINGW name;
 uint32 parent;
 real32 position[3];
 real32 m[9];
};

struct MeshUTFKeyFrame {
 uint32 frame;
 real32 position[3];
 real32 quaternion[4];
 real32 scale[3];
};

struct MeshUTFBoneKeyFrame {
 STDSTRINGW bone;
 uint32 bone_index;
 std::vector<MeshUTFKeyFrame> keyframes;
};

struct MeshUTFAnimation {
 STDSTRINGW name;
 std::vector<MeshUTFBoneKeyFrame> bonelist;
};

struct MeshUTFTexture {
 STDSTRINGW semantic;
 uint32 channel;
 STDSTRINGW filename;
};

struct MeshUTFMesh {
 STDSTRINGW name;
 std::vector<MeshUTFTexture> textures;
 std::vector<std::array<real32, 3>> position;
 std::vector<std::array<real32, 3>> normal;
 std::vector<std::array<real32, 2>> uvs[2];
 std::vector<std::array<uint16, 4>> bi;
 std::vector<std::array<real32, 4>> bw;
 std::vector<std::array<real32, 3>> colors[2];
};

struct MeshUTFDirect3D {
  // Joint Buffers
  ID3D11Buffer* jd_buffer;
  ID3D11Buffer* jv_buffer;
  ID3D11Buffer* ji_buffer;
  ID3D11Buffer* jc_buffer;
  // Mesh Buffers
  ID3D11Buffer* mm_buffer;
};

class MeshUTF {
 private :
  std::map<STDSTRINGW, uint32> jointmap;
  std::vector<MeshUTFJoint> joints;
  std::vector<MeshUTFAnimation> animations;
  std::vector<MeshUTFMesh> meshes;
  std::vector<MeshUTFDirect3D> buffers;
  ID3D11Buffer* ja_buffer;
 public :
  ErrorCode LoadModel(const wchar_t* filename);
  void FreeModel(void);
 public :
  ErrorCode RenderSkeleton(void);
  ErrorCode RenderMeshList(void);
  ErrorCode RenderModel(void);
 public : 
  MeshUTF();
  virtual ~MeshUTF();
 private :
  MeshUTF(const MeshUTF&) = delete;
  void operator =(const MeshUTF&) = delete;  
};

class MeshUTFInstance {
 private :
  const MeshUTF* mesh;
  real32 time;
  size_t anim;
 private :
  DirectX::XMMATRIX m;
  std::unique_ptr<DirectX::XMMATRIX> jm;
 public :
  void update(real32 dt) {
   time += dt;
   // compute transforms
   // if(time > mesh->) // loop animation
  }
 public :
  MeshUTFInstance(const MeshUTF& ptr) {
   mesh = &ptr;
   time = 0.0f;
   m = DirectX::XMMatrixIdentity();
  }
};

#endif
