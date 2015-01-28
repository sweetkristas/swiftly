#include <algorithm>

#include "asserts.hpp"
#include "polygon.hpp"
#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"

using namespace ClipperLib;

namespace geometry
{
	namespace 
	{
		struct Rect 
		{
			struct {float x, y;} min;
			struct {float x, y;} max;
		};
	}

	bool isWoundClockwise(const std::vector<p2t::Point>& inputPoly)
	{
		double finalSum = 0;
		size_t i_prev = inputPoly.size() - 1;

		for(size_t i = 0; i < inputPoly.size(); i++)
		{
			// (x2-x1)(y2+y1)
			finalSum += (inputPoly[i].x - inputPoly[i_prev].x) * (inputPoly[i].y + inputPoly[i_prev].y);
			i_prev = i;
		}

		// Negative result = counter-clockwise
		return finalSum < 0 ? false : true;
	}

	bool PolygonContains2p2t(p2t::Point **inVertices, int inNumVertices, const p2t::Point *inPoint)
	{
	   bool oddCount = false;
	   int i;
	   double xinters;
	   const p2t::Point *p1, *p2;

	   p1 = inVertices[inNumVertices-1];

	   for(i = 0; i < inNumVertices; i++)
	   {
		  p2 = inVertices[i];
		  if(inPoint->y > std::min(p1->y, p2->y) && inPoint->y <= std::max(p1->y, p2->y) && inPoint->x <= std::max(p1->x, p2->x) && (p1->y != p2->y))
		  {
			 xinters = (inPoint->y - p1->y) * (p2->x - p1->x) / (p2->y - p1->y) + p1->x;
			 if(p1->x == p2->x || inPoint->x <= xinters)
				oddCount = !oddCount;
		  }

		  p1 = p2;
	   }

	   return oddCount;
	}


	bool processComplex(std::vector<p2t::Point> &outputTriangles, const Rect& bounds, const std::vector<std::vector<p2t::Point> >& polygonList)
	{
	   std::vector<std::vector<p2t::Point*> > polylines;
	   std::vector<std::vector<p2t::Point*> > holes;

	   std::vector<p2t::Point*> outline;

	   float minx = bounds.min.x;  float miny = bounds.min.y;
	   float maxx = bounds.max.x;  float maxy = bounds.max.y;

	   outline.push_back(new p2t::Point(minx, miny));
	   outline.push_back(new p2t::Point(minx, maxy));
	   outline.push_back(new p2t::Point(maxx, maxy));
	   outline.push_back(new p2t::Point(maxx, miny));

	   polylines.push_back(outline);

	   for(size_t i = 0; i < polygonList.size(); i++)
	   {
		  const std::vector<p2t::Point>& currentPoly = polygonList[i];

		  if(isWoundClockwise(currentPoly))
		  {
			 std::vector<p2t::Point*> polyline;
			 for(size_t j = 0; j < currentPoly.size(); j++)
				polyline.push_back(new p2t::Point(currentPoly[j].x, currentPoly[j].y));

			 polylines.push_back(polyline);
		  }
		  else
		  {
			 std::vector<p2t::Point*> hole;
			 for(size_t j = 0; j < currentPoly.size(); j++)
				hole.push_back(new p2t::Point(currentPoly[j].x, currentPoly[j].y));

			 holes.push_back(hole);
		  }
	   }


	   std::vector<p2t::CDT*> cdtRegistry;

	   for(size_t i = 0; i < polylines.size(); i++)
	   {
		  std::vector<p2t::Point*> polyline = polylines[i];

		  p2t::CDT* cdt = new p2t::CDT(polyline);
		  cdtRegistry.push_back(cdt);

		  std::vector<std::vector<p2t::Point*> > filteredHoles;
		  for(size_t k = 0; k < holes.size(); k++)
		  {
			 if(PolygonContains2p2t(&polyline[0], polyline.size(), holes[k][0]))  // &polyline[0]  <- vector to array!
				filteredHoles.push_back(holes[k]);
		  }

		  for(size_t j = 0; j < filteredHoles.size(); j++)
			 cdt->AddHole(filteredHoles[j]);

		  cdt->Triangulate();

		  std::vector<p2t::Triangle*> currentOutput = cdt->GetTriangles();

		  p2t::Triangle *currentTriangle;
		  for(size_t i = 0; i < currentOutput.size(); i++)
		  {
			 currentTriangle = currentOutput[i];
			 outputTriangles.push_back(p2t::Point(currentTriangle->GetPoint(0)->x, currentTriangle->GetPoint(0)->y));
			 outputTriangles.push_back(p2t::Point(currentTriangle->GetPoint(1)->x, currentTriangle->GetPoint(1)->y));
			 outputTriangles.push_back(p2t::Point(currentTriangle->GetPoint(2)->x, currentTriangle->GetPoint(2)->y));
		  }
	   }

	   for(size_t i = 0; i < cdtRegistry.size(); i++)
		  delete cdtRegistry[i];

	   for(size_t i = 0; i < polylines.size(); i++)
	   {
		  std::vector<p2t::Point*> polyline = polylines[i];
		  std::vector<p2t::Point*>::iterator iterator;
		  for(iterator = polyline.begin(); iterator != polyline.end(); ++iterator)
			 delete *iterator;

		  polyline.clear();
	   }

	   if(outputTriangles.size() == 0)
		  return false;

	   return true;
	}

	void generate_triangles(const std::vector<point2d_twip>& points, const rect& bounds, std::vector<float>& triangles)
	{
		std::vector<p2t::Point> triangles_out;
		Rect b2;
		b2.min.x = bounds.x1/20.0f;
		b2.min.y = bounds.y1/20.0f;
		b2.max.x = bounds.x2/20.0f;
		b2.max.y = bounds.y2/20.0f;
		std::vector<std::vector<p2t::Point> > poly_list;
		poly_list.resize(1);
		for(auto p : points) {
			poly_list[0].push_back(p2t::Point(p.x/20.0, p.y/20.0));
		}
		bool have_triangles = processComplex(triangles_out, b2, poly_list);
		ASSERT_LOG(have_triangles, "No triangles in output.");
		for(auto t : triangles_out) {
			triangles.push_back(float(t.x));
			triangles.push_back(float(t.y));
		}
	}
}