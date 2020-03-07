#ifndef GRAPHESSORIMAGE_HPP
#define GRAPHESSORIMAGE_HPP

#include<vector>

class GraphessorImage
{
    private:
        vector<Production*> productions;
    public:
        GraphessorImage(int width, int height);
        void AddProduction(Producion* production);
        Image GetImage();
};

#endif /* GRAPHESSORIMAGE_HPP */
