#version 460

in float Alpha;

void main()
{
	//ignore transparent obejcts
	if(Alpha != 1.0f) discard;
}