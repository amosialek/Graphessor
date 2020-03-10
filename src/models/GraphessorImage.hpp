#ifndef GRAPHESSORIMAGE_HPP
#define GRAPHESSORIMAGE_HPP

#include<vector>
#include<memory>
#include "Production.hpp"
#include "Image.hpp"

class GraphessorImage
{
    private:
        std::unique_ptr<std::vector<std::unique_ptr<Production>>> productions;
        std::unique_ptr<IGraphessorImageSerializer> saver;
    public:
        GraphessorImage(int width, int height);

        void AddProduction(std::unique_ptr<Production> production);
        Image GetImage();
        void Save(std::ostream& output);
        void Save(std::string& filename);
        static std::unique_ptr<GraphessorImage> Load(std::istream input);
        static std::unique_ptr<GraphessorImage> Load(std::string filename);
};

class IGraphessorImageSerializer{
    public:
        void Serialize(std::ostream& output, GraphessorImage& image, std::vector<std::unique_ptr<Production>>& productions);
        GraphessorImage Deserialize(std::ostream& input);    
};

#endif /* GRAPHESSORIMAGE_HPP */
