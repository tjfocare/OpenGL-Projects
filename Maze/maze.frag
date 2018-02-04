#version 330

in vec4 pos;
out vec4 fragColour;

uniform bool surf;
uniform bool sphere;

void main(void)
{
	vec4 floorClr = vec4(0.05, 0.5, 0.0, 1.0);
	vec4 topClr = vec4(1.0, 0.4, 0.0, 1.0);
	vec4 sideClr = vec4(0.37, 0.18, 0.0, 1.0);
	vec4 sphereClr = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 borderClr = vec4(0.0, 0.0, 0.0, 1.0);

	if(surf && !sphere) {
		fragColour = vec4(0.05, 0.5, 0.0, 1.0);

	} else if(!surf && !sphere) {
		fragColour = vec4(0.37, 0.18, 0.0, 1.0);

		//top surface of boxes
		if ((pos.y) >= 0.95) {
			fragColour = vec4(1.0, 0.4, 0.0, 1.0);
		}

	} else if(sphere) {
		fragColour = vec4(1.0, 0.0, 0.0, 1.0);
	}

	//set borders
  if ((abs(pos.x) >= 0.95) && (abs(pos.y) >= 0.95) ||
      (abs(pos.x) >= 0.95) && (abs(pos.z) >= 0.95) ||
      (abs(pos.y) >= 0.95) && (abs(pos.z) >= 0.95) )
  {
			fragColour = vec4(0.0, 0.0, 0.0, 1.0);
	}

}
