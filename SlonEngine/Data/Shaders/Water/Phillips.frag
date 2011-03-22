

void main()
{
    float k = floor(linearIndex / numPartitions);
    
    // fftIndex = ( iIndex + k * n ) % N
    vec2 fftIndex = index + direction * k * numPartitions;
	fftIndex -= floor(fftIndex / N) * N;

	// fetch fft coefficents
    vec4 He = texture2DLod(fftInput, fftIndex / N, 0.0);
    vec4 Ho = texture2DLod(fftInput, (fftIndex + direction * numPartitions) / N, 0.0);

	// compute next fft coefficent
    vec2 Wn = W(N / numPartitions, k);
    vec4 rhs = vec4( complex_mult(Wn, Ho.xy), complex_mult(Wn, Ho.zw) ); // vec4(H*W, H*W)
	vec4 result = He + rhs;
			 
	// shift the FFT
	if ( numPartitions == 1.0 ) 
	{
		Wn = W(N, -linearIndex * N / 2.0);
		result.xy = complex_mult(Wn, result.xy); 
		result.zw = complex_mult(Wn, result.zw);
	}
	
	gl_FragColor = result; // H = He + W * Ho
}