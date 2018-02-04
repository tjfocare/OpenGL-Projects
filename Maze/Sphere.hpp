#ifndef SPHERE_H
#define SPHERE_H

class Sphere {
public:

	/**
	 * Generates a new sphere mesh with given radius, vertical sub-divisions and horizontal
	 * sub-divisions. Produces vertex positions, normal vectors, and element
	 * indices available for use in vertex arrays.
	 * More sub-divisions generates a smoother sphere.
	 * It is a good idea to have vertical and horizontal divisions equal.
	 * @param radius, radius of sphere in generic units
	 * @param vertDiv, (min 1), number of vertical divisions (bands)
	 * @param horzDiv, (min 4), number of horizontal divisions (slices)
	 */
	Sphere(float radius = 1.0f, int vertDiv = 16, int horzDiv = 16);

	float *vertices;		// Vertex position (x,y,z)
	float *normals;			// Normal vector (x,y,z)
	unsigned int *indices;	// Element indices

	// Counts of array elements
	int vertCount;
	int normCount;
	int indCount;

};

#endif
