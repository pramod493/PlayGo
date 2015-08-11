// Attribution:
// https://github.com/Yongjiao/Ramer-Douglas-Peucker-Algorithm
// Modified to suit this code

#pragma once
#include "commonfunctions.h"
#include "point2dpt.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include "poly2tri.h"	// For less conversion required for poly2tri

namespace CDI
{
	class RamerDouglas
	{
		const std::pair<int, float> findMaximumDistance(const std::vector<Point2D>& Points)const;
		
		const std::pair<int, float> findMaximumDistance(const std::vector<p2t::Point>& Points) const;

		//"simplifyWithRDP" returns the simplified path with a Point vector. The function takes in the paths to be simplified and a customized thresholds for the simplification.
	public:
		std::vector<Point2D> simplifyWithRDP(std::vector<Point2D>& Points, float epsilon)const;
		std::vector<p2t::Point> simplifyWithRDP(std::vector<p2t::Point>& Points, float epsilon)const;
	};
}
