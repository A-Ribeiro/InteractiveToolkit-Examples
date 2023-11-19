#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace AlgorithmCore::Procedural;
using namespace AlgorithmCore::Search;

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    using namespace RoguelikeMatrix;

    World<char> roguelikeMatrix;

    // setup element to char map
    roguelikeMatrix.element_map[Element::NonWalkPlace] = '.';
    roguelikeMatrix.element_map[Element::WalkPlace] = ' ';
    roguelikeMatrix.element_map[Element::Edge] = 'o';

    roguelikeMatrix.generate(MathCore::vec2i(120, 27), // matrix_size
                             10,                       // max_areas
                             MathCore::vec2i(5, 2),    //_area_size_min,
                             MathCore::vec2i(30, 7),   //_area_size_max
                             5,                        // random_connections
                             false                     // allow_overlap
    );

    MathCore::vec2i start = roguelikeMatrix.randomAreaPoint();
    MathCore::vec2i end = roguelikeMatrix.randomAreaPoint();

    // AStar working struct definition
    struct Matrix_Map_TileType_Definition
    {
        using type = Element;
        static inline bool canWalkOnThisTile(const type &tile)
        {
            return tile == Element::WalkPlace;
        }
    };
    AStarMatrix<Matrix_Map_TileType_Definition> aStar;

    std::vector<MathCore::vec2i> path;
    aStar.setInputMatrix(&roguelikeMatrix.element_matrix);

    aStar.computePath(start, end, &path);

    // put the path in the final matrix
    for (const auto &v : path)
        roguelikeMatrix.final_matrix[v.y][v.x] = '*';
    roguelikeMatrix.final_matrix[start.y][start.x] = 'S';
    roguelikeMatrix.final_matrix[end.y][end.x] = 'E';

    // draw the result
    for (int y = 0; y < roguelikeMatrix.final_matrix.size.height; y++)
    {
        for (int x = 0; x < roguelikeMatrix.final_matrix.size.width; x++)
            printf("%c", roguelikeMatrix.final_matrix[y][x]);
        printf("\n");
    }

    return 0;
}
