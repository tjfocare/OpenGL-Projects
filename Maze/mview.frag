#version 330

in vec4 pos; // Interpolated vertex position, in object coords

// The final colour we will see at this location on screen
out vec4 fragColour;

void main(void)
{
	vec4 sideClr = vec4(0.8, 0.6, 0.2, 1.0);
	vec4 topClr = vec4(0.2, 0.6, 0.6, 1.0);

	// We will ignore vertex colour this time
	// Draw the top of the cube a different colour to the rest

	if (pos.y >= 0.95) {
		fragColour = topClr;
	}
	else {
		fragColour = sideClr;
	}

}
