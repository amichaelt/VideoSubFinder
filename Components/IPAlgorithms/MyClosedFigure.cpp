                              //MyClosedFigure.cpp//                                
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

#include "MyClosedFigure.h"
#include <QVector>

MyClosedFigure::MyClosedFigure()
{
    m_PointsArray = 0;
    m_Image = 0;
    m_Square=0;
    m_Count=1;
}

MyClosedFigure::~MyClosedFigure()
{
    if (m_PointsArray)
    {
        delete[] m_PointsArray;
    }
    if (m_Image)
    {
        delete[] m_Image;
    }
}

bool MyClosedFigure::operator>(MyClosedFigure& other)
{
    if((m_minY - other.m_minY) >= (0.7 * m_DY))
    {
        return true;
    }
    else
    {
        if((m_minX < other.m_minX) && (abs(m_minY - other.m_minY) < (0.7 * m_DY)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void MyClosedFigure::operator=(MyClosedFigure& other)
{    
    if (m_PointsArray)
    {
        delete[] m_PointsArray;
    }
    m_PointsArray = new MyPoint[other.m_Square];
    memcpy(m_PointsArray, other.m_PointsArray, 4 * other.m_Square);
    
    if (other.m_Image)
    {
        if (m_Image)
        {
            delete[] m_Image;
        }

        m_ImageWidth = other.m_ImageWidth;
        m_ImageHeight = other.m_ImageHeight;
        m_White = other.m_White;
        m_Black = other.m_Black;
        m_Image = new char[m_ImageWidth * m_ImageHeight];
        memcpy(m_Image, other.m_Image, m_ImageWidth * m_ImageHeight);
    }

    m_minX = other.m_minX;
    m_maxX = other.m_maxX;
    m_minY = other.m_minY;
    m_maxY = other.m_maxY;
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    m_Square = other.m_Square;
    m_Count = other.m_Count;
    m_DY = other.m_DY;
    m_SymbolValue = other.m_SymbolValue;
    m_ParentImageName = other.m_ParentImageName;
}

void MyClosedFigure::operator+=(MyClosedFigure& other)
{    
    int N1, N2;
    MyPoint *m;

    N1 = m_Square;
    N2 = other.m_Square;

    m = new MyPoint[N1 + N2];
    memcpy(m, m_PointsArray, 4 * N1);
    memcpy(m + N1, other.m_PointsArray, 4 * N2);

    delete[] m_PointsArray;
    m_PointsArray = m;
    m_Square = N1 + N2;
    m_Count += other.m_Count;

    if (other.m_minX < m_minX)
    {
        m_minX = other.m_minX;
    }
    if (other.m_maxX > m_maxX)
    {
        m_maxX = other.m_maxX;
    }
    if (other.m_minY < m_minY)
    {
        m_minY = other.m_minY;
    }
    if (other.m_maxY > m_maxY)
    {
        m_maxY = other.m_maxY;
    }
    m_Width = m_maxX-m_minX+1;
    m_Height = m_maxY-m_minY+1;
}

void MyClosedFigure::refresh()
{
    int i, N;
    int minX, maxX, minY, maxY;
    MyPoint *m;
    
    N = m_Square;
    if (N > 0)
    {
        m = m_PointsArray;
        minX = maxX = m[0].x();
        minY = maxY = m[0].y();

        for (i = 0; i < N; ++i)
        {
            if (m[i].x() < minX)
            {
                minX = m[i].x();
            }
            if (m[i].x() > maxX)
            {
                maxX = m[i].x();
            }
            if (m[i].y() < minY)
            {
                minY = m[i].y();
            }
            if (m[i].y() > maxY)
            {
                maxY = m[i].y();
            }
        }
        
        m_minX = minX;
        m_maxX = maxX;
        m_minY = minY;
        m_maxY = maxY;
        m_Width = (maxX - minX) + 1;
        m_Height = (maxY - minY) + 1;
    }
}

bool MyClosedFigure::IsNear(MyClosedFigure &other, int error)
{
    MyPoint Point,Point2, *m1, *m2;
    int i,j,N1,N2;

    if (abs((m_maxX + m_minX) - (other.m_maxX + other.m_minX)) < (m_maxX - m_minX) + (other.m_maxX - other.m_minX) + 3 + 2 * error)
    if (abs((m_maxY + m_minY) - (other.m_maxY + other.m_minY)) < (m_maxY - m_minY) + (other.m_maxY - other.m_minY) + 3 + 2 * error)
    {
        N1 = m_Square;
        N2 = other.m_Square;
        m1 = m_PointsArray;
        m2 = other.m_PointsArray;

        for(i = 0; i < N1; ++i)
        {
            Point = m1[i];
            
            for(j = 0; j < N2; ++j)
            {
                Point2 = m2[j];
                
                if (abs(Point.x() - Point2.x()) < 2 + error) 
                if (Point.y() == Point2.y())
                {
                    return true;
                }

                if (abs(Point.y() - Point2.y()) < 2 + error)
                if (Point.x() == Point2.x())
                {
                    return true;
                }
            }
        }
        return false;
    }
    return false;
}

void MyClosedFigure::CreateImage(int w, int h, char White, char Black)
{
    int i, x0, y0, x, y;

    m_ImageWidth = w;
    m_ImageHeight = h;
    m_White = White;
    m_Black = Black;

    x0 = (w - (m_minX + m_maxX)) / 2;
    y0 = (h - (m_minY + m_maxY)) / 2;

    if (m_Image)
    {
        delete[] m_Image;
    }

    m_Image = new char[w * h];

    for (i = 0; i< (w * h); ++i)
    {
        m_Image[i] = White;
    }

    for (i = 0; i < m_Square; ++i)
    { 
        x = x0 + m_PointsArray[i].x();
        y = y0 + m_PointsArray[i].y();
        
        if(y >= 0)
        {
            if(y < h)
            {
                if(x >= 0)
                {
                    if(x < w)
                    {
                        m_Image[(y * w) + x] = Black;
                    }
                }
            }
        }
    }
}

double MyClosedFigure::CompareWith(MyClosedFigure &other, double MaxPercentDeviation)
{
    char *m1, *m2;
    char Black, White;
    int i, w, h, dS;
    double value;

    if (m_Count == other.m_Count)
    {
        if ((double)abs(m_Width - other.m_Width) / (double)m_Width <= MaxPercentDeviation)
        {
            if ((double)abs(m_Height - other.m_Height) / (double)m_Height <= MaxPercentDeviation)
            {
                w = m_ImageWidth;
                h = m_ImageHeight;
                Black = m_Black;
                White = m_White;
                m1 = m_Image;
                m2 = other.m_Image;
                
                dS = 0;
                for (i = 0; i < (w * h); ++i)
                {
                    if (m1[i] != m2[i])
                    {
                        if ((m1[i - 1] != m2[i - 1]) || (m1[i - 1] == White))
                        {
                            if ((m1[i + 1] != m2[i + 1]) || (m1[i + 1] == White))
                            {
                                dS++;
                            }
                        }
                    }
                }

                value = (double)dS / m_Square;
                if (value <= MaxPercentDeviation)
                {
                    return value;
                }
            }
        }
    }
    return 1;
}

clock_t SearchClosedFigures(int *Im, int w, int h, int white, MyClosedFigure* &FiguresArray, int &Number)
{
    int N;
    int x, y;
    int size = w * h;
    QVector<int> m(size, -1);
    QVector<int> key(size);
    QVector<int> key2(size);
    int *NN, *I, *minX, *maxX, *minY, *maxY;
    int i, j, i1, i2, jj, kk, index;
    bool bln, bln2;
    clock_t start;
        
    start = clock();

    index = 0;

    for(i = 0; i < size; ++i)
    {
        key[i] = i;
        key2[i] = i;
    }

    //We find all closed figures in the picture
    for(i = 0; i < size; ++i)
    {
        if(Im[i] == white)
        {
            y = i / w;
            x = i - (y * w);
            
            bln = false;
            bln2 = false;
            //Check whether there is a point on the left figure
            if ((x > 0) && (Im[i - 1] == white)) //[x-1][y]
            {
                bln = true;
                i1 = i - 1;
                
                if (y > 0)
                {
                    //Check whether c from the left bottom point of the figure
                    if (Im[(i - w) - 1] != white) //[x-1][y-1]
                    {
                        //from the bottom to the left point of the figure no:

                        //Check whether there is a point below the figure
                        if (Im[i - w] == white) //[x][y-1]
                        {
                            bln2 = true;
                            i2 = i - w;
                        }

                        else 
                        {
                            //Check whether there is a lower right point of the figure
                            if (x < w - 1)
                            {
                                if (Im[(i - w) + 1] == white) //[x+1][y-1]
                                {
                                    bln2 = true;
                                    i2 = (i - w) + 1;
                                }
                            }
                        }
                    }
                    else
                    {
                        //from the bottom to the left point of the figure are:
                        
                        //Check whether there is a point below the figure
                        if (Im[i - w] != white) //[x][y-1]
                        {
                            //Check whether there is a lower right point of the figure
                            if (x < w - 1)
                            {
                                if (Im[(i - w) + 1] == white) //[x+1][y-1]
                                {
                                    bln2 = true;
                                    i2 = (i - w) + 1;
                                }
                            }
                        }
                    }
                }
            }
            else 
            {
                //from the left point of the figure no:
                if (y > 0)
                {
                    //Check whether there is a point below the figure
                    if (Im[i - w] == white) //[x][y-1]
                    {
                        bln = true;
                        i1 = i - w;
                    }
                    else
                    {
                        //Check whether there is a point at the bottom left figure
                        if((x > 0) && (Im[(i - w) - 1] == white)) //[x-1][y-1]
                        {
                            bln = true;
                            i1 = (i - w) - 1;
                            
                            //Check whether there is a point at the bottom right figure
                            if (x < w - 1)
                            {
                                if (Im[(i - w) + 1] == white) //[x+1][y-1]
                                {
                                    bln2 = true;
                                    i2 = (i - w) + 1;
                                }
                            }
                        }
                        else 
                        {
                            //Check whether there is a point on the bottom right figure
                            if (x<w-1)
                            {
                                if (Im[(i - w) + 1] == white) //[x+1][y-1]
                                {
                                    bln = true;
                                    i1 = (i - w) + 1;
                                }
                            }
                        }
                    }
                }
            }

            if (bln)
            {
                m[i] = key[m[i1]];
            }
            else
            {
                m[i] = index;
                index++;
            }

            if (bln2)
            {
                if (m[i1] != m[i2])
                {
                    jj = std::max(m[i1], m[i2]);
                    kk = std::min(m[i1], m[i2]);
                    while (key[jj] != jj)
                    {
                        jj = key[jj];
                    }
                    while (key[kk] != kk)
                    {
                        kk = key[kk];
                    }
                    key[std::max(jj, kk)] = std::min(jj, kk);
                }
            }
        }
    }

    N = 0;
    for (i = 0; i < index; ++i)
    {
        j = i;
        while (key[j] != j)
        {
            j = key[j];
        }
        key[i] = j;

        if (key[i] == i)
        {
            key2[i] = N;
            ++N;
        }
    }

    Number = N;
    NN = new int[N];
    I = new int[N];
    minX = new int[N];
    maxX = new int[N];
    minY = new int[N];
    maxY = new int[N];

    for(i = 0; i < N; ++i)
    {
        NN[i] = 0;
        I[i] = 0;
        minX[i] = w;
        maxX[i] = 0;
        minY[i] = h;
        maxY[i] = 0;
    }

    for(i = 0; i < w * h; ++i)
    {
        if (Im[i] == white)
        {
            y = i / w;
            x = i - (y * w);

            j = key2[key[m[i]]];
            NN[j]++;
            maxY[j] = y;
            if (minY[j]>y)
            {
                minY[j] = y;
            }
            if (maxX[j]<x)
            {
                maxX[j] = x;
            }
            if (minX[j]>x)
            {
                minX[j] = x;
            }
        }
    }

    FiguresArray = new MyClosedFigure[N];

    MyClosedFigure *pfa = FiguresArray;
    MyClosedFigure *pf;
    
    for(i = 0; i < N; ++i)
    {
        pf = pfa + i;
        pf->setPointsArray(new MyPoint[NN[i]]);
        pf->setSquare(NN[i]);
        pf->setMinX(minX[i]);
        pf->setMaxX(maxX[i]);
        pf->setMinY(minY[i]);
        pf->setMaxY(maxY[i]);
        pf->setWidth((maxX[i] - minX[i]) + 1);
        pf->setHeight((maxY[i] - minY[i]) + 1);
    }

    for(y = 0, i = 0; y < h; ++y)
    {
        for(x = 0; x < w; ++x, ++i)
        {
            if (Im[i] == white)
            {
                j = key2[key[m[i]]];
                pfa[j].pointsArray()[I[j]] = MyPoint(x, y, i);
                I[j]++;
            }
        }
    }

    delete[] NN;
    delete[] I;
    delete[] minX;
    delete[] maxX;
    delete[] minY;
    delete[] maxY;
    
    return (clock() - start);
}

clock_t CreateIndexedImage(int *Im, int *ImRES, int w, int h, int white, int &Number)
{
    int N;
    int *m, *key, *key2;
    int i, j, i1, i2, jj, kk, index;
    int x, y, size;
    bool bln, bln2;
    clock_t start;
        
    start = clock();
    size = w*h;

    m = ImRES;
    key = new int[size];
    key2 = new int[size];
    index=0;

    for(i=0; i<size; i++)
    {
        m[i]=-1;
        key[i]=i;
        key2[i]=i;
    }

    //Ќаходим все замкнутые фигуры в изображении
    for(i=0; i<size; i++)
    {
        if(Im[i] == white)
        {
            y = i/w;
            x = i - y*w;
            
            bln=false;
            bln2=false;
            //ѕровер€ем есть ли с лева точка фигуры
            if ( (x>0) && (Im[i-1]==white) )//[x-1][y]
            {//есть:
                bln=true;
                i1=i-1;
                
                if (y>0)
                {
                    //ѕровер€ем нет ли c низу с лева точки фигуры
                    if (Im[i-w-1]!=white)//[x-1][y-1]
                    {//с низу с лева точки фигуры нету:

                        //ѕровер€ем есть ли с низу точка фигуры
                        if (Im[i-w]==white)//[x][y-1]
                        {//есть:
                            bln2=true;
                            i2=i-w;
                        }

                        else 
                        {//нету: 
                            
                            //ѕровер€ем есть ли с низу с права точка фигуры
                            if (x<w-1) 
                            if (Im[i-w+1]==white)//[x+1][y-1]
                            {//есть:
                                bln2=true;
                                i2=i-w+1;
                            }
                        }
                    }
                    else
                    {//с низу с лева точка фигуры есть:
                        
                        //ѕровер€ем нет ли с низу точки фигуры
                        if (Im[i-w]!=white)//[x][y-1]
                        {//нет:
                            
                            //ѕровер€ем есть ли с низу с права точка фигуры
                            if (x<w-1) 
                            if (Im[i-w+1]==white)//[x+1][y-1]
                            {//есть:
                                bln2=true;
                                i2=i-w+1;
                            }
                        }
                    }
                }
            }
            else 
            {//с лева точки фигуры нету:

                if (y>0)
                {
                    //ѕровер€ем есть ли с низу точка фигуры
                    if (Im[i-w]==white)//[x][y-1]
                    {//есть:
                        bln=true;
                        i1=i-w;
                    }

                    else
                    {//нету:
                        
                        //ѕровер€ем есть ли снизу слева точка фигуры
                        if( (x>0) && (Im[i-w-1]==white) )//[x-1][y-1]
                        {//есть:
                            bln=true;
                            i1=i-w-1;
                            
                            //ѕровер€ем есть ли снизу справа точка фигуры
                            if (x<w-1) 
                            if (Im[i-w+1]==white)//[x+1][y-1]
                            {
                            //есть:
                                bln2=true;
                                i2=i-w+1;
                            }
                        }
                        
                        else 
                        {//нету: 
                            
                            //ѕровер€ем есть ли снизу справа точка фигуры
                            if (x<w-1) 
                            if (Im[i-w+1]==white)//[x+1][y-1]
                            {
                            //есть:
                                bln=true;
                                i1=i-w+1;
                            }
                        }
                    }
                }
            }

            if (bln)
            {
                m[i]=key[m[i1]];
            }
            else
            {
                m[i]=index;
                index++;
            }

            if (bln2)
            if (m[i1]!=m[i2])
            {
                jj=std::max(m[i1],m[i2]);
                kk=std::min(m[i1],m[i2]);
                while (key[jj]!=jj) jj=key[jj];
                while (key[kk]!=kk) kk=key[kk];
                key[std::max(jj,kk)]=std::min(jj,kk);
            }

        }
    }

    N=0;
    for (i=0; i<index; i++)
    {
        j=i;
        while (key[j]!=j) j=key[j];
        key[i]=j;

        if (key[i]==i)
        {
            key2[i]=N;
            N++;
        }
    }

    for(i=0; i<size; i++)
    {
        if (Im[i] == white)
        {
            y = i/w;
            x = i - y*w;

            j=key2[key[m[i]]];
            m[i] = j;
        }
    }

    Number = N;

    delete[] key;
    delete[] key2;
    
    return (clock()-start);
}
