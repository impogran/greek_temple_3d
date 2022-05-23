#version 330

uniform sampler2D textureMap;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
in vec4 l1;
in vec4 l2;
in vec4 n;
in vec4 v;
in float i_nl;
in vec2 i_tc;

void main(void) {
	
	vec4 color = texture(textureMap, i_tc); 

	vec4 ml1 = normalize(l1);
	vec4 ml2 = normalize(l2);

	vec4 mn = normalize(n);
	vec4 mv = normalize(v);

	vec4 mr1 = reflect(-ml1, mn); //Wektor odbity
	vec4 mr2 = reflect(-ml2, mn); //Wektor odbity


	float nl1 = clamp(dot(mn,ml1),0,1); //Kosinus k¹ta pomiêdzy wektorami n i l.
	float nl2 = clamp(dot(mn,ml2),0,1);

	float rv1 = pow(clamp(dot(mr1,mv), 0, 1), 50);// Kosinus k¹ta pomiêdzy wektorami r i v podniesiony do potêgi
	float rv2 = pow(clamp(dot(mr2,mv), 0, 1), 50);

	pixelColor = vec4(nl1*color.rgb + nl2*color.rgb, color.a) + vec4(rv1+rv2,rv1+rv2,rv1+rv2,0); //Wyliczenie modelu oœwietlenia (bez ambient);

}
