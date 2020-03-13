#include "GraphessorImage.hpp"

GraphessorImage::GraphessorImage(int width, int height){};

void GraphessorImage::AddProduction(std::unique_ptr<Production> production){};
Image GraphessorImage::GetImage(){};
void GraphessorImage::Save(std::ostream& output){};
void GraphessorImage::Save(std::string& filename){};
std::unique_ptr<GraphessorImage> GraphessorImage::Load(std::istream input){};
std::unique_ptr<GraphessorImage> GraphessorImage::Load(std::string filename){};
