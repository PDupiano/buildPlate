#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <map>
#include "bitmap_image.hpp"

struct subBox
{
	double x;
	double y;
	double xMax;
	double yMax;
};

class v3
{
	public:
	v3() {}
	v3(char* bin);
	v3(double x, double y, double z);

	double m_x, m_y, m_z;
};

v3::v3(char* face)
{
	char f1[4] = {face[0], face[1], face[2], face[3]};
	char f2[4] = {face[4], face[5], face[6], face[7]};
	char f3[4] = {face[8], face[9], face[10], face[11]};

	float xx = *((float*) f1);
	float yy = *((float*) f2);
	float zz = *((float*) f3);

	m_x = double(xx);
	m_y = double(yy);
	m_z = double(zz);
}

class tri
{
	public:
	tri();
	tri(v3 p1, v3 p2, v3 p3);
	void draw();

	v3 m_p1, m_p2, m_p3;
};

tri::tri(v3 p1, v3 p2, v3 p3)
{
	m_p1 = p1;
	m_p2 = p2;
	m_p3 = p3;
}

struct tMatrix
{
	double xOffset;
	double yOffset;
	double rotation;
};

class box
{
public:
	box();
	box(double xMinVal, double xMaxVal, double yMinVal, double yMaxVal)
	{
		xMin = xMinVal;
		xMax = xMaxVal;
		yMin = yMinVal;
		yMax = yMaxVal;
		area = (yMax-yMin)*(xMax - xMin);
		diagonal = hypot(yMax - yMin, xMax - xMin);
		transform.xOffset = 0.0;
		transform.yOffset = 0.0;
		transform.rotation = 0.0;
	}
	double xMin, xMax, yMin, yMax;
	double diagonal;
	double area;
	tMatrix transform;
};


bool compareBox(box a, box b)
{
	return(a.area>b.area);
}
//void read_stl(std::string fname, std::vector<tri>&v);
box read_stl(std::string fname);//, std::vector<box>&b);
void arrangeModels(box model);
void arrangeModels(double plateWidth, double plateLength, std::vector<box>&b);
void setToOrigin(std::vector<box>&b);
int main () 
{
	bitmap_image image(295, 195);
	image.set_all_channels(255,255,255);
	image_drawer draw(image);
	int imageCenterX = image.width() / 2;
	int imageCenterY = image.height() / 2;

	std::map<std::string, box> testMap;
	std::vector<box> boom;
	//modelMap.insert(std::pair<box, int>(read_stl("cube.stl"), 0));

	//testMap.insert(std::pair<std::string, std::map<box, tMatrix> >("3dBench.stl", std::pair<box, tMatrix>(read_stl("3DBenchy.stl"), tMatrix(0,0,0)));
	testMap.insert(std::pair<std::string, box>("cube.stl", read_stl("./testModels/cube.stl")));
	testMap.insert(std::pair<std::string, box>("3DBenchy.stl", read_stl("./testModels/3DBenchy.stl")));

	boom.push_back(read_stl("./testModels/cube.stl"));
	boom.push_back(read_stl("./testModels/cube.stl"));
	boom.push_back(read_stl("./testModels/3DBenchy.stl"));
	boom.push_back(read_stl("./testModels/3DBenchy.stl"));
	boom.push_back(read_stl("./testModels/3DBenchy.stl"));
	boom.push_back(read_stl("./testModels/GnomeScan.stl"));

	sort(boom.begin(), boom.end(), compareBox);
	setToOrigin(boom);
	arrangeModels(295,195.0,boom);

	for ( box model: boom)
	{
		std::cout << "Max X: " << model.xMax <<std::endl;
		std::cout << "Diagonal: " << model.diagonal <<std::endl;
		std::cout << "Area: " << model.area <<std::endl;
		std::cout << "Transformation: " << model.transform.xOffset << " " << model.transform.yOffset 
					<< " " << model.transform.rotation << std::endl <<std::endl;
		draw.pen_width(5);
		draw.pen_color(0,0,0);
		if (model.transform.rotation) 
		{
			draw.rectangle(imageCenterX + model.transform.xOffset + model.yMin, 
						imageCenterY + model.transform.yOffset + model.xMin, 
						imageCenterX + model.transform.xOffset + model.yMax, 
						imageCenterY + model.transform.yOffset + model.xMax);
		}
		else 
		{
			draw.rectangle(imageCenterX + model.transform.xOffset + model.xMin, 
						imageCenterY + model.transform.yOffset + model.yMin, 
						imageCenterX + model.transform.xOffset + model.xMax, 
						imageCenterY + model.transform.yOffset + model.yMax);
		}
		
	}
	image.save_image("./outputs/output.bmp");
}

void setToOrigin(std::vector<box>&b)
{
	for (std::vector<box>::iterator it = b.begin() ; it != b.end(); ++it)
	{
		if ((*it).xMin != 0)
			(*it).transform.xOffset = 0 - (*it).xMin;
		if ((*it).yMin != 0)
			(*it).transform.yOffset = 0 - (*it).yMin;
	}
}

void arrangeModels(double plateWidth, double plateLength, std::vector<box>&b)
{
	int numModels = b.size();
	double tempXmax, tempYmax, oldBlobXmax, oldBlobYmax, buffer;
	double tempBlobH, tempBlobV, tempBlobHR, tempBlobVR, tempBlobPoints;
	box blob(0,0,0,0);
	std::vector<box> pointsToCheck;
	for (std::vector<box>::iterator it = b.begin() ; it != b.end(); ++it)
	{
		if (numModels == 1)
		{
			(*it).transform.xOffset = (0.0 - 0.5*((*it).xMax - (*it).xMin)) - (*it).xMin;
			(*it).transform.yOffset = (0.0 - 0.5*((*it).yMax - (*it).yMin)) - (*it).yMin;
			(*it).transform.rotation = 0;
		} 
		else
		{	if (it == b.begin())
			{
				blob.xMin = (*it).xMin + (*it).transform.xOffset;
				blob.xMax = (*it).xMax + (*it).transform.xOffset;
				blob.yMin = (*it).yMin + (*it).transform.yOffset;
				blob.yMax = (*it).yMax + (*it).transform.yOffset;	
			}
			else
			{
				oldBlobXmax = blob.xMax;
				oldBlobYmax = blob.yMax;

				tempXmax = blob.xMax + (*it).xMax - (*it).xMin;
				tempYmax = (blob.yMax > (*it).yMax) ? blob.yMax : (*it).yMax;
				tempBlobH = hypot(tempYmax - blob.yMin, tempXmax - blob.xMin);
				//box tempBlobH = box(blob.xMin, tempXmax, blob.yMin, tempYmax); 

				tempXmax = (blob.xMax > (*it).xMax) ? blob.xMax : (*it).xMax;
				tempYmax = blob.yMax + (*it).yMax - (*it).yMin;
				tempBlobV = hypot(tempYmax - blob.yMin, tempXmax - blob.xMin);
				//box tempBlobV = box(blob.xMin, tempXmax, blob.yMin, tempYmax);

				tempXmax = blob.xMax + (*it).yMax - (*it).yMin;
				tempYmax = (blob.yMax > (*it).xMax) ? blob.yMax : (*it).xMax;
				tempBlobHR = hypot(tempYmax - blob.yMin, tempXmax - blob.xMin);
				//box tempBlobHR = box(blob.xMin, tempXmax, blob.yMin, tempYmax); 

				tempXmax = (blob.xMax > (*it).yMax) ? blob.xMax : (*it).yMax;
				tempYmax = blob.yMax + (*it).xMax - (*it).xMin;
				tempBlobVR = hypot(tempYmax - blob.yMin, tempXmax - blob.xMin);
				//box tempBlobVR = box(blob.xMin, tempXmax, blob.yMin, tempYmax);

				tempBlobPoints = 0.0;
				box innerBoxToCheck(0,0,0,0);
				int index = 0;

				for (std::vector<box>::iterator boxIt= pointsToCheck.begin() ; boxIt != pointsToCheck.end(); ++boxIt)
				{
					tempXmax = (*boxIt).xMin + (*it).xMax - (*it).xMin;
					tempYmax = (*boxIt).yMin + (*it).yMax - (*it).yMin;
					if (tempBlobPoints == 0.0)
					{
						tempBlobPoints = hypot(tempYmax - blob.yMin, tempXmax - blob.xMin);
						index = std::distance(pointsToCheck.begin(), boxIt);
						innerBoxToCheck = pointsToCheck[index];
						innerBoxToCheck.diagonal = tempBlobPoints;
					}
					else
					{
						buffer = hypot(tempYmax - blob.yMin, tempXmax - blob.xMin);
						if (tempBlobPoints > buffer)
						{
							index = std::distance(pointsToCheck.begin(), boxIt);
							innerBoxToCheck = pointsToCheck[index];
							tempBlobPoints = buffer;
							innerBoxToCheck.diagonal = tempBlobPoints;
						}
					}
				}
				std::cout << "innerBox: " << innerBoxToCheck.diagonal<< std::endl;
				innerBoxToCheck.diagonal = (295*195);
				std::cout << "innerBox: " << innerBoxToCheck.diagonal<< std::endl;

				if ((tempBlobH <= tempBlobV)&&(tempBlobH <= tempBlobHR) && (tempBlobH <= tempBlobVR) && (tempBlobH <= innerBoxToCheck.diagonal))
				{
					buffer = blob.xMax;
					(*it).transform.xOffset += blob.xMax;
					blob.xMax +=  (*it).xMax - (*it).xMin;
					if (blob.yMax > (*it).yMax)
					{
						pointsToCheck.push_back(box(buffer,plateWidth,(*it).yMax, plateLength));
					}
					if (blob.yMax < (*it).yMax)
					{
						blob.yMax = (*it).yMax;
						pointsToCheck.push_back(box(blob.xMin, blob.xMax, blob.yMax, plateLength));
					}
					//blob.yMax = (blob.yMax > (*it).yMax) ? blob.yMax : (*it).yMax;

				} 
				else if ((tempBlobV<= tempBlobH)&&(tempBlobV<= tempBlobHR) && (tempBlobV <= tempBlobVR)&& (tempBlobV <= innerBoxToCheck.diagonal))
				{
					buffer = blob.yMax;
					(*it).transform.yOffset += blob.yMax;
					blob.yMax += (*it).yMax - (*it).yMin; 
					if (blob.xMax > (*it).xMax)
					{
						pointsToCheck.push_back(box((*it).xMax,plateWidth,blob.yMax, plateLength));
					}
					if (blob.xMax < (*it).xMax)
					{
						pointsToCheck.push_back(box(blob.xMax, plateLength, blob.yMin, buffer));
						blob.xMax = (*it).xMax;
					}
					blob.xMax = (blob.xMax > (*it).xMax) ? blob.xMax : (*it).xMax;
				}
				else if ((tempBlobHR <= tempBlobV)&&(tempBlobHR <= tempBlobH) && (tempBlobHR <= tempBlobVR)&& (tempBlobHR <= innerBoxToCheck.diagonal))
				{
					buffer = (*it).transform.yOffset;
					(*it).transform.rotation = 1;
					(*it).transform.yOffset = (*it).transform.xOffset;
					(*it).transform.xOffset = buffer + blob.xMax;
					blob.yMax = (blob.yMax > (*it).xMax) ? blob.yMax : (*it).xMax;
					blob.xMax += (*it).yMax - (*it).yMin;
					std::cout << "Rotated HR" <<std::endl;
				}
				else if ((tempBlobVR <= tempBlobV)&&(tempBlobVR <= tempBlobH) && (tempBlobVR <= tempBlobHR) && (tempBlobVR <= innerBoxToCheck.diagonal))
				{
					buffer = (*it).transform.xOffset;
					(*it).transform.rotation = 1;
					(*it).transform.xOffset = (*it).transform.yOffset;
					(*it).transform.yOffset = buffer + blob.yMax;
					blob.yMax += (*it).xMax - (*it).xMin;
					blob.xMax = (blob.xMax > (*it).yMax)? blob.xMax : (*it).yMax;
					std::cout << "Rotated VR" <<std::endl;
				}
				else
				{
					double newXmax = innerBoxToCheck.xMin + (*it).xMax - (*it).xMin;
					double newYmax = innerBoxToCheck.yMin + (*it).yMax - (*it).yMin;
					blob.xMax = (blob.xMax > newXmax) ? blob.xMax : newXmax;
					blob.yMax = (blob.yMax > newYmax) ? blob.yMax : newYmax;
					(*it).transform.xOffset += innerBoxToCheck.xMin;
					(*it).transform.yOffset += innerBoxToCheck.yMin;
					pointsToCheck.clear();

/*
					//buffer = blob.xMax;
					(*it).transform.xOffset += blob.xMax;
					blob.xMax +=  (*it).xMax - (*it).xMin;
					if (blob.yMax > (*it).yMax)
					{
						pointsToCheck.push_back(box(buffer,plateWidth,(*it).yMax, plateLength));
					}
					if (blob.yMax < (*it).yMax)
					{
						blob.yMax = (*it).yMax;
						pointsToCheck.push_back(box(blob.xMin, blob.xMax, blob.yMax, plateLength));
					}
*/
				}
			}
		}
	}
	for (std::vector<box>::iterator it = b.begin() ; it != b.end(); ++it)
	{
			(*it).transform.xOffset -= 0.5*(blob.xMax - blob.xMin);
			(*it).transform.yOffset -= 0.5*(blob.yMax - blob.yMin);	
	}

}
box read_stl(std::string fname)
{
	std::ifstream myFile(fname.c_str(),std::fstream::in | std::fstream::binary);

	char header_info[80] = "";
	char numTriangles[4] = "";
	unsigned long numTrianglesLong = 0x00000000;
	double xMin = 0.0;
	double xMax = 0.0;
	double yMin = 0.0;
	double yMax = 0.0;

	if (myFile) 
	{
		myFile.read(header_info, 80);
	}
	else 
		std::cout << "Error reading from STL" << std::endl;

	if (myFile)
	{
		myFile.read(numTriangles, 4);
		numTrianglesLong = *reinterpret_cast<unsigned long*>(numTriangles) & 0x00000000FFFFFFFF;
	}
	else 
		std::cout <<"Error getting number of triangles. Format Error?" <<std::endl;

	for (int i = 0; i < numTrianglesLong; i++) 
	{
		char face[50];
		if (myFile)
		{
			myFile.read(face,50);
			v3 p1(face+12);
			v3 p2(face+24);
			v3 p3(face+36);
			//v.push_back(tri(p1, p2, p3));

			xMin = (xMin > p1.m_x) ? p1.m_x : xMin;
			xMax = (xMax < p1.m_x) ? p1.m_x : xMax;
			yMin = (yMin > p1.m_y) ? p1.m_y : yMin;
			yMax = (yMax < p1.m_y) ? p1.m_y : yMax;

			xMin = (xMin > p2.m_x) ? p2.m_x : xMin;
			xMax = (xMax < p2.m_x) ? p2.m_x : xMax;
			yMin = (yMin > p2.m_y) ? p2.m_y : yMin;
			yMax = (yMax < p2.m_y) ? p2.m_y : yMax;

			xMin = (xMin > p3.m_x) ? p3.m_x : xMin;
			xMax = (xMax < p3.m_x) ? p3.m_x : xMax;
			yMin = (yMin > p3.m_y) ? p3.m_y : yMin;
			yMax = (yMax < p3.m_y) ? p3.m_y : yMax;
			
		} 
		else 
			std::cout <<"Error reading faces. Corrupted File?" <<std::endl;
	}
	return box(xMin, xMax, yMin, yMax);
}
