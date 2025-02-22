#include "../inc/mean_shift.hpp"

// Additional functions
// --------------------

double euclidean_distance(const Eigen::Vector3d &point_a, const Eigen::Vector3d &point_b){
    double total = 0;
    Eigen::Vector3d tmp = point_a - point_b;
    /*
    for(int i=0; i<point_a.size(); i++){
        total += (point_a[i] - point_b[i]) * (point_a[i] - point_b[i]);
    }
    */
    return tmp.norm();
}

double gaussian_kernel(double distance, double kernel_bandwidth){
    double temp =  exp(-(distance*distance) / (kernel_bandwidth));
    return temp;
}

// Mean-shift specfific functions
// ------------------------------

MeanShift::MeanShift() { set_kernel(NULL); }

MeanShift::MeanShift(double (*_kernel_func)(double,double)) { set_kernel(kernel_func); }

std::vector<Eigen::Vector3d> MeanShift::cluster(std::vector<Eigen::Vector3d> points, double kernel_bandwidth)
{
	std::vector<bool> stop_moving(points.size(), false);
	stop_moving.reserve(points.size());
	std::vector<Eigen::Vector3d> shifted_points = points;
	double max_shift_distance;

	do {
	    max_shift_distance = 0;
	    for(int i=0; i<shifted_points.size(); i++){
	        if (!stop_moving[i]) {
	            Eigen::Vector3d point_new = shift_point(shifted_points[i], points, kernel_bandwidth);
	            double shift_distance = euclidean_distance(point_new, shifted_points[i]);
	            if(shift_distance > max_shift_distance){
	                max_shift_distance = shift_distance;
	            }
	            if(shift_distance <= EPSILON) {
	                stop_moving[i] = true;
	            }
	            shifted_points[i] = point_new;
	        }
	    }
	    //printf("max_shift_distance: %f\n", max_shift_distance);
	} while (max_shift_distance > EPSILON);

	return shifted_points;
} // Fin de la funcion cluster

void MeanShift::set_kernel(double (*_kernel_func)(double,double))
{
//	if(!_kernel_func){
	kernel_func = gaussian_kernel;
//   } else {
//       kernel_func = _kernel_func;
//   }
}

Eigen::Vector3d MeanShift::shift_point(const Eigen::Vector3d &point, const std::vector< Eigen::Vector3d > &points, double kernel_bandwidth)
{
	Eigen::Vector3d shifted_point = point;
	for(int dim = 0; dim<shifted_point.size(); dim++){
	    shifted_point[dim] = 0;
	}

	double total_weight = 0;
	for(int i=0; i<points.size(); i++){
	    Eigen::Vector3d temp_point = points[i];
	    double distance = euclidean_distance(point, temp_point);
	    double weight = kernel_func(distance, kernel_bandwidth);

	    for(int j=0; j<shifted_point.size(); j++){
	        shifted_point[j] += temp_point[j] * weight;
	    }

	    total_weight += weight;
	}

	for(int i=0; i<shifted_point.size(); i++){
	    shifted_point[i] /= total_weight;
	}

	return shifted_point;
} // Fin de la funcion shift_point
