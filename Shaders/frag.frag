void main (void)  
{     
   gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);  
} 


crap copied from shader string
		vec3 lightVector = normalize(lightPosition - position);
		vColor = aColor * dot(lightVector, aNormal);

		vec3 lightVector = normalize(lightPosition - aPosition);
		vColor = aColor * dot(lightVector, aNormal);