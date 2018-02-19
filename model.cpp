#include "stdafx.h"
#include "stdgfx.h"
#include "errors.h"
#include "win.h"
#include "gfx.h"
#include "math.h"
#include "vector3.h"
#include "axes.h"
#include "ascii.h"
#include "model.h"

MeshUTF::MeshUTF()
{
 ja_buffer = 0;
}

MeshUTF::~MeshUTF()
{
}

ErrorCode MeshUTF::LoadModel(const wchar_t* filename)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // load lines
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(filename, linelist);
 if(Fail(code)) return code;

 //
 // READ SKELETON
 //

 // read number of joints
 uint32 n_jnts = 0;
 code = ASCIIReadUint32(linelist, &n_jnts);
 if(Fail(code)) return code;

 // read joints
 if(n_jnts) joints.resize(n_jnts);
 for(uint32 i = 0; i < n_jnts; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return code;

     // name must be valid
     joints[i].name = ConvertUTF8ToUTF16(buffer);
     if(!joints[i].name.length()) return EC_MODEL_BONENAME;

     // read parent
     code = ASCIIReadUint32(linelist, &joints[i].parent);
     if(Fail(code)) return code;

     // read position
     code = ASCIIReadVector3(linelist, &joints[i].position[0], false);
     if(Fail(code)) return code;

     // read matrix
     code = ASCIIReadMatrix3(linelist, &joints[i].m[0], false);
     if(Fail(code)) return code;

     // add joint to jointmap
     jointmap.insert(std::map<STDSTRINGW, uint32>::value_type(joints[i].name, i));
    }

 //
 // READ ANIMATIONS
 //

 // read number of animations
 uint32 n_anim = 0;
 code = ASCIIReadUint32(linelist, &n_anim);
 if(Fail(code)) return code;

 // read animations
 if(n_anim) animations.resize(n_anim);
 for(size_t i = 0; i < n_anim; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return code;

     // name must be valid
     animations[i].name = ConvertUTF8ToUTF16(buffer);
     if(!animations[i].name.length()) return EC_MODEL_ANIMATION_NAME;

     // read number of keyframed bones
     uint32 n_keyframedbones = 0;
     code = ASCIIReadUint32(linelist, &n_keyframedbones);
     if(Fail(code)) return code;
     if(!n_keyframedbones) return EC_MODEL_KEYFRAMED_BONES;

     // read keyframed bones
     animations[i].bonelist.resize(n_keyframedbones);
     for(size_t j = 0; j < n_keyframedbones; j++)
        {
         // read name
         char buffer[1024];
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return code;

         // name must be valid
         animations[i].bonelist[j].bone = ConvertUTF8ToUTF16(buffer);
         if(!animations[i].bonelist[j].bone.length()) return EC_MODEL_BONENAME;

         // lookup bone index
         auto iter = jointmap.find(animations[i].bonelist[j].bone);
         if(iter == jointmap.end()) return EC_MODEL_BONE_LOOKUP;
         animations[i].bonelist[j].bone_index = iter->second;

         // read number of keyframes
         uint32 n_keyframes = 0;
         code = ASCIIReadUint32(linelist, &n_keyframes);
         if(Fail(code)) return code;
         if(!n_keyframes) return EC_MODEL_KEYFRAMES;

         // read keyframes
         animations[i].bonelist[j].keyframes.resize(n_keyframes);
         for(size_t k = 0; k < n_keyframes; k++)
            {
             code = ASCIIReadUint32(linelist, &animations[i].bonelist[j].keyframes[k].frame);
             if(Fail(code)) return code;

             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].position[0]);
             if(Fail(code)) return code;

             code = ASCIIReadVector4(linelist, &animations[i].bonelist[j].keyframes[k].quaternion[0]);
             if(Fail(code)) return code;

             code = ASCIIReadVector3(linelist, &animations[i].bonelist[j].keyframes[k].scale[0]);
             if(Fail(code)) return code;
            }
        }
    }

 //
 // READ MESHES
 //

 // read number of meshes
 uint32 n_mesh = 0;
 code = ASCIIReadUint32(linelist, &n_mesh);
 if(Fail(code)) return code;
 if(!n_mesh) return EC_MODEL_MESH;

 // read mesh list
 meshes.resize(n_mesh);
 for(uint32 i = 0; i < n_mesh; i++)
    {
     // read name
     char buffer[1024];
     code = ASCIIReadString(linelist, buffer);
     if(Fail(code)) return code;

     // name must be valid
     meshes[i].name = ConvertUTF8ToUTF16(buffer);
     if(!meshes[i].name.length()) return EC_MODEL_MESHNAME;

     // read number of vertices
     uint32 n_verts = 0;
     code = ASCIIReadUint32(linelist, &n_verts);
     if(Fail(code)) return code;
     if(!n_verts) return EC_MODEL_VERTICES;

     // read number of UV channels
     uint32 n_uvs = 0;
     code = ASCIIReadUint32(linelist, &n_uvs);
     if(Fail(code)) return code;
     if(n_uvs > 2) return EC_MODEL_UV_CHANNELS;

     // read number of color channels
     uint32 n_colors = 0;
     code = ASCIIReadUint32(linelist, &n_colors);
     if(Fail(code)) return code;
     if(n_colors > 2) return EC_MODEL_COLOR_CHANNELS;

     // read number of textures
     uint32 n_textures = 0;
     code = ASCIIReadUint32(linelist, &n_textures);
     if(Fail(code)) return code;
     if(n_textures > 8) return EC_MODEL_TEXTURES;

     // read textures
     meshes[i].textures.resize(n_textures);
     for(uint32 j = 0; j < n_textures; j++)
        {
         // read semantic
         char buffer[1024];
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return code;

         // semantic must be valid
         meshes[i].textures[j].semantic = ConvertUTF8ToUTF16(buffer);
         if(!meshes[i].textures[j].semantic.length()) return EC_MODEL_TEXTURE_SEMANTIC;

         // read channel used
         uint32 n_use = 0;
         code = ASCIIReadUint32(linelist, &n_use);
         if(Fail(code)) return code;
         if(!(n_use < n_uvs)) return EC_MODEL_TEXTURE_CHANNEL;
         meshes[i].textures[j].channel = n_use;

         // read filename
         code = ASCIIReadString(linelist, buffer);
         if(Fail(code)) return code;

         // filename must be valid
         meshes[i].textures[j].filename = ConvertUTF8ToUTF16(buffer);
         if(!meshes[i].textures[j].filename.length()) return EC_MODEL_TEXTURE_FILENAME;
        }

     // create mesh data
     meshes[i].position.resize(n_verts);
     meshes[i].normal.resize(n_verts);
     meshes[i].uvs[0].resize(n_verts);
     meshes[i].uvs[1].resize(n_verts);
     if(joints.size()) {
        meshes[i].bi.resize(n_verts);
        meshes[i].bw.resize(n_verts);
       }
     meshes[i].colors[0].resize(n_verts);
     meshes[i].colors[1].resize(n_verts);

     // read mesh data
     for(uint32 j = 0; j < n_verts; j++)
        {
         // read position
         code = ASCIIReadVector3(linelist, &meshes[i].position[j][0], false);
         if(Fail(code)) return code;

         // read normal
         code = ASCIIReadVector3(linelist, &meshes[i].normal[j][0], false);
         if(Fail(code)) return code;

         // read UVs
         for(uint32 k = 0; k < n_uvs; k++) {
             code = ASCIIReadVector2(linelist, &meshes[i].uvs[j][k][0], false);
             if(Fail(code)) return code;
            }

         // read blendindices and blendweights
         if(joints.size()) {
            code = ASCIIReadVector4(linelist, &meshes[i].bi[j][0], false); // set to repeat?
            if(Fail(code)) return code;
            code = ASCIIReadVector4(linelist, &meshes[i].bw[j][0], false); // set to repeat?
            if(Fail(code)) return code;
           }

         // read colors
         for(uint32 k = 0; k < n_colors; k++) {
             code = ASCIIReadVector3(linelist, &meshes[i].colors[j][k][0], false);
             if(Fail(code)) return code;
            }
        }
    }

 //
 // CREATE BUFFERS
 //

 // prepare joint axes buffer
 struct JAXISBUFFER {
  real32 m[16];
  real32 scale[4];
 };

 // create Direct3D buffer (move this to gfx.h)
 D3D11_BUFFER_DESC desc;
 ZeroMemory(&desc, sizeof(desc));
 desc.Usage = D3D11_USAGE_DYNAMIC;
 desc.ByteWidth = n_jnts*sizeof(JAXISBUFFER);
 desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
 desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

 // create buffer
 HRESULT result = GetD3DDevice()->CreateBuffer(&desc, NULL, &ja_buffer);
 if(FAILED(result)) return EC_D3D_CREATE_BUFFER;

 D3D11_MAPPED_SUBRESOURCE msr;
 ZeroMemory(&msr, sizeof(msr));
 if(FAILED(GetD3DDeviceContext()->Map(ja_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) return EC_D3D_MAP_RESOURCE;
 JAXISBUFFER* data = reinterpret_cast<JAXISBUFFER*>(msr.pData);
 for(size_t i = 0; i < n_jnts; i++) {
     data[i].m[0x0] = joints[i].m[0];
     data[i].m[0x1] = joints[i].m[1];
     data[i].m[0x2] = joints[i].m[2];
     data[i].m[0x3] = joints[i].position[0];
     data[i].m[0x4] = joints[i].m[3];
     data[i].m[0x5] = joints[i].m[4];
     data[i].m[0x6] = joints[i].m[5];
     data[i].m[0x7] = joints[i].position[1];
     data[i].m[0x8] = joints[i].m[6];
     data[i].m[0x9] = joints[i].m[7];
     data[i].m[0xA] = joints[i].m[8];
     data[i].m[0xB] = joints[i].position[2];
     data[i].m[0xC] = 0.0f;
     data[i].m[0xD] = 0.0f;
     data[i].m[0xE] = 0.0f;
     data[i].m[0xF] = 1.0f;
     data[i].scale[0] = 1.0f;
     data[i].scale[1] = 1.0f;
     data[i].scale[2] = 1.0f;
     data[i].scale[3] = 1.0f;
    }
 GetD3DDeviceContext()->Unmap(ja_buffer, 0);

 return EC_SUCCESS;
}

void MeshUTF::FreeModel(void)
{
 if(ja_buffer) ja_buffer->Release();
 ja_buffer = nullptr;

 for(size_t i = 0; i < buffers.size(); i++) {
     buffers[i].jd_buffer->Release(); buffers[i].jd_buffer = nullptr;
     buffers[i].jv_buffer->Release(); buffers[i].jv_buffer = nullptr;
     buffers[i].ji_buffer->Release(); buffers[i].ji_buffer = nullptr;
     buffers[i].jc_buffer->Release(); buffers[i].jc_buffer = nullptr;
     buffers[i].mm_buffer->Release(); buffers[i].mm_buffer = nullptr;
    }
 buffers.clear();
 animations.clear();
 meshes.clear();
 joints.clear();
}

ErrorCode MeshUTF::RenderSkeleton(void)
{
 if(ja_buffer) RenderAxes(ja_buffer, (UINT)joints.size());
 return EC_SUCCESS;
}

ErrorCode MeshUTF::RenderMeshList(void)
{
 return EC_SUCCESS;
}

ErrorCode MeshUTF::RenderModel(void)
{
 SetVertexShaderPerModelBuffer(GetIdentityMatrix());
 RenderSkeleton();
 return EC_SUCCESS;
}
