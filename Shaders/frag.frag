void main (void)  
{     
   gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);  
} 


crap copied from shader string
		vec3 lightVector = normalize(lightPosition - position);
		vColor = aColor * dot(lightVector, aNormal);

		vec3 lightVector = normalize(lightPosition - aPosition);
		vColor = aColor * dot(lightVector, aNormal);



	11:23
				gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
		//vec3 lightPosition = vec3(1., 2., 0.);
		//vec3 lightVector = normalize(lightPosition - aPosition.xyz);
		vec3 lightVector = vec3(0., 1., 0.);
		brightness = dot(lightVector, aNormal);
		vec 4 lighting = vec4(brightness, brightness, brightness, 1.);
		if (brightness < .3)
			minimum = true;
		vec4 minColor = vec4(.3, .3, .3, .3);
		vColor = minimum ? minColor : brightness;
		//vColor = aColor * dot(lightVector, aNormal);