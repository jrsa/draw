#include <glbinding/Binding.h>
#include <glog/logging.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


int main(int argc, char* argv[]) {

  LOG(INFO) << "loading " << argv[1];

  // load from file
  const aiScene* scene = aiImportFile(argv[1], aiProcessPreset_TargetRealtime_MaxQuality);

  LOG(INFO) << "scene loaded: contains " << scene->mNumMeshes << " meshes";

  for (int m = 0; m < scene->mNumMeshes; ++m) {
    LOG(INFO) << "mesh " << m << " contains:" << scene->mMeshes[m]->mNumVertices << " verts";
  }


  // release structure loaded from file
  aiReleaseImport(scene);
  return 0;
}