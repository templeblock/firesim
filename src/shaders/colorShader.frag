#version 330 core
 


void main()
{
	//Source for M: http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
	mat3 M = mat3(
					0.4002400, -0.2263000, 0.0000000, // first column (not row!)
					0.7076000, 1.1653200, 0.0000000, // second column
				   -0.0808100, 0.0457000, 0.9182200  // third column
				);
	mat3 M_inv = mat3(
					1.8599364, 0.3611914, 0.0000000, // first column 
					-1.1293816, 0.6388125, 0.0000000, // second column
				   0.2198974, -0.0000064, 1.0890636  // third column
				);
	
	float Xr = raw.x;
	float Yr = raw.y;
	float Zr = raw.z;

	//Assuming white is hottest point
	float Lw = white.x;
	float Mw = white.y;
	float Sw = white.z;

	mat3 colorScale  =	mat3(
					1.0/Lw, 0.0, 0.0, // first column
					0.0, 1.0/Mw, 0.0, // second column
					0.0, 0.0, 1.0/Sw  // third column
					);
				
	xyz_adjusted = invM * colorScale * M * raw;
}