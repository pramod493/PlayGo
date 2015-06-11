#include "ramerdouglaspeucker.h"


using std::vector;
//Given an array of points, "findMaximumDistance" calculates the GPS point which have largest distance from the line formed by first and last points in RDP algorithm. Returns the index of the point in the array and the distance.
namespace CDI
{
	const std::pair<int, float> RamerDouglas::findMaximumDistance(const std::vector<Point2D> &Points)const{
		Point2D firstpoint=Points[0];
		Point2D lastpoint=Points[Points.size()-1];
		int index=0;  //index to be returned
		float Mdist=-1; //the Maximum distance to be returned

		//distance calculation
		Point2D p=lastpoint-firstpoint;
		for(int i=1;i<Points.size()-1;i++){ //traverse through second point to second last point
			Point2D pp=Points[i]-firstpoint;
			float Dist=abs(crossProduct(&pp,&p)) / magnitude(&p); //formula for point-to-line distance
			if (Dist>Mdist){
				Mdist=Dist;
				index=i;
			}
		}
		return std::make_pair(index, Mdist);
	}

	const std::pair<int, float> RamerDouglas::findMaximumDistance(const std::vector<p2t::Point> &Points)const{
		p2t::Point firstpoint=Points[0];
		p2t::Point lastpoint=Points[Points.size()-1];
		int index=0;  //index to be returned
		float Mdist=-1; //the Maximum distance to be returned

		//distance calculation
		p2t::Point p= p2t::Point(lastpoint.x-firstpoint.x, lastpoint.y-firstpoint.y);
		for(int i=1;i<Points.size()-1;i++){ //traverse through second point to second last point
			p2t::Point pp=p2t::Point(Points[i].x-firstpoint.x, Points[i].y-firstpoint.y);
			//float Dist=abs(crossProduct(&pp,&p)) / magnitude(&p); //formula for point-to-line distance
			float Dist = abs(pp.x * p.y - pp.y * p.x) / sqrt(p.x * p.x + p.y * p.y);
			if (Dist>Mdist){
				Mdist=Dist;
				index=i;
			}
		}
		return std::make_pair(index, Mdist);
	}


	vector<Point2D> RamerDouglas::simplifyWithRDP(vector<Point2D>& Points, float epsilon)const{
		if(Points.size()<3){  //base case 1
			return Points;
		}
		std::pair<int, float> maxDistance=findMaximumDistance(Points);
		if(maxDistance.second>=epsilon){
			int index=maxDistance.first;
			vector<Point2D>::iterator it=Points.begin();
			vector<Point2D> path1(Points.begin(),it+index+1); //new path l1 from 0 to index
			vector<Point2D> path2(it+index,Points.end()); // new path l2 from index to last

			vector<Point2D> r1 =simplifyWithRDP(path1,epsilon);
			vector<Point2D> r2=simplifyWithRDP(path2,epsilon);

			//Concat simplified path1 and path2 together
			vector<Point2D> rs(r1);
			rs.pop_back();
			rs.insert(rs.end(),r2.begin(),r2.end());
			return rs;
		}
		else { //base case 2, all points between are to be removed.
			vector<Point2D> r(1,Points[0]);
			r.push_back(Points[Points.size()-1]);
			return r;
		}
	}
	
	vector<p2t::Point> RamerDouglas::simplifyWithRDP(vector<p2t::Point>& Points, float epsilon)const{
		if(Points.size()<3){  //base case 1
			return Points;
		}
		std::pair<int, float> maxDistance=findMaximumDistance(Points);
		if(maxDistance.second>=epsilon){
			int index=maxDistance.first;
			vector<p2t::Point>::iterator it=Points.begin();
			vector<p2t::Point> path1(Points.begin(),it+index+1); //new path l1 from 0 to index
			vector<p2t::Point> path2(it+index,Points.end()); // new path l2 from index to last

			vector<p2t::Point> r1 =simplifyWithRDP(path1,epsilon);
			vector<p2t::Point> r2=simplifyWithRDP(path2,epsilon);

			//Concat simplified path1 and path2 together
			vector<p2t::Point> rs(r1);
			rs.pop_back();
			rs.insert(rs.end(),r2.begin(),r2.end());
			return rs;
		}
		else { //base case 2, all points between are to be removed.
			vector<p2t::Point> r(1,Points[0]);
			r.push_back(Points[Points.size()-1]);
			return r;
		}
	}
}
