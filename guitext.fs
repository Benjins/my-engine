#version 130                                      
out vec4 FragColor;                                
in vec2 uv_coord;                                
uniform sampler2D _mainTex;                          
void main(){                                         
	vec4  _Color = vec4(1,0.5,1,1);                     
	vec4 col2 = texture(_mainTex, uv_coord);        
	FragColor = _Color * vec4(col2.xyz, col2.x);           
}