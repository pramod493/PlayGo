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

namespace CDI
{
	class RamerDouglas
	{
		const std::pair<int, double> findMaximumDistance(const std::vector<Point2D>& Points)const;

		//"simplifyWithRDP" returns the simplified path with a Point vector. The function takes in the paths to be simplified and a customerized thresholds for the simplication.
	public:
		std::vector<Point2D> simplifyWithRDP(std::vector<Point2D>& Points, double epsilon)const;
	};
}
