                              //MyClosedFigure.h//                                
//////////////////////////////////////////////////////////////////////////////////
//                              Version 1.76                                      //
//                                                                                //
// Author:  Simeon Kosnitsky                                                    //
//          skosnits@gmail.com                                                    //
//                                                                                //
// License:                                                                        //
//     This software is released into the public domain.  You are free to use    //
//     it in any way you like, except that you may not sell this source code.    //
//                                                                                //
//     This software is provided "as is" with no expressed or implied warranty.    //
//     I accept no liability for any damage or loss of business that this        //
//     software may cause.                                                        //
//                                                                                //
//////////////////////////////////////////////////////////////////////////////////

#ifndef MYCLOSEDFIGURE_H
#define MYCLOSEDFIGURE_H

#include <vector>
#include <time.h>

class MyPoint
{
public:
    MyPoint() {}
    MyPoint(int x, int y, int pointNumber)
    {
        x1 = x;
        y1 = y;
        number = pointNumber;
    }

    inline int x() { return x1; }
    inline void setX(int x) { x1 = x; }
    inline int y() { return y1; }
    inline void setY(int y) { y1 = y; }
    inline int pointNumber() { return number; }
    inline void setPointNumber(int pointNumber) { number = pointNumber; }

private:
    int x1;
    int y1;
    int    number;
}; 

typedef enum {UP, DOWN} POS;

class MyClosedFigure
{
public:
    MyClosedFigure();
    ~MyClosedFigure();

    bool operator>(MyClosedFigure& other);
    void operator=(MyClosedFigure& other);
    void operator+=(MyClosedFigure& other);

    void refresh();
    bool IsNear(MyClosedFigure &other, int error);    
    void CreateImage(int w, int h, char White, char Black);
    double CompareWith(MyClosedFigure &other, double MaxPercentDeviation);

    MyPoint *pointsArray() { return m_PointsArray; }
    void setPointsArray(MyPoint *pointsArray) { m_PointsArray = pointsArray; }
    int minX() { return m_minX; }
    void setMinX(int minX) { m_minX = minX; }
    int maxX() { return m_maxX; }
    void setMaxX(int maxX) { m_maxX = maxX; }
    int minY() { return m_minY; }
    void setMinY(int minY) { m_minY = minY; }
    int maxY() { return m_maxY; }
    void setMaxY(int maxY) { m_maxY = maxY; }
    int width() { return m_Width; }
    void setWidth(int width) { m_Width = width; }
    int height() { return m_Height; }
    void setHeight(int height) { m_Height = height; }
    int square() { return m_Square; }
    void setSquare(int square) { m_Square = square; }
    int count() { return m_Count; }
    void setCount(int count) { m_Count = count; }
    int weight() { return m_Weight; }
    void setWeight(int weight) { m_Weight = weight; }
    int dy() { return m_DY; }
    void setDY(int dy) { m_DY = dy; }
    std::string symbolValue() { return m_SymbolValue; }
    void setSymbolValue(std::string symbolValue) { m_SymbolValue = symbolValue; }
    std::string parentImageName() { return m_ParentImageName; }
    void setParentImageName(std::string parentImageName) { m_ParentImageName = parentImageName; }

    int mY() { return m_mY; }
    void setMY(int mY) { m_mY = mY; }
    int mI() { return m_mI; }
    void setMI(int mI) { m_mI = mI; }
    int mQ() { return m_mQ; }
    void setMQ(int mQ) { m_mQ = mQ; }
    int mmY() { return m_mmY; }
    void setMMY(int mmY) { m_mmY = mmY; }
    int mmI() { return m_mmI; }
    void setMMI(int mmI) { m_mmI = mmI; }
    int mmQ() { return m_mmQ; }
    void setMMQ(int mmQ) { m_mmQ = mmQ; }

    char *image() { return m_Image; }
    void setImage(char *image) { m_Image = image; }
    int imageWidth() { return m_ImageWidth; }
    void setImageWidth(int width) { m_ImageWidth = width; }
    int imageHeight() { return m_ImageHeight; }
    void setImageHeight(int height) { m_ImageHeight = height; }
    char white() { return m_White; }
    void setWhite(char white) { m_White = white; }
    char black() { return m_Black; }
    void setBlack(char black) { m_Black = black; }

    POS position() { return m_Position; }
    void setPosition(POS pos) { m_Position = pos; }

private:
    MyPoint *m_PointsArray;
    int m_minX;
    int m_maxX;
    int m_minY;
    int m_maxY;
    int m_Width;
    int m_Height;
    int m_Square;
    int m_Count;
    int m_Weight;
    int m_DY;
    std::string m_SymbolValue;
    std::string m_ParentImageName;

    int m_mY;
    int m_mI;
    int m_mQ;
    int m_mmY;
    int m_mmI;
    int m_mmQ;

    char *m_Image;
    int m_ImageWidth;
    int m_ImageHeight;
    char m_White;
    char m_Black;

    POS m_Position;
};

clock_t SearchClosedFigures(int *Im, int w, int h, int white, MyClosedFigure* &FiguresArray, int &Number);
clock_t CreateIndexedImage(int *Im, int *ImRES, int w, int h, int white, int &Number);

#endif
