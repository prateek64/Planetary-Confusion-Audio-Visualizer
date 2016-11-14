#include "ofApp.h"


using namespace stk; //  For STK
//--------------------------------------------------------------
void ofApp::setup(){

	
	ofSetFrameRate(60);  // Sets the framerate of the project 
	
    fftlive_setup();    // Sets up the fftlive addon process
	left.resize(buffersize); // Stores the audio data coming in on the left microphone 

	leftHistory.resize(1, left);  // This saves the data from previous buffers (in this case there is only one past buffer)

    initialize_granular_synth();  

	meshOriginal = meshWarped = ofMesh::sphere(200, 14); // A  sphere of ofMesh type
	

	// ofSetVerticalSync(true);
    ofSetFullscreen(true);  
	ofEnableDepthTest();
	ofBackground(0); // Black Background 
	cigarette.set(30, 700);  // Sets the cigarette 
	//cam.setDistance(1000);

	sound_stream.printDeviceList();   // Prints the input and output devices available 

	left.assign(buffersize, 0.0); // Assigns the first buffersize values equal to zero 
	
	sound_stream.setup(this,2, 1, 44100, buffersize, 2); // 2 output and 1 input channel with a sampling rate of 44100
    
	small_sphere.setRadius(100);
	big_sphere.setRadius(big_radius);

	texture.loadImage(ofToDataPath("world3.jpg"));  // Loads image to be used for texturing 
	big_sphere.mapTexCoords(0, 0, texture.getWidth(), texture.getHeight()); // Maps image to the sphere (Wraps around the sphere)

	earth.loadImage(ofToDataPath("saturn.jpg"));
	small_sphere.mapTexCoords(0, 0, earth.getWidth(), earth.getHeight());
	
	
	cigar.loadImage(ofToDataPath("cigar_3.jpg"));
	cigarette.mapTexCoords(0, 0, cigar.getWidth(), cigar.getHeight());

	fftSmoothed = new float[1024];  // FFT array 

	for (int i = 0; i < 1024; i++) {

		fftSmoothed[i] = 0;

	}

	nBandsToGet = 256;

	
	death_star();  // Initializes the death star visual component 

	
}

// Initializes the granular synthesis object 

void ofApp::initialize_granular_synth() {


	grain_synth.reset();
	grain_synth.openFile(ofToDataPath("ed.wav"),true);
	grain_synth.setVoices(no_of_voices); 
	grain_synth.setStretch(stretch_factor);
	grain_synth.setGrainParameters(grain_length,10,20,30);
	grain_synth.setRandomFactor(1);
	
}


// Audio out callback function 
// Here the audio out are the granulated song and the original song. 

void ofApp::audioOut(float * output, int bufferSize, int nChannels) {

	
	for (int i = 0; i < buffersize*nChannels; i = i+2) {

		if (activate_grains) {
			
			float value = grain_synth.tick(); // Granulated song 
			output[i] = scaledVol*value;
			output[i + 1] = (1-scaledVol)*value;
			

		} 
		
		
	}
} 


// Function to live update the fft
void ofApp::fftlive_update() {

	fftLive.setThreshold(audioThreshold);  // Audio Thershold 
	fftLive.setPeakDecay(audioPeakDecay); 
	fftLive.setMaxDecay(audioMaxDecay);
	fftLive.setMirrorData(true);
	fftLive.update();

	
	//---------------------------------------------------------- dispacing mesh using audio.
	vector<ofVec3f> & vertsOriginal = meshOriginal.getVertices();
	vector<ofVec3f> & vertsWarped = meshWarped.getVertices();
	int numOfVerts = meshOriginal.getNumVertices();

	float * audioData = new float[numOfVerts];
	fftLive.getFftPeakData(audioData, numOfVerts);

	float meshDisplacement = 100;


	// For loop that warps the meshed star according to the fft of the data 

	for (int i = 0; i<numOfVerts; i++) {
		float audioValue = audioData[i];
		ofVec3f & vertOriginal = vertsOriginal[i];
		ofVec3f & vertWarped = vertsWarped[i];

		ofVec3f direction = vertOriginal.getNormalized();
		vertWarped = vertOriginal + direction * meshDisplacement * audioValue;
	}

	delete[] audioData;

}

void ofApp::fftlive_setup() {

	fftLive.setMirrorData(true);
	fftLive.setup();

}

// Draws the death star 
void ofApp::death_star() {

	nTri = 600;			//The number of the triangles
	nVert = nTri * 3;		//The number of the vertices

		//The sphere's radius
		//Maximal triangle's “radius”
					//(formally, it's the maximal coordinates'
					//deviation from the triangle's center)

					//Fill the vertices array
	vertices.resize(nVert);		//Set the array size
	for (int i = 0; i<nTri; i++) {	//Scan all the triangles
									//Generate the center of the triangle
									//as a random point on the sphere

									//Take the random point from
									//cube [-1,1]x[-1,1]x[-1,1]	
		ofPoint center(ofRandom(-1, 1),
			ofRandom(-1, 1),
			ofRandom(-1, 1));
		center.normalize(); //Normalize vector's length to 1
		center *= Rad;	//Now the center vector has
						//length Rad

						//Generate the triangle's vertices
						//as the center plus random point from
						//[-rad, rad]x[-rad, rad]x[-rad, rad]
		for (int j = 0; j<3; j++) {
			vertices[i * 3 + j] =
				center + ofPoint(ofRandom(-rad, rad),
					ofRandom(-rad, rad),
					ofRandom(-rad, rad));
		}
	}

	//Fill the array of triangles' colors
	colors.resize(nTri);
	for (int i = 0; i<nTri; i++) {
		//Take a random color from black to red
		colors[i] = ofColor(30, ofRandom(0, 200), ofRandom(0, 255));
	}


}


// Audio Input Callback function 

void ofApp::audioIn(float * input, int bufferSize, int nChannels) {

	float curVol = 0.0;

	// samples are "interleaved"
	int numCounted = 0;

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++) {
		left[i] = input[i];
		curVol += left[i] * left[i];
		
		numCounted += 1;
	}

	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

	
    bufferCounter++;
	

} 


//--------------------------------------------------------------
void ofApp::update(){
	cout << ofToString(ofGetFrameRate()) << endl;
	
	fftlive_update();
	ofSoundUpdate();
	
	leftHistory.push_back(left);
	leftHistory.erase(leftHistory.begin(), leftHistory.begin() + 1);
	

	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	cylinder_rotation_angle_Y += scaledVol*2;
	cylinder_rotation_angle_Z += scaledVol*2; 

	signal_rot_x += scaledVol * 4;
	signal_rot_y += scaledVol * 4;

	death_star_angle_X += scaledVol * 2.5;
	death_star_angle_Y += scaledVol * 3;
	death_star_angle_Z += scaledVol * 2.5;



	ball_rotation_angle += scaledVol*6;
	
	
	float * val = ofSoundGetSpectrum(nBandsToGet);		// request  values for fft
	for (int i = 0; i < nBandsToGet; i++) {

		// let the smoothed calue sink to zero:
		fftSmoothed[i] *= 0.96f;

		// take the max, either the smoothed or the incoming:
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];

	} 

	Rad = 400 * scaledVol*3.5;



	 
}

// Sets the ring color for saturn 

void ofApp::set_ring_color(int index) {


	 if (index < nBandsToGet / 4) {

		ofSetColor(0,200,200);

	}

	 else if (index > nBandsToGet / 4 && index <= nBandsToGet / 2) {

		 ofSetColor(0,255,0);
	 }

	else if (index > nBandsToGet / 2 && index <= 3*nBandsToGet/4) {

		ofSetColor(0,0,255);
	}

	else if (index > 3 * nBandsToGet / 4 && index <= nBandsToGet) {

		ofSetColor(255,0,0);
	}


}

//--------------------------------------------------------------
void ofApp::draw(){

	ofColor cylinder_color;

	ofPushMatrix();

		ofTranslate(ofGetWindowWidth()*0.5, ofGetWindowHeight()*0.5);
		cam.begin();
		ofPushMatrix();
		
			ofRotateY(cylinder_rotation_angle_Y);  // Rotates in Y direction 
			ofRotateZ(cylinder_rotation_angle_Z);  // Rotates in Z direction 

			ofPushStyle();

			    texture.getTextureReference().bind();  // Binds the image textures onto the spheres (so they look like planets)
				big_sphere.draw();
				texture.unbind();

			ofPopStyle();

			ofPushMatrix();

				ofRotateY(ball_rotation_angle);
				ofTranslate(700, 0, 0);

				ofRotateZ(ball_rotation_angle);

				int new_ring_x =0, new_ring_y=0;

				ofPushStyle();
				
					float width = (float)(5 * 128) / nBandsToGet;
					int sign_shift = 1;

					// We draw the saturn rings over here

					for (int l = 0; l < 20; l++) {

						last_ring_x = 100 + 6*l+ rings_radius;
						last_ring_y = 0;
					

						for (int i = 0; i <nBandsToGet; i++) {

							new_ring_x = (rings_radius + 100 +6*l)*cos(i * 2 * PI /(0.1*nBandsToGet));
							new_ring_y = (rings_radius + 100 +6*l )*sin(i * 2 * PI / (0.1*nBandsToGet));
							new_ring_z = sign_shift * 175 *fftSmoothed[i];
					        
							set_ring_color(i);
							
							ofDrawLine(last_ring_x, last_ring_y,last_ring_z, new_ring_x, new_ring_y, new_ring_z);

							last_ring_x = new_ring_x;
							last_ring_y = new_ring_y;
							last_ring_z = new_ring_z;
							sign_shift = -sign_shift;

						}
					}

				ofPopStyle();

				ofPushStyle();

					//ofSetColor(0, 0, 256);
				    earth.getTextureReference().bind();
					small_sphere.draw();
					earth.unbind();

				ofPopStyle();

				

			ofPopMatrix();
			
	
		ofPushStyle();

			
			ofRotateX(signal_rot_x);
			// ofTranslate(0, -650, 0);
 			ofSetLineWidth(3);

			
			ofNoFill();	

			int new_point_x, new_point_y;
			    
			for (int k = 0; k < 30; k++) {

				// Make a vertex for each sample value
				last_point_x = 0;
				last_point_y = 0;

				// We draw the magnetic field over here 

				for (unsigned int i = 0; i < buffersize; i++) {

					ofSetColor(rand()%255, rand()%50 ,rand()%50);
					new_point_x = (big_radius + (big_radius)*sin(PI*i / buffersize) + vol_scale/5*waveform_amp*leftHistory[0][i])*cos(PI*i / buffersize);
					new_point_y = (big_radius + (big_radius/6)*sin(PI*i / buffersize) + vol_scale/5*waveform_amp*leftHistory[0][i])*sin(PI*i / buffersize);

					ofDrawLine(last_point_x, last_point_y, new_point_x, new_point_y);

					last_point_x = new_point_x;
					last_point_y = new_point_y;

				}

				ofRotateX(360 /30);

			}

	
		ofPopStyle();

		
	
		ofPopMatrix();


		moon.loadImage(ofToDataPath("moon.jpg"));
		// sphere.mapTexCoords(0, 0, moon.getWidth(), moon.getHeight());



		ofPushMatrix();

			ofRotateY(cylinder_rotation_angle_Y+120);
			ofRotateZ(cylinder_rotation_angle_Z+60);

			ofPushStyle();
			cigarette.setResolution(20, 13, 4);

			cigar.getTextureReference().bind();
			cigarette.draw();
			cigar.unbind();
			ofPopStyle();

			ofPushMatrix();

				ofRotateY(ball_rotation_angle);
				ofTranslate(600, 0, 0);
				ofRotateZ(ball_rotation_angle);

				ofPushStyle();

				    ofSetColor(0, 200, 200);
					meshWarped.drawWireframe();
				

				ofPopStyle();

			ofPopMatrix();


		ofPopMatrix();

		//Enable z-buffering
		glEnable(GL_DEPTH_TEST);

		//Set a gradient background from white to gray
		//for adding an illusion of visual depth to the scene
	//	ofBackgroundGradient(ofColor(255), ofColor(128));

		ofPushMatrix();	//Store the coordinate system

						//Move the coordinate center to screen's center
	//	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);

		//Calculate the rotation angle
	//	float time = ofGetElapsedTimef();	//Get time in seconds
	//	float angle = time * 10;			//Compute angle. We rotate at speed 10 degrees per second
	
		ofRotateX(-death_star_angle_X);
		ofRotateY(-death_star_angle_Y);
		ofRotateZ(-death_star_angle_Z);
		
		
		death_star();

		
		ofPushStyle();
											//Draw the triangles
			for (int i = 0; i<nTri; i++) {

				ofSetColor(colors[i]);	//Set color
				ofTriangle(vertices[i * 3],
				vertices[i * 3 + 1],
				vertices[i * 3 + 2]); //Draw triangle

			}

		ofPopStyle();

		ofPopMatrix();	//Restore the coordinate system

		cam.end();
		
	ofPopMatrix();

	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

// Plays the song on up key press 

	if (key == OF_KEY_UP) {

		my_beats.load("ed.wav");
		my_beats.play();
		grain_synth.reset();
		
	}

	if (key == OF_KEY_DOWN) {

		my_beats.stop();
		activate_grains = true;
	}

	// Modifies grain data on right key press
	if (key == OF_KEY_RIGHT) {

		no_of_voices = no_of_voices - grain_param_control;
		grain_length = grain_length - 2*grain_param_control;
		stretch_factor = stretch_factor - grain_param_control / 2;
		grain_synth.setVoices(no_of_voices);
		grain_synth.setStretch(1);
		grain_synth.setGrainParameters(grain_length, 5, 5, 10);
		grain_synth.setRandomFactor(0.3);

		cout << "Grain Length : " << grain_length << endl;
		cout << "Number of Voices : " << no_of_voices << endl;
		cout << "Stretch Factor : " << stretch_factor << endl;

	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}